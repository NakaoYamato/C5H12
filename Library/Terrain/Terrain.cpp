#include "Terrain.h"

#include "../HRTrace.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Graphics/GpuResourceManager.h"
#include "../../Library/Collision/CollisionMath.h"
#include "../../Library/DebugSupporter/DebugSupporter.h"
#include "../../Library/Algorithm/Converter.h"

#include <Mygui.h>

Terrain::Terrain(ID3D11Device* device, const std::string& serializePath) :
	_serializePath(serializePath)
{
    // 地形メッシュの頂点とインデックスを生成
    CreateTerrainMesh(device);

    // 定数バッファ作成
    GpuResourceManager::CreateConstantBuffer(
        device,
        sizeof(ConstantBuffer),
        _constantBuffer.ReleaseAndGetAddressOf());

    D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        {   "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    // 頂点シェーダー
    GpuResourceManager::CreateVsFromCso(
        device,
        "./Data/Shader/TerrainVS.cso",
        _vertexShader.ReleaseAndGetAddressOf(),
        _inputLayout.ReleaseAndGetAddressOf(),
        inputElementDesc,
        static_cast<UINT>(_countof(inputElementDesc)));
    // ハルシェーダー
    GpuResourceManager::CreateHsFromCso(
        device,
        "./Data/Shader/TerrainHS.cso",
        _hullShader.ReleaseAndGetAddressOf());
    // ドメインシェーダー
    GpuResourceManager::CreateDsFromCso(
        device,
        "./Data/Shader/TerrainDS.cso",
        _domainShader.ReleaseAndGetAddressOf());
    // ピクセルシェーダー
    GpuResourceManager::CreatePsFromCso(
        device,
        "./Data/Shader/TerrainPS.cso",
        _pixelShader.ReleaseAndGetAddressOf());
    // GBufferへ書き込む用のピクセルシェーダー
    GpuResourceManager::CreatePsFromCso(
        device,
        "./Data/Shader/TerrainGBPS.cso",
        _gbPixelShader.ReleaseAndGetAddressOf());

    // マテリアルマップ作成
	_materialMapFB = std::make_unique<FrameBuffer>(
		device,
        MaterialMapSize, MaterialMapSize, true,
		std::vector<DXGI_FORMAT>(
            {   DXGI_FORMAT_R16G16B16A16_FLOAT, 
                DXGI_FORMAT_R16G16B16A16_FLOAT })
    );
    // パラメータマップ作成
    _parameterMapFB = std::make_unique<FrameBuffer>(
        device, 
        ParameterMapSize, ParameterMapSize, true);

    // ストリームアウトを使用してプリミティブを取得するシェーダー
    {
        D3D11_SO_DECLARATION_ENTRY declaration[] =
        {
            {0,"SV_POSITION",	0,0,4,0},
            {0,"WORLD_POSITION",0,0,3,0},
            {0,"NORMAL",		0,0,3,0},
            {0,"TEXCOORD",		0,0,2,0},
			{0,"COST",          0,0,1,0},
        };
        UINT bufferStrides[] = { sizeof(StreamOutVertex) };
        GpuResourceManager::CreateGsWithStreamOutFromCso(
            device,
            "./Data/Shader/TerrainGS.cso",
            _streamOutGeometryShader.ReleaseAndGetAddressOf(),
            declaration, _countof(declaration),
            bufferStrides, _countof(bufferStrides),
            0);
    }

    // ストリームアウトプットされた情報の受け取り用バッファ
    {
        D3D11_BUFFER_DESC vBufferDesc{};
        vBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        vBufferDesc.ByteWidth = sizeof(StreamOutVertex) * StreamOutMaxVertex;
        vBufferDesc.BindFlags = D3D11_BIND_STREAM_OUTPUT;
        vBufferDesc.CPUAccessFlags = 0;
        vBufferDesc.MiscFlags = 0;
        vBufferDesc.StructureByteStride = 0;

        HRESULT hr = device->CreateBuffer(&vBufferDesc, nullptr,
            _streamOutVertexBuffer.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
    }

    // CPUからアクセサするためのバッファ
    {
        D3D11_BUFFER_DESC vBufferDesc{};
        vBufferDesc.Usage = D3D11_USAGE_STAGING;
        vBufferDesc.ByteWidth = sizeof(StreamOutVertex) * StreamOutMaxVertex;
        vBufferDesc.BindFlags = 0;
        vBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        vBufferDesc.MiscFlags = 0;
        vBufferDesc.StructureByteStride = 0;

        HRESULT hr = device->CreateBuffer(&vBufferDesc, nullptr,
            _streamOutCopyBuffer.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
    }

    // データの読み込み
	LoadFromFile(serializePath);
	// 各テクスチャのパスが設定されている場合はテクスチャをロード
	if (!_baseColorTexturePath.empty())
	{
        GpuResourceManager::LoadTextureFromFile(device,
			_baseColorTexturePath.c_str(), 
			_loadBaseColorSRV.ReleaseAndGetAddressOf(),
            nullptr);
	}
	if (!_normalTexturePath.empty())
	{
		GpuResourceManager::LoadTextureFromFile(device,
			_normalTexturePath.c_str(),
			_loadNormalSRV.ReleaseAndGetAddressOf(),
			nullptr);
	}
	if (!_parameterTexturePath.empty())
	{
		GpuResourceManager::LoadTextureFromFile(device,
			_parameterTexturePath.c_str(),
			_loadParameterSRV.ReleaseAndGetAddressOf(),
			nullptr);
	}
	// テクスチャのロードフラグを立てる
	_isLoadingTextures = true;
}
// 描画処理
void Terrain::Render(TextureRenderer& textureRenderer, const RenderContext& rc, const DirectX::XMFLOAT4X4& world, bool writeGBuffer)
{
    ID3D11DeviceContext* dc = rc.deviceContext;

	// マテリアルマップ、パラメータマップのリセット
    if (_resetMap)
    {
        _materialMapFB->Clear(BaseColorTextureIndex, dc, Vector4::White);
        _materialMapFB->Clear(NormalTextureIndex, dc, Vector4::Blue);
        _parameterMapFB->Clear(dc, Vector4::Black);
        _resetMap = false;
    }

	// テクスチャがロードされている場合は各マップに書き込む
    if (_isLoadingTextures)
    {
		if (_loadBaseColorSRV)
		{
			_materialMapFB->ClearAndActivate(BaseColorTextureIndex, dc);
			textureRenderer.Blit(dc, _loadBaseColorSRV.GetAddressOf(), 0, 1);
			_materialMapFB->Deactivate(dc);
			_loadBaseColorSRV.Reset();
		}
		if (_loadNormalSRV)
		{
			_materialMapFB->ClearAndActivate(NormalTextureIndex, dc);
			textureRenderer.Blit(dc, _loadNormalSRV.GetAddressOf(), 0, 1);
			_materialMapFB->Deactivate(dc);
			_loadNormalSRV.Reset();
		}
		if (_loadParameterSRV)
		{
			_parameterMapFB->ClearAndActivate(dc);
			textureRenderer.Blit(dc, _loadParameterSRV.GetAddressOf(), 0, 1);
			_parameterMapFB->Deactivate(dc);
			_loadParameterSRV.Reset();
		}
		_isLoadingTextures = false;
    }

    if (writeGBuffer)
        dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::MultipleRenderTargets), nullptr, 0xFFFFFFFF);
    else
        dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::Opaque), nullptr, 0xFFFFFFFF);
    // 定数バッファ設定
    _data.world = world;
    dc->UpdateSubresource(_constantBuffer.Get(), 0, 0, &_data, 0, 0);
    dc->VSSetConstantBuffers(ModelCBIndex, 1, _constantBuffer.GetAddressOf());
    dc->HSSetConstantBuffers(ModelCBIndex, 1, _constantBuffer.GetAddressOf());
    dc->DSSetConstantBuffers(ModelCBIndex, 1, _constantBuffer.GetAddressOf());
    dc->GSSetConstantBuffers(ModelCBIndex, 1, _constantBuffer.GetAddressOf());
    dc->PSSetConstantBuffers(ModelCBIndex, 1, _constantBuffer.GetAddressOf());
    // シェーダー設定
    dc->IASetInputLayout(_inputLayout.Get());
    dc->VSSetShader(_vertexShader.Get(), nullptr, 0);
    dc->HSSetShader(_hullShader.Get(), nullptr, 0);
    dc->DSSetShader(_domainShader.Get(), nullptr, 0);
    if (writeGBuffer)
        dc->PSSetShader(_gbPixelShader.Get(), nullptr, 0);
    else
        dc->PSSetShader(_pixelShader.Get(), nullptr, 0);
    if (_streamOut)
    {
        dc->GSSetShader(_streamOutGeometryShader.Get(), nullptr, 0);
        //	ストリームアウト用の頂点バッファを設定
        UINT streamoutOffsets[1] = { 0 };
        dc->SOSetTargets(1, _streamOutVertexBuffer.GetAddressOf(), streamoutOffsets);
    }
    else
    {
        dc->GSSetShader(nullptr, nullptr, 0);
    }
    // 地形用テクスチャ設定
    ID3D11ShaderResourceView* srvs[] =
    {
        _materialMapFB->GetColorSRV(BaseColorTextureIndex).Get(),
        _materialMapFB->GetColorSRV(NormalTextureIndex).Get(),
    };
    dc->PSSetShaderResources(0, _countof(srvs), srvs);
    // パラメータマップ設定
    dc->DSSetShaderResources(ParameterMapIndex, 1, _parameterMapFB->GetColorSRV().GetAddressOf());
    dc->PSSetShaderResources(ParameterMapIndex, 1, _parameterMapFB->GetColorSRV().GetAddressOf());
    dc->GSSetShaderResources(ParameterMapIndex, 1, _parameterMapFB->GetColorSRV().GetAddressOf());
    // 頂点バッファとインデックスバッファを設定
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    dc->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);
    dc->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
    // 描画（平面描画で三角形が2枚あるためIndexCountが6）
    dc->DrawIndexed(static_cast<UINT>(6), 0, 0);

    // シェーダーリソースビューを解除
    ID3D11ShaderResourceView* nullSRVs[] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
    dc->PSSetShaderResources(0, _countof(nullSRVs), nullSRVs);
    dc->DSSetShaderResources(ParameterMapIndex, 1, nullSRVs);
    dc->PSSetShaderResources(ParameterMapIndex, 1, nullSRVs);
    dc->GSSetShaderResources(ParameterMapIndex, 1, nullSRVs);
    // シェーダーを解除
    dc->VSSetShader(nullptr, nullptr, 0);
    dc->HSSetShader(nullptr, nullptr, 0);
    dc->DSSetShader(nullptr, nullptr, 0);
    dc->GSSetShader(nullptr, nullptr, 0);
    dc->PSSetShader(nullptr, nullptr, 0);
    if (_streamOut)
    {
        _streamOut = false;

        // 受け取りバッファをクリア
        ID3D11Buffer* clear_streamout_buffer[] = { nullptr };
        UINT streamout_offsets[1] = { 0 };
        dc->SOSetTargets(1, clear_streamout_buffer, streamout_offsets);

        // CPUからアクセスするためのバッファにコピー
        dc->CopyResource(_streamOutCopyBuffer.Get(),
            _streamOutVertexBuffer.Get());

        // 取得した頂点情報から草の生える位置を算出
        D3D11_MAPPED_SUBRESOURCE mapped_resource;
        dc->Map(_streamOutCopyBuffer.Get(), 0,
            D3D11_MAP_READ, 0, &mapped_resource);
        if (mapped_resource.pData)
        {
            // 頂点情報を受け取り
            _streamOutData.clear();
            UINT size = mapped_resource.RowPitch / sizeof(StreamOutVertex);
            _streamOutData.resize(size);
            CopyMemory(_streamOutData.data(), mapped_resource.pData, sizeof(StreamOutVertex) * size);

            // マップ解除
            dc->Unmap(_streamOutCopyBuffer.Get(), 0);
        }
    }
}
// GUI描画
void Terrain::DrawGui(ID3D11Device* device, ID3D11DeviceContext* dc)
{
    if (ImGui::Button(u8"頂点再計算"))
        _streamOut = true;
    if (ImGui::TreeNode(u8"定数バッファ"))
    {
        ImGui::SliderFloat(u8"Edge Factor", &_data.edgeFactor, 1.0f, 128.0f, "%.1f");
        ImGui::SliderFloat(u8"Inner Factor", &_data.innerFactor, 1.0f, 128.0f, "%.1f");
        ImGui::SliderFloat(u8"Height Scaler", &_data.heightScaler, 0.1f, 10.0f, "%.1f");
        ImGui::SliderFloat(u8"Emissive", &_data.emissive, 0.0f, 1.0f, "%.2f");
        ImGui::SliderFloat(u8"Metalness", &_data.metalness, 0.0f, 1.0f, "%.2f");
        ImGui::SliderFloat(u8"Roughness", &_data.roughness, 0.0f, 1.0f, "%.2f");
        ImGui::TreePop();
    }
    if (ImGui::TreeNode(u8"テクスチャ"))
    {
		ImGui::Text(u8"基本色テクスチャ: %s", ToString(_baseColorTexturePath).c_str());
		ImGui::Image(_materialMapFB->GetColorSRV(BaseColorTextureIndex).Get(), ImVec2(256, 256), ImVec2(0, 0), ImVec2(1, 1));
		ImGui::Text(u8"法線テクスチャ: %s", ToString(_normalTexturePath).c_str());
		ImGui::Image(_materialMapFB->GetColorSRV(NormalTextureIndex).Get(), ImVec2(256, 256), ImVec2(0, 0), ImVec2(1, 1));
        ImGui::Text(u8"パラメータマップ: %s", ToString(_parameterTexturePath).c_str());
        ImGui::Image(_parameterMapFB->GetColorSRV().Get(), ImVec2(256, 256), ImVec2(0, 0), ImVec2(1, 1));
        if (ImGui::Button(u8"リセット"))
            _resetMap = true;
        ImGui::TreePop();
    }
    if (ImGui::TreeNode(u8"透明壁"))
    {
        size_t wallIndex = 0;
        for (auto& wall : _transparentWalls)
        {
            if (ImGui::TreeNode(("Wall:" + std::to_string(wallIndex)).c_str()))
            {
                ImGui::Text(u8"透明壁の頂点数: %zu", wall.points.size());
                for (size_t i = 0; i < wall.points.size(); i++)
                {
                    ImGui::DragFloat3(std::to_string(i).c_str(), &wall.points[i].x, 0.1f);
                    ImGui::SameLine();
                    if (ImGui::Button(u8"削除"))
                    {
                        // 透明壁の頂点を削除
						wall.points.erase(wall.points.begin() + i);
						// 削除後はループを抜ける
						break;
                    }
                }
                ImGui::DragFloat(u8"透明壁の高さ", &wall.height, 0.1f, 0.0f, 100.0f);
                if (ImGui::Button(u8"透明壁の頂点追加"))
                {
                    // 透明壁の頂点を追加
                    wall.points.push_back(Vector3{});
                }

				ImGui::TreePop();
            }
            wallIndex++;
        }
        ImGui::Separator();
		if (ImGui::Button(u8"透明壁の追加"))
		{
			// 透明壁を追加
            _transparentWalls.push_back(TransparentWall{});
		}

        ImGui::TreePop();
    }
    if (ImGui::TreeNode(u8"環境オブジェクト"))
    {
        _terrainObjectLayout.DrawGui();
        ImGui::TreePop();
    }
    if (ImGui::TreeNode(u8"入出力"))
    {
		std::string resultPath = "";

		if (ImGui::OpenDialogBotton(u8"基本色テクスチャ読み込み", &resultPath, ImGui::DDSTextureFilter))
		{
            GpuResourceManager::LoadTextureFromFile(
                device,
                ToWString(resultPath).c_str(),
                _loadBaseColorSRV.ReleaseAndGetAddressOf(),
                nullptr);
			// 基本色テクスチャのパスを更新
			_baseColorTexturePath = ToWString(resultPath);
			// テクスチャをロード
            GpuResourceManager::LoadTextureFromFile(device,
                _baseColorTexturePath.c_str(),
                _loadBaseColorSRV.ReleaseAndGetAddressOf(),
                nullptr);
            // フラグをオンにする
			_isLoadingTextures = true;
		}
        if (ImGui::OpenDialogBotton(u8"法線テクスチャ読み込み", &resultPath, ImGui::DDSTextureFilter))
        {
			GpuResourceManager::LoadTextureFromFile(
                device,
				ToWString(resultPath).c_str(),
				_loadNormalSRV.ReleaseAndGetAddressOf(),
				nullptr);
			// 法線テクスチャのパスを更新
			_normalTexturePath = ToWString(resultPath);
			// テクスチャをロード
			GpuResourceManager::LoadTextureFromFile(device,
				_normalTexturePath.c_str(),
				_loadNormalSRV.ReleaseAndGetAddressOf(),
				nullptr);
			// フラグをオンにする
			_isLoadingTextures = true;
        }
		if (ImGui::OpenDialogBotton(u8"パラメータマップ読み込み", &resultPath, ImGui::DDSTextureFilter))
		{
			GpuResourceManager::LoadTextureFromFile(
                device,
				ToWString(resultPath).c_str(),
				_loadParameterSRV.ReleaseAndGetAddressOf(),
				nullptr);
			// パラメータマップのパスを更新
			_parameterTexturePath = ToWString(resultPath);
			// テクスチャをロード
			GpuResourceManager::LoadTextureFromFile(device,
				_parameterTexturePath.c_str(),
				_loadParameterSRV.ReleaseAndGetAddressOf(),
				nullptr);
			// フラグをオンにする
			_isLoadingTextures = true;
		}
		ImGui::Separator();

		if (ImGui::SaveDialogBotton(u8"基本色テクスチャ書き出し", &resultPath, ImGui::DDSTextureFilter))
		{
			SaveBaseColorTexture(device, dc,
				ToWString(resultPath).c_str());
		}
		if (ImGui::SaveDialogBotton(u8"法線テクスチャ書き出し", &resultPath, ImGui::DDSTextureFilter))
		{
			SaveNormalTexture(device, dc,
				ToWString(resultPath).c_str());
		}
		if (ImGui::SaveDialogBotton(u8"パラメータマップ書き出し", &resultPath, ImGui::DDSTextureFilter))
		{
			SaveParameterMap(device, dc,
				ToWString(resultPath).c_str());
		}
		ImGui::Separator();

		if (ImGui::SaveDialogBotton(u8"保存", &resultPath, ImGui::JsonFilter))
		{
			// 地形の情報をJSONファイルに保存
			SaveToFile(resultPath);
		}
		if (ImGui::OpenDialogBotton(u8"読み込み", &resultPath, ImGui::JsonFilter))
		{
			// 地形の情報をJSONファイルから読み込み
			LoadFromFile(resultPath);
		}
		ImGui::Separator();

        ImGui::TreePop();
    }
    ImGui::Separator();
}
// レイキャスト
bool Terrain::Raycast(
    const DirectX::XMFLOAT4X4& world,
    const Vector3& rayStart, 
    const Vector3& rayDirection,
    float rayLength,
    Vector3* intersectionWorldPoint,
    Vector3* intersectionWorldNormal,
    Vector2* intersectUVPosition) const
{
    auto& streamOutData = GetStreamOutData();
    if (streamOutData.empty())
        return false;

	DirectX::XMMATRIX InvWorld = DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&world));

	bool isHit = false;
	Vector3 hitWPoint{};
	Vector3 hitWNormal{};
	float length = rayLength;

    DirectX::XMVECTOR RayStart = DirectX::XMLoadFloat3(&rayStart);
    DirectX::XMVECTOR RayDir = DirectX::XMLoadFloat3(&rayDirection);
    // 地形との交差判定
    for (size_t i = 0; i < streamOutData.size(); i += 3)
    {
        const auto& v1 = streamOutData[i + 0];
        const auto& v2 = streamOutData[i + 1];
        const auto& v3 = streamOutData[i + 2];
        DirectX::XMVECTOR triangleVerts[3] = {
            DirectX::XMLoadFloat3(&v1.worldPosition),
            DirectX::XMLoadFloat3(&v2.worldPosition),
            DirectX::XMLoadFloat3(&v3.worldPosition)
        };
        HitResult hitResult;
        if (Collision3D::IntersectRayVsTriangle(
            RayStart, RayDir, length,
            triangleVerts,
            hitResult))
        {
            // 交差点を記録
            hitWPoint = hitResult.position;
			hitWNormal = hitResult.normal;
            isHit = true;
			// 交差点の長さを更新
			length = hitResult.distance;
        }
    }

    if (isHit)
    {
		if (intersectionWorldPoint)
			*intersectionWorldPoint = hitWPoint;
		if (intersectionWorldNormal)
			*intersectionWorldNormal = hitWNormal;
        if (intersectUVPosition)
        {
            Vector3 uv = hitWPoint.TransformCoord(InvWorld);
			intersectUVPosition->x = (uv.x + 1.0f) / 2.0f;
			intersectUVPosition->y = (-uv.z + 1.0f) / 2.0f;
        }
    }
	return isHit;
}
// 基本色テクスチャのの書き出し
void Terrain::SaveBaseColorTexture(ID3D11Device* device, ID3D11DeviceContext* dc, const wchar_t* baseColorPath)
{
    if (Exporter::SaveDDSFile(device, dc,
        _materialMapFB->GetColorSRV(BaseColorTextureIndex).Get(),
        baseColorPath))
    {
        // 基本色テクスチャのパスを更新
        _baseColorTexturePath = baseColorPath;
    }
}
// 法線テクスチャの書き出し
void Terrain::SaveNormalTexture(ID3D11Device* device, ID3D11DeviceContext* dc, const wchar_t* normalPath)
{
    if (Exporter::SaveDDSFile(device, dc,
        _materialMapFB->GetColorSRV(NormalTextureIndex).Get(),
        normalPath))
    {
        // 基本色テクスチャのパスを更新
        _normalTexturePath = normalPath;
    }
}
// パラメータマップの書き出し
void Terrain::SaveParameterMap(ID3D11Device* device, ID3D11DeviceContext* dc, const wchar_t* parameterMapPath)
{
    if (Exporter::SaveDDSFile(device, dc,
        _parameterMapFB->GetColorSRV().Get(),
        parameterMapPath))
    {
		// パラメータマップのパスを更新
		_parameterTexturePath = parameterMapPath;
    }
}
// 書き出し
void Terrain::SaveToFile(const std::string& path)
{
    nlohmann::json jsonData;
    {
        jsonData["baseColorTexturePath"] = _baseColorTexturePath;
        jsonData["normalTexturePath"] = _normalTexturePath;
        jsonData["parameterTexturePath"] = _parameterTexturePath;
    }
    jsonData["transparentWallsSize"] = _transparentWalls.size();
    for (size_t i = 0; i < _transparentWalls.size(); ++i)
    {
        _transparentWalls[i].Export(("transparentWall" + std::to_string(i)).c_str(), &jsonData);
    }
    Exporter::SaveJsonFile(path, jsonData);

    // シリアライズパスを更新
    _serializePath = path;
}
// 読み込み
void Terrain::LoadFromFile(const std::string& path)
{
    nlohmann::json jsonData;
    if (!Exporter::LoadJsonFile(path, &jsonData))
        return;

	// JSONからテクスチャパスを読み込み
	if (jsonData.contains("baseColorTexturePath"))
		_baseColorTexturePath = jsonData["baseColorTexturePath"].get<std::wstring>();
	if (jsonData.contains("normalTexturePath"))
		_normalTexturePath = jsonData["normalTexturePath"].get<std::wstring>();
	if (jsonData.contains("parameterTexturePath"))
		_parameterTexturePath = jsonData["parameterTexturePath"].get<std::wstring>();
    // 透明壁の読み込み
    if (jsonData.contains("transparentWallsSize"))
    {
        size_t wallSize = jsonData["transparentWallsSize"].get<size_t>();
        _transparentWalls.resize(wallSize);
        for (size_t i = 0; i < wallSize; ++i)
        {
            _transparentWalls[i].Inport(("transparentWall" + std::to_string(i)).c_str(), jsonData);
        }
    }

    // シリアライズパスを更新
	_serializePath = path;
}
// 地形メッシュの頂点とインデックスを生成
void Terrain::CreateTerrainMesh(ID3D11Device* device)
{
    // 地形メッシュの頂点とインデックスを生成
    std::vector<Vertex> vertices =
    {
        {{-1.0f,  0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},// 左下
        {{-1.0f,  0.0f,  1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},// 左上
        {{ 1.0f,  0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},// 右下
        {{ 1.0f,  0.0f,  1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},// 右上
    };
    std::vector<uint32_t> indices = {
        0, 1, 2, // 左下, 左上, 右下
        1, 3, 2  // 左上, 右上, 右下
    };

    // 頂点バッファとインデックスバッファを作成
    HRESULT hr{ S_OK };
    D3D11_BUFFER_DESC buffer_desc{};
    D3D11_SUBRESOURCE_DATA subresource_data{};
    buffer_desc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * vertices.size());
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    buffer_desc.CPUAccessFlags = 0;
    buffer_desc.MiscFlags = 0;
    buffer_desc.StructureByteStride = 0;
    subresource_data.pSysMem = vertices.data();
    subresource_data.SysMemPitch = 0;
    subresource_data.SysMemSlicePitch = 0;
    hr = device->CreateBuffer(&buffer_desc, &subresource_data,
        _vertexBuffer.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    buffer_desc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * indices.size());
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    subresource_data.pSysMem = indices.data();
    hr = device->CreateBuffer(&buffer_desc, &subresource_data,
        _indexBuffer.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
}
// 書き出し
void Terrain::TransparentWall::Export(const char* label, nlohmann::json* jsonData)
{
    (*jsonData)[label]["pointsSize"] = points.size();
    for (size_t i = 0; i < points.size(); ++i)
    {
        // Vector3をJSONに変換
        nlohmann::json pointJson = {
            {"x", points[i].x},
            {"y", points[i].y},
            {"z", points[i].z}
        };
        // JSONに追加
        (*jsonData)[label][std::to_string(i)] = pointJson;
    }
    (*jsonData)[label]["height"] = height;
}
// 読み込み
void Terrain::TransparentWall::Inport(const char* label, nlohmann::json& jsonData)
{
    if (jsonData.contains(label))
    {
        if (jsonData[label].contains("pointsSize"))
        {
            size_t pointSize = jsonData[label]["pointsSize"].get<size_t>();
            points.resize(pointSize);
            for (size_t i = 0; i < pointSize; ++i)
            {
                if (jsonData[label].contains(std::to_string(i)))
                {
                    auto& pointData = jsonData[label][std::to_string(i)];
                    points[i].x = pointData.value("x", points[i].x);
                    points[i].y = pointData.value("y", points[i].y);
                    points[i].z = pointData.value("z", points[i].z);
                }
            }
        }
        height = jsonData[label].value("height", height);
    }
}
