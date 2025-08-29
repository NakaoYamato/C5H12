#include "Terrain.h"

#include "../HRTrace.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Graphics/GpuResourceManager.h"
#include "../../Library/Collision/CollisionMath.h"
#include "../../Library/Algorithm/Converter.h"
#include "../../Library/Renderer/TerrainRenderer.h"
#include "../../Library/Exporter/Exporter.h"

#include <Mygui.h>

Terrain::Terrain(ID3D11Device* device, const std::string& serializePath) :
	_serializePath(serializePath)
{
    // 地形メッシュの頂点とインデックスを生成
    CreateTerrainMesh(device);

    // マテリアルマップ作成
	_materialMapFB = std::make_unique<FrameBuffer>(
		device,
        MaterialMapSize, MaterialMapSize, true,
		std::vector<DXGI_FORMAT>(
			{ 
                DXGI_FORMAT_R8G8B8A8_UNORM, // BaseColor 
                DXGI_FORMAT_R8G8B8A8_UNORM, // Normal
            })
    );
    // パラメータマップ作成
    _parameterMapFB = std::make_unique<FrameBuffer>(
        device, 
        ParameterMapSize, ParameterMapSize, true);
	// 頂点情報をGPUに送るためのバッファ、SRV作成
	{
		D3D11_BUFFER_DESC desc{};
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.ByteWidth = sizeof(StreamOutVertex) * 
            (TerrainRenderer::DivisionCount * 2) * 
            (TerrainRenderer::DivisionCount * 2 - 1) *
            static_cast<UINT>(std::pow(TerrainRenderer::MaxTessellation, 2.0f));
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.StructureByteStride = sizeof(StreamOutVertex);
		desc.Usage = D3D11_USAGE_DEFAULT;
		HRESULT hr = device->CreateBuffer(&desc, nullptr, _streamOutVertexBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		hr = device->CreateShaderResourceView(_streamOutVertexBuffer.Get(),
            nullptr, _streamOutSRV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

    // データの読み込み
	LoadFromFile(device, serializePath);
}
// テクスチャ更新
bool Terrain::UpdateTextures(TextureRenderer& textureRenderer, ID3D11DeviceContext* dc)
{
    bool res = false;

    // マテリアルマップ、パラメータマップのリセット
    if (_resetMap)
    {
        _materialMapFB->Clear(BaseColorTextureIndex, dc, Vector4::White);
        _materialMapFB->Clear(NormalTextureIndex, dc, Vector4::Blue);
        _parameterMapFB->Clear(dc, Vector4::Black);
        _resetMap = false;
        res = true;
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
            res = true;
        }
        if (_loadNormalSRV)
        {
            _materialMapFB->ClearAndActivate(NormalTextureIndex, dc);
            textureRenderer.Blit(dc, _loadNormalSRV.GetAddressOf(), 0, 1);
            _materialMapFB->Deactivate(dc);
            _loadNormalSRV.Reset();
            res = true;
        }
        if (_loadParameterSRV)
        {
            _parameterMapFB->ClearAndActivate(dc);
            textureRenderer.Blit(dc, _loadParameterSRV.GetAddressOf(), 0, 1);
            _parameterMapFB->Deactivate(dc);
            _loadParameterSRV.Reset();
            res = true;
        }
        _isLoadingTextures = false;
    }

    return res;
}
// GUI描画
void Terrain::DrawGui(ID3D11Device* device, ID3D11DeviceContext* dc)
{
    if (ImGui::TreeNode(u8"テクスチャ"))
    {
        if (ImGui::TreeNode(u8"基本色"))
        {
            ImGui::Text(ToString(_baseColorTexturePath).c_str());
            ImGui::Image(_materialMapFB->GetColorSRV(BaseColorTextureIndex).Get(), ImVec2(256, 256), ImVec2(0, 0), ImVec2(1, 1));

            std::string resultPath = "";
            if (ImGui::OpenDialogBotton(u8"読み込み", &resultPath, ImGui::DDSTextureFilter))
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
            ImGui::SameLine();
            if (ImGui::SaveDialogBotton(u8"書き出し", &resultPath, ImGui::DDSTextureFilter))
            {
                SaveBaseColorTexture(device, dc,
                    ToWString(resultPath).c_str());
            }

            ImGui::TreePop();
        }
        if (ImGui::TreeNode(u8"法線"))
        {
            ImGui::Text(ToString(_normalTexturePath).c_str());
            ImGui::Image(_materialMapFB->GetColorSRV(NormalTextureIndex).Get(), ImVec2(256, 256), ImVec2(0, 0), ImVec2(1, 1));

            std::string resultPath = "";
            if (ImGui::OpenDialogBotton(u8"読み込み", &resultPath, ImGui::DDSTextureFilter))
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
			ImGui::SameLine();
			if (ImGui::SaveDialogBotton(u8"書き出し", &resultPath, ImGui::DDSTextureFilter))
			{
				SaveNormalTexture(device, dc,
					ToWString(resultPath).c_str());
			}

            ImGui::TreePop();
        }
        if (ImGui::TreeNode(u8"パラメータ"))
        {
            ImGui::Text(ToString(_parameterTexturePath).c_str());
            ImGui::Image(_parameterMapFB->GetColorSRV().Get(), ImVec2(256, 256), ImVec2(0, 0), ImVec2(1, 1));

            std::string resultPath = "";
            if (ImGui::OpenDialogBotton(u8"読み込み", &resultPath, ImGui::DDSTextureFilter))
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
            ImGui::SameLine();
            if (ImGui::SaveDialogBotton(u8"書き出し", &resultPath, ImGui::DDSTextureFilter))
            {
                SaveParameterMap(device, dc,
                    ToWString(resultPath).c_str());
            }

            ImGui::TreePop();
        }

        if (ImGui::Button(u8"リセット"))
            _resetMap = true;

        ImGui::TreePop();
    }
    if (ImGui::TreeNode(u8"透明壁"))
    {
        _transparentWall.DrawGui();

        ImGui::TreePop();
    }
    if (ImGui::TreeNode(u8"環境オブジェクト"))
    {
        _terrainObjectLayout.DrawGui();
        ImGui::TreePop();
    }
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
// ストリームアウトデータを設定
void Terrain::SetStreamOutData(ID3D11DeviceContext* dc, const std::vector<StreamOutVertex>& data)
{
    _streamOutData = data;
    UINT size = static_cast<UINT>(_streamOutData.size() * sizeof(StreamOutVertex));
    // バッファを更新
    D3D11_BOX writeBox = {};
    writeBox.left = 0;
    writeBox.right = size;
    writeBox.top = 0;
    writeBox.bottom = 1;
    writeBox.front = 0;
    writeBox.back = 1;
    dc->UpdateSubresource(
        _streamOutVertexBuffer.Get(),
        0,
        &writeBox,
        _streamOutData.data(),
        size,
        0);
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
    // 透明壁の書き出し
    _transparentWall.Export(&jsonData);
	// 地形データの書き出し
	_terrainObjectLayout.Export(&jsonData);

    Exporter::SaveJsonFile(path, jsonData);

    // シリアライズパスを更新
    _serializePath = path;
}
// 読み込み
void Terrain::LoadFromFile(ID3D11Device* device, const std::string& path)
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
    _transparentWall.Import(jsonData);
	// 地形データの読み込み
	_terrainObjectLayout.Import(device, jsonData);
    // シリアライズパスを更新
	_serializePath = path;
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
// 地形メッシュの頂点とインデックスを生成
void Terrain::CreateTerrainMesh(ID3D11Device* device)
{
	size_t divisionCount = TerrainRenderer::DivisionCount;
	size_t vertexCountPerSide = TerrainRenderer::VertexCountPerSide;
    std::vector<TerrainRenderer::Vertex> vertices{};
    std::vector<uint32_t> indices{};
    {
		// 頂点の生成
        vertices.reserve(vertexCountPerSide * vertexCountPerSide);
        for (size_t x = 0; x < vertexCountPerSide; ++x)
        {
            for (size_t z = 0; z < vertexCountPerSide; ++z) 
            {
                float posX = -1.0f + static_cast<float>(x) / divisionCount * 2.0f;
                float posZ = -1.0f + static_cast<float>(z) / divisionCount * 2.0f;

                float u = static_cast<float>(x) / divisionCount;
                float v = 1.0f - static_cast<float>(z) / divisionCount;

                vertices.push_back({
                    {posX, 0.0f, posZ},     // Position
                    {0.0f, 1.0f, 0.0f},     // Normal (Y-up)
                    {u, v}                  // Texture Coordinate
                    });
            }
        }

		// インデックスの生成
        indices.reserve(divisionCount * divisionCount * 4); // 1クアッドあたり4インデックス
        for (size_t x = 0; x < divisionCount; ++x)
        {
            for (size_t z = 0; z < divisionCount; ++z)
            {
                // 現在のクアッド（パッチ）を構成する4つの頂点のインデックスを計算
                // (zが下方向に増加するグリッドと仮定)
                // v0 -- v2
                // |    |
                // v1 -- v3
                size_t v0 = x * vertexCountPerSide + z;          // 左上 (Top-Left)
                size_t v1 = x * vertexCountPerSide + (z + 1);      // 左下 (Bottom-Left)
                size_t v2 = (x + 1) * vertexCountPerSide + z;      // 右上 (Top-Right)
                size_t v3 = (x + 1) * vertexCountPerSide + (z + 1);  // 右下 (Bottom-Right)

                // 4つの頂点インデックスをパッチとして追加します。
                // ハルシェーダーでの辺の扱いやカリングを意識して、一貫した順序で追加します。
                // ここでは反時計回り (v0 -> v1 -> v3 -> v2) で追加しています。
                indices.push_back(static_cast<uint32_t>(v0));
                indices.push_back(static_cast<uint32_t>(v1));
                indices.push_back(static_cast<uint32_t>(v3));
                indices.push_back(static_cast<uint32_t>(v2));
            }
        }
    }

    // 頂点バッファとインデックスバッファを作成
    HRESULT hr{ S_OK };
    D3D11_BUFFER_DESC buffer_desc{};
    D3D11_SUBRESOURCE_DATA subresource_data{};
    buffer_desc.ByteWidth = static_cast<UINT>(sizeof(TerrainRenderer::Vertex) * vertices.size());
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
