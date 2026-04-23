#include "TerrainRenderer.h"

#include "../HRTrace.h"
#include "../../Library/Graphics/GpuResourceManager.h"
#include "../../Library/JobSystem/JobSystem.h"
#include "../../Library/Shader/Model/CascadedShadowMap/CascadedShadowMapShader.h"

#include <Mygui.h>

const size_t TerrainRenderer::DivisionCount = 17;
const float TerrainRenderer::TerrainLength = 100.0f;

// 初期化
void TerrainRenderer::Initialize(ID3D11Device* device)
{
	const UINT MaxTess = static_cast<UINT>(MaxTessellation);
	const UINT StreamOutMaxVertex =	
        (DivisionCount * 2) * 
        (DivisionCount * 2 - 1) * 
        ((MaxTess - 1) * (MaxTess - 2)) / 2;

    _currentTextureQuality = Terrain::TextureQuality::Low;

    // 定数バッファデータ初期化
    _data.lodTessFactors = Vector4(31.0f, 19.0f, 9.0f, 7.0f);
    _data.lodTessDistance = 30.0f;
    _data.collisionTessFactor = 7.0f;
    // 地形1枚の長さ(m)
	_data.terrainLength = TerrainLength;
    // 分割数
    _data.divisionCount = static_cast<int>(DivisionCount);

    // 定数バッファ作成
    GpuResourceManager::CreateConstantBuffer(
        device,
        sizeof(ConstantBuffer),
        _constantBuffer.ReleaseAndGetAddressOf());
	// 草の定数バッファ作成
	GpuResourceManager::CreateConstantBuffer(
		device,
		sizeof(GrassConstantBuffer),
		_grassConstantBuffer.ReleaseAndGetAddressOf());

	// 通常描画用シェーダー作成
    {
        D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        // 頂点シェーダー
		_vertexShader.Load(device,
			"./Data/Shader/HLSL/Terrain/TerrainVS.cso",
			inputElementDesc,
			static_cast<UINT>(_countof(inputElementDesc)));
        // ハルシェーダー
		_hullShader.Load(device, "./Data/Shader/HLSL/Terrain/TerrainHS.cso");
        // ドメインシェーダー
		_domainShader.Load(device, "./Data/Shader/HLSL/Terrain/TerrainDS.cso");
        // ピクセルシェーダー
		_pixelShader.Load(device, "./Data/Shader/HLSL/Terrain/TerrainPS.cso");
        // GBufferへ書き込む用のピクセルシェーダー
		_gbPixelShader.Load(device, "./Data/Shader/HLSL/Terrain/TerrainGBPS.cso");

        // デバッグ表示用ピクセルシェーダー
		_debugPixelShader.Load(device, "./Data/Shader/HLSL/Terrain/TerrainDebugGBPS.cso");
    }

    // 草描画用シェーダー作成
	{
        D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
        {
            { "SV_POSITION",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "WORLD_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL",         0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TANGENT",        0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "PARAMETER",      0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD",       0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        // 頂点シェーダー作成
		_grassVertexShader.Load(device,
			"./Data/Shader/HLSL/Terrain/Grass/TerrainGrassVS.cso",
			inputElementDesc,
            static_cast<UINT>(_countof(inputElementDesc)));
        // ハルシェーダー
		_grassHullShader.Load(device, "./Data/Shader/HLSL/Terrain/Grass/TerrainGrassHS.cso");
        // ドメインシェーダー
        _grassDomainShader.Load(device, "./Data/Shader/HLSL/Terrain/Grass/TerrainGrassDS.cso");
		// ジオメトリシェーダー作成
		_grassGeometryShader.Load(device, "./Data/Shader/HLSL/Terrain/Grass/TerrainGrassGS.cso");
		// ピクセルシェーダー作成
		_grassPixelShader.Load(device, "./Data/Shader/HLSL/Terrain/Grass/TerrainGrassPS.cso");
        // GBufferへ書き込む用のピクセルシェーダー
		_grassGBPixelShader.Load(device, "./Data/Shader/HLSL/Terrain/Grass/TerrainGrassGBPS.cso");
	}

    // ストリームアウトを使用してプリミティブを取得するシェーダー
    {
        D3D11_SO_DECLARATION_ENTRY declaration[] =
        {
            {0,"SV_POSITION",	0,0,4,0},
            {0,"WORLD_POSITION",0,0,3,0},
            {0,"NORMAL",		0,0,3,0},
            {0,"TANGENT",		0,0,4,0},
            {0,"PARAMETER",     0,0,4,0},
            {0,"TEXCOORD",		0,0,2,0},
        };
        UINT bufferStrides[] = { sizeof(Terrain::StreamOutVertex) };
		_streamOutGeometryShader.LoadForStreamOut(
			device,
			"./Data/Shader/HLSL/Terrain/TerrainGS.cso",
			declaration, _countof(declaration),
			bufferStrides, _countof(bufferStrides),
			0);

		// ハルシェーダー
		_streamOutHullShader.Load(device, "./Data/Shader/HLSL/Terrain/TerrainStreamOutHS.cso");
        // ドメインシェーダー
        _streamOutDomainShader.Load(device, "./Data/Shader/HLSL/Terrain/TerrainStreamOutDS.cso");
    }

    // コリジョン用ストリームアウトシェーダー
    {
        D3D11_SO_DECLARATION_ENTRY declaration[] =
        {
            {0,"WORLD_POSITION",0,0,3,0},
            {0,"PADDING",		0,0,1,0},
        };
        UINT bufferStrides[] = { sizeof(Terrain::CollisionStreamOutVertex) };
        _streamOutCollisionGeometryShader.LoadForStreamOut(
            device,
            "./Data/Shader/HLSL/Terrain/TerrainCollisionGS.cso",
            declaration, _countof(declaration),
            bufferStrides, _countof(bufferStrides),
            0);
    }

    // ストリームアウトプットされた情報の受け取り用バッファ（コリジョン用）
    {
        D3D11_BUFFER_DESC desc{};
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.ByteWidth = sizeof(Terrain::CollisionStreamOutVertex) * StreamOutMaxVertex;
        desc.BindFlags = D3D11_BIND_STREAM_OUTPUT;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        HRESULT hr = device->CreateBuffer(&desc, nullptr,
            _streamOutCollisionVertexBuffer.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
    }

	// CPUからアクセスするためのバッファ（コリジョン用）
    {
        D3D11_BUFFER_DESC desc{};
        desc.Usage = D3D11_USAGE_STAGING;
        desc.ByteWidth = sizeof(Terrain::CollisionStreamOutVertex) * StreamOutMaxVertex;
        desc.BindFlags = 0;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        HRESULT hr = device->CreateBuffer(&desc, nullptr,
            _streamOutCollisionCopyBuffer.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
    }

    // 影描画用シェーダ読み込み
    {
        D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
        {
            { "POSITION",       0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        // 頂点シェーダ
        _shadowVertexShader.Load(device, "./Data/Shader/HLSL/Terrain/Shadow/TerrainShadowVS.cso",
			inputElementDesc, static_cast<UINT>(_countof(inputElementDesc)));
        // ハルシェーダー
        _shadowHullShader.Load(device, "./Data/Shader/HLSL/Terrain/Shadow/TerrainShadowHS.cso");
        // ドメインシェーダー
        _shadowDomainShader.Load(device, "./Data/Shader/HLSL/Terrain/Shadow/TerrainShadowDS.cso");
        // ジオメトリシェーダー
        _shadowGeometryShader.Load(device, "./Data/Shader/HLSL/Model/CascadedShadow/CascadedShadowGS.cso");
    }

    // 風のゆがみテクスチャ読み込み
    GpuResourceManager::LoadTextureFromFile(device,
        L"./Data/Texture/Wind/distortion texture.png",
        _windDistortionSRV.ReleaseAndGetAddressOf(),
        nullptr);
	// 草のテクスチャ読み込み
	GpuResourceManager::LoadTextureFromFile(device,
		L"./Data/Terrain/Environment/Grass/Texture/ground_02.png",
		_grassColorSRV.ReleaseAndGetAddressOf(),
		nullptr);
}
// コリジョン用頂点情報書き出し登録
void TerrainRenderer::RegisterCollisionVertices(Terrain* terrain, const DirectX::XMFLOAT4X4& world)
{
    std::lock_guard<std::mutex> lock(_drawInfoMutex);

    DrawInfo drawInfo;
    drawInfo.terrain = terrain;
    drawInfo.world = world;

    _collisionExportVertexDrawInfos.push_back(drawInfo);
}
// 草用頂点情報書き出し登録
void TerrainRenderer::RegisterGrassVertices(Terrain* terrain, const DirectX::XMFLOAT4X4& world)
{
    std::lock_guard<std::mutex> lock(_drawInfoMutex);

    DrawInfo drawInfo;
    drawInfo.terrain = terrain;
    drawInfo.world = world;

    _grassExportVertexDrawInfos.push_back(drawInfo);
}
// 描画登録
void TerrainRenderer::Draw(Terrain* terrain, const DirectX::XMFLOAT4X4& world)
{
    std::lock_guard<std::mutex> lock(_drawInfoMutex);

	DrawInfo drawInfo;
	drawInfo.terrain = terrain;
	drawInfo.world = world;

    _drawInfos.push_back(drawInfo);

	// 草の描画登録
	_grassDrawInfos.push_back(drawInfo);
}
// 影描画登録
void TerrainRenderer::DrawShadow(Terrain* terrain, const DirectX::XMFLOAT4X4& world)
{
    std::lock_guard<std::mutex> lock(_drawInfoMutex);

    // 頂点情報を書き出していない場合は登録しない
    if (terrain->GetCollisionStreamOutData().size() == 0)
        return;

    DrawInfo drawInfo;
    drawInfo.terrain = terrain;
    drawInfo.world = world;
    _shadowDrawInfos.push_back(drawInfo);
}
// 頂点書き出し処理
void TerrainRenderer::ExportVertex(const RenderContext& rc)
{
    ProfileScopedSection_3(0, "TerrainRenderer::ExportVertex", ImGuiControl::Profiler::Green);

    ID3D11DeviceContext* dc = rc.deviceContext;

    // 定数バッファ設定
    dc->VSSetConstantBuffers(ModelCBIndex, 1, _constantBuffer.GetAddressOf());
    dc->HSSetConstantBuffers(ModelCBIndex, 1, _constantBuffer.GetAddressOf());
    dc->DSSetConstantBuffers(ModelCBIndex, 1, _constantBuffer.GetAddressOf());
    dc->GSSetConstantBuffers(ModelCBIndex, 1, _constantBuffer.GetAddressOf());
    dc->PSSetConstantBuffers(ModelCBIndex, 1, _constantBuffer.GetAddressOf());

    // 頂点情報を書き出す
    dc->OMSetDepthStencilState(rc.renderState->GetDepthStencilState(DepthState::NoTestNoWrite), 0);
    RenderCollisionStreamOut(rc);
    RenderGrassStreamOut(rc);
    dc->OMSetDepthStencilState(rc.renderState->GetDepthStencilState(DepthState::TestAndWrite), 0);
}
// 描画処理
void TerrainRenderer::Render(const RenderContext& rc, bool writeGBuffer)
{
    ProfileScopedSection_3(0, "TerrainRenderer::Render", ImGuiControl::Profiler::Green);

    ID3D11DeviceContext* dc = rc.deviceContext;

    if (_isWireFrame)
        dc->RSSetState(rc.renderState->GetRasterizerState(RasterizerState::WireCullNone));
    else
        dc->RSSetState(rc.renderState->GetRasterizerState(RasterizerState::SolidCullFront));
	// MRTのブレンドステートを設定
    if (writeGBuffer)
        dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::MultipleRenderTargets), nullptr, 0xFFFFFFFF);
    else
        dc->OMSetBlendState(rc.renderState->GetBlendState(BlendState::Opaque), nullptr, 0xFFFFFFFF);
    // 定数バッファ設定
    dc->VSSetConstantBuffers(ModelCBIndex, 1, _constantBuffer.GetAddressOf());
    dc->HSSetConstantBuffers(ModelCBIndex, 1, _constantBuffer.GetAddressOf());
    dc->DSSetConstantBuffers(ModelCBIndex, 1, _constantBuffer.GetAddressOf());
    dc->GSSetConstantBuffers(ModelCBIndex, 1, _constantBuffer.GetAddressOf());
    dc->PSSetConstantBuffers(ModelCBIndex, 1, _constantBuffer.GetAddressOf());

    RenderDynamic(rc, writeGBuffer);

    // シェーダーリソースビューを解除
    ID3D11ShaderResourceView* nullSRVs[3] = {};
    dc->PSSetShaderResources(0, _countof(nullSRVs), nullSRVs);
    dc->VSSetShaderResources(0, _countof(nullSRVs), nullSRVs);
    dc->DSSetShaderResources(0, _countof(nullSRVs), nullSRVs);

    RenderGrass(rc, writeGBuffer);

    // 登録しているTerrainを描画した後はクリア
    _drawInfos.clear();
    _grassDrawInfos.clear();
}
// 影描画実行
void TerrainRenderer::CastShadow(const RenderContext& rc)
{
    ID3D11DeviceContext* dc = rc.deviceContext;
    
    dc->RSSetState(rc.renderState->GetRasterizerState(RasterizerState::SolidCullFront));

    dc->HSSetConstantBuffers(ModelCBIndex, 1, _constantBuffer.GetAddressOf());
    dc->DSSetConstantBuffers(ModelCBIndex, 1, _constantBuffer.GetAddressOf());

    dc->IASetInputLayout(_shadowVertexShader.GetInputLayout());
    dc->VSSetShader(_shadowVertexShader.Get(), nullptr, 0);
    dc->HSSetShader(_shadowHullShader.Get(), nullptr, 0);
    dc->DSSetShader(_shadowDomainShader.Get(), nullptr, 0);
    dc->GSSetShader(_shadowGeometryShader.Get(), nullptr, 0);
    dc->PSSetShader(nullptr, nullptr, 0);

    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

    for (const auto& drawInfo : _shadowDrawInfos)
    {
        // 定数バッファの更新
        _data.world = drawInfo.world;
        dc->UpdateSubresource(_constantBuffer.Get(), 0, nullptr, &_data, 0, 0);

        // シェーダーリソースビューの設定
        ID3D11ShaderResourceView* srvs[] =
        {
            drawInfo.terrain->GetMaterialMapFB(_currentTextureQuality)->GetColorSRV(Terrain::BaseColorTextureIndex).Get(),
            drawInfo.terrain->GetMaterialMapFB(_currentTextureQuality)->GetColorSRV(Terrain::NormalTextureIndex).Get(),
            drawInfo.terrain->GetMaterialMapFB(_currentTextureQuality)->GetColorSRV(Terrain::ParameterTextureIndex).Get()
        };
        dc->DSSetShaderResources(0, _countof(srvs), srvs);
        // 頂点バッファとインデックスバッファを設定
        UINT stride = sizeof(TerrainRenderer::Vertex);
        UINT offset = 0;
        dc->IASetVertexBuffers(0, 1, drawInfo.terrain->GetVertexBuffer().GetAddressOf(), &stride, &offset);
        dc->IASetIndexBuffer(drawInfo.terrain->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);

        // 描画
        D3D11_BUFFER_DESC bufferDesc{};
        drawInfo.terrain->GetIndexBuffer()->GetDesc(&bufferDesc);
        dc->DrawIndexedInstanced(bufferDesc.ByteWidth / sizeof(uint32_t), _CASCADED_SHADOW_MAPS_SIZE, 0, 0, 0);
    }

    // シェーダー解除
    dc->VSSetShader(nullptr, nullptr, 0);
    dc->HSSetShader(nullptr, nullptr, 0);
    dc->DSSetShader(nullptr, nullptr, 0);
    dc->GSSetShader(nullptr, nullptr, 0);
    dc->PSSetShader(nullptr, nullptr, 0);

    // 登録しているTerrainを描画した後はクリア
    _shadowDrawInfos.clear();

    dc->RSSetState(rc.renderState->GetRasterizerState(RasterizerState::SolidCullNone));
}
// GUI描画
void TerrainRenderer::DrawGui()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu(u8"デバッグ"))
        {
			ImGui::Checkbox(u8"Terrain", &_isDrawingGui);
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
    if (_isDrawingGui)
    {
        if (ImGui::Begin(u8"Terrain"))
        {
			ImGui::Checkbox(u8"草を描画", &_isDrawingGrass);
			ImGui::Checkbox(u8"ワイヤーフレーム", &_isWireFrame);
			ImGui::Checkbox(u8"Mipmapを使用", &_isUsingMipmap);
            ImGui::Checkbox(u8"デバッグ表示", &_isDebugDraw);
			ImGui::Separator();
			int textureQuality = static_cast<int>(_currentTextureQuality);
			if (ImGui::Combo(u8"テクスチャ品質", &textureQuality, u8"低\0中\0高\0"))
			{
				_currentTextureQuality = static_cast<Terrain::TextureQuality>(textureQuality);
			}
			ImGui::Separator();

            if (ImGui::TreeNode(u8"定数バッファ"))
            {
                ImGui::SliderFloat4(u8"分割数", &_data.lodTessFactors.x, 1.0f, MaxTessellation, "%.1f");
                ImGui::SliderFloat(u8"LOD距離", &_data.lodTessDistance, 1.0f, 200.0f, "%.1f");
                ImGui::SliderFloat(u8"衝突判定用エッジ分割数", &_data.collisionTessFactor, 1.0f, MaxTessellation, "%.1f");

				ImGui::DragFloat(u8"影の分割係数", &_data.shadowTessFactor, 0.1f, 0.0f, 10.0f);
				ImGui::DragFloat(u8"影のオフセット", &_data.shadowOffset, 0.001f, -1.0f, 1.0f);
				ImGui::DragFloat(u8"基本色の高さ影響度", &_data.texHeightInfluence, 0.001f, 0.0f, 1.0f);

                ImGui::SliderFloat(u8"エミッシブ", &_data.emissive, 0.0f, 1.0f, "%.2f");
                ImGui::SliderFloat(u8"メタリック", &_data.metalness, 0.0f, 1.0f, "%.2f");
                ImGui::SliderFloat(u8"ラフネス", &_data.roughness, 0.0f, 1.0f, "%.2f");

                ImGui::TreePop();

                auto Oddification = [](float& value)
                {
                    value += static_cast<int>(value) % 2 == 0 ? 1.0f : 0.0f;
                    value = std::floor(value);
                };
                // 分割数を奇数にする
                Oddification(_data.lodTessFactors.x);
                Oddification(_data.lodTessFactors.y);
                Oddification(_data.lodTessFactors.z);
                Oddification(_data.lodTessFactors.w);
                Oddification(_data.collisionTessFactor);
            }
            if (ImGui::TreeNode(u8"草の定数バッファ"))
            {
                ImGui::SliderFloat4(u8"分割数", &_dataGrass.lodTessFactors.x, 1.0f, MaxTessellation, "%.1f");
                ImGui::SliderFloat(u8"LOD距離", &_dataGrass.lodTessDistance, 1.0f, 200.0f, "%.1f");
                ImGui::DragFloat(u8"ノイズ", &_dataGrass.parlinNoiseDistribution, 0.01f, 0.01f, 10.0f, "%.2f");

                ImGui::DragFloat(u8"高さ", &_dataGrass.height, 0.01f, 0.01f, 10.0f, "%.2f");
                ImGui::DragFloat(u8"高さのズレ", &_dataGrass.heightVariance, 0.01f, 0.01f, 10.0f, "%.2f");
                ImGui::DragFloat(u8"幅", &_dataGrass.width, 0.001f, 0.001f, 10.0f, "%.2f");
                ImGui::DragFloat(u8"幅のズレ", &_dataGrass.widthVariance, 0.001f, 0.001f, 10.0f, "%.2f");

                ImGui::DragFloat(u8"曲率", &_dataGrass.curvature, 0.01f, 0.01f, 10.0f, "%.2f");
                ImGui::DragFloat(u8"曲率のズレ", &_dataGrass.curvatureVariance, 0.01f, 0.01f, 10.0f, "%.2f");
				ImGui::DragFloat(u8"枯れ具合", &_dataGrass.witherdFactor, 0.01f, 0.01f, 1.0f, "%.2f");
                ImGui::SliderFloat(u8"障害物の影響度", &_dataGrass.obstacleInfluence, 0.0f, 1.0f, "%.2f");

                ImGui::TreePop();
            }
        }
        // GUIが埋まってしまう問題を解決するためのスペース
        ImGui::Dummy(ImVec2(0.0f, 100.0f));
		ImGui::End();
    }
}

void TerrainRenderer::RenderCollisionStreamOut(const RenderContext& rc)
{
    ID3D11DeviceContext* dc = rc.deviceContext;

    // シェーダー設定
    dc->IASetInputLayout(_vertexShader.GetInputLayout());
    dc->VSSetShader(_vertexShader.Get(), nullptr, 0);
    dc->HSSetShader(_streamOutHullShader.Get(), nullptr, 0);
    dc->DSSetShader(_streamOutDomainShader.Get(), nullptr, 0);
    // 書き込みはしない
    dc->PSSetShader(nullptr, nullptr, 0);
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

    // 登録しているTerrainを描画
    for (const auto& drawInfo : _collisionExportVertexDrawInfos)
    {
        // 定数バッファの更新
        _data.world = drawInfo.world;
        dc->UpdateSubresource(_constantBuffer.Get(), 0, nullptr, &_data, 0, 0);

        // シェーダーリソースビューの設定
        ID3D11ShaderResourceView* srvs[] =
        {
            drawInfo.terrain->GetMaterialMapFB(Terrain::TextureQuality::High)->GetColorSRV(Terrain::BaseColorTextureIndex).Get(),
            drawInfo.terrain->GetMaterialMapFB(Terrain::TextureQuality::High)->GetColorSRV(Terrain::NormalTextureIndex).Get(),
            drawInfo.terrain->GetMaterialMapFB(Terrain::TextureQuality::High)->GetColorSRV(Terrain::ParameterTextureIndex).Get()
        };
        // Mipmapを使用するかどうか設定
        if (_isUsingMipmap)
            srvs[0] = drawInfo.terrain->GetMipmapBaseColorSRV().Get();
        dc->PSSetShaderResources(0, _countof(srvs), srvs);
        dc->DSSetShaderResources(0, _countof(srvs), srvs);
        // 頂点バッファとインデックスバッファを設定
        UINT stride = sizeof(TerrainRenderer::Vertex);
        UINT offset = 0;
        dc->IASetVertexBuffers(0, 1, drawInfo.terrain->GetVertexBuffer().GetAddressOf(), &stride, &offset);
        dc->IASetIndexBuffer(drawInfo.terrain->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);

        // コリジョン用に使うストリームアウトを書き出し
        {
            // コリジョン用ストリームアウトジオメトリシェーダーを設定
            dc->GSSetShader(_streamOutCollisionGeometryShader.Get(), nullptr, 0);

            //	ストリームアウト用の頂点バッファを設定
            UINT streamoutOffsets[1] = { 0 };
            dc->SOSetTargets(1, _streamOutCollisionVertexBuffer.GetAddressOf(), streamoutOffsets);

            // 描画
            D3D11_BUFFER_DESC bufferDesc{};
            drawInfo.terrain->GetIndexBuffer()->GetDesc(&bufferDesc);
            dc->DrawIndexed(bufferDesc.ByteWidth / sizeof(uint32_t), 0, 0);
            // 書き出した頂点情報をTerrainに送る
            // 受け取りバッファをクリア
            ID3D11Buffer* clear_streamout_buffer[] = { nullptr };
            UINT streamout_offsets[1] = { 0 };
            dc->SOSetTargets(1, clear_streamout_buffer, streamout_offsets);
            // CPUからアクセスするためのバッファにコピー
            dc->CopyResource(_streamOutCollisionCopyBuffer.Get(),
                _streamOutCollisionVertexBuffer.Get());
            // 取得した頂点情報からコリジョン用の頂点を算出
            D3D11_MAPPED_SUBRESOURCE mapped_resource;
            dc->Map(_streamOutCollisionCopyBuffer.Get(), 0,
                D3D11_MAP_READ, 0, &mapped_resource);
            if (mapped_resource.pData)
            {
                // 頂点情報を受け取り
                std::vector<Terrain::CollisionStreamOutVertex> streamOut;
                // 頂点数を計算
                const UINT size =
                    static_cast<UINT>(std::pow(DivisionCount, 2)) *
                    static_cast<UINT>(std::pow(_data.collisionTessFactor, 2)) *
                    6;
                streamOut.resize(size);
                CopyMemory(streamOut.data(), mapped_resource.pData, sizeof(Terrain::CollisionStreamOutVertex) * size);
                // マップ解除
                dc->Unmap(_streamOutCollisionCopyBuffer.Get(), 0);
                // Terrainに頂点情報を設定
                drawInfo.terrain->SetCollisionStreamOutData(dc, streamOut);
            }
            else
            {
                // マップ解除
                dc->Unmap(_streamOutCollisionCopyBuffer.Get(), 0);
            }
        }
    }

    // シェーダー解除
    dc->VSSetShader(nullptr, nullptr, 0);
    dc->HSSetShader(nullptr, nullptr, 0);
    dc->DSSetShader(nullptr, nullptr, 0);
    dc->GSSetShader(nullptr, nullptr, 0);
    dc->PSSetShader(nullptr, nullptr, 0);

	// 登録しているTerrainを描画した後はクリア
	_collisionExportVertexDrawInfos.clear();
}

void TerrainRenderer::RenderGrassStreamOut(const RenderContext& rc)
{
    ID3D11DeviceContext* dc = rc.deviceContext;

    // シェーダー設定
    dc->IASetInputLayout(_vertexShader.GetInputLayout());
    dc->VSSetShader(_vertexShader.Get(), nullptr, 0);
    dc->HSSetShader(_streamOutHullShader.Get(), nullptr, 0);
    dc->DSSetShader(_streamOutDomainShader.Get(), nullptr, 0);
    // 書き込みはしない
    dc->PSSetShader(nullptr, nullptr, 0);
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

    // 登録しているTerrainを描画
    for (const auto& drawInfo : _grassExportVertexDrawInfos)
    {
        // 定数バッファの更新
        _data.world = drawInfo.world;
        dc->UpdateSubresource(_constantBuffer.Get(), 0, nullptr, &_data, 0, 0);

        // シェーダーリソースビューの設定
        ID3D11ShaderResourceView* srvs[] =
        {
            drawInfo.terrain->GetMaterialMapFB(Terrain::TextureQuality::High)->GetColorSRV(Terrain::BaseColorTextureIndex).Get(),
            drawInfo.terrain->GetMaterialMapFB(Terrain::TextureQuality::High)->GetColorSRV(Terrain::NormalTextureIndex).Get(),
            drawInfo.terrain->GetMaterialMapFB(Terrain::TextureQuality::High)->GetColorSRV(Terrain::ParameterTextureIndex).Get()
        };
        // Mipmapを使用するかどうか設定
        if (_isUsingMipmap)
            srvs[0] = drawInfo.terrain->GetMipmapBaseColorSRV().Get();
        dc->PSSetShaderResources(0, _countof(srvs), srvs);
        dc->DSSetShaderResources(0, _countof(srvs), srvs);
        // 頂点バッファとインデックスバッファを設定
        UINT stride = sizeof(TerrainRenderer::Vertex);
        UINT offset = 0;
        dc->IASetVertexBuffers(0, 1, drawInfo.terrain->GetVertexBuffer().GetAddressOf(), &stride, &offset);
        dc->IASetIndexBuffer(drawInfo.terrain->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);

        // 草に使うストリームアウトを書き出し
        {
            // ストリームアウト用ジオメトリシェーダーを設定
            dc->GSSetShader(_streamOutGeometryShader.Get(), nullptr, 0);

            //	ストリームアウト用の頂点バッファを設定
            UINT streamoutOffsets[1] = { 0 };
            dc->SOSetTargets(1, drawInfo.terrain->GetStreamOutVertexBuffer().GetAddressOf(), streamoutOffsets);

            // 描画
            D3D11_BUFFER_DESC bufferDesc{};
            drawInfo.terrain->GetIndexBuffer()->GetDesc(&bufferDesc);
            dc->DrawIndexed(bufferDesc.ByteWidth / sizeof(uint32_t), 0, 0);

            // ターゲットを外す
            ID3D11Buffer* nullBuffer[] = { nullptr };
            dc->SOSetTargets(1, nullBuffer, streamoutOffsets);
        }
    }

    // シェーダー解除
    dc->VSSetShader(nullptr, nullptr, 0);
    dc->HSSetShader(nullptr, nullptr, 0);
    dc->DSSetShader(nullptr, nullptr, 0);
    dc->GSSetShader(nullptr, nullptr, 0);
    dc->PSSetShader(nullptr, nullptr, 0);

    // 登録しているTerrainを描画した後はクリア
    _grassExportVertexDrawInfos.clear();
}

void TerrainRenderer::RenderDynamic(const RenderContext& rc, bool writeGBuffer)
{
    ID3D11DeviceContext* dc = rc.deviceContext;

    // シェーダー設定
    dc->IASetInputLayout(_vertexShader.GetInputLayout());
    dc->VSSetShader(_vertexShader.Get(), nullptr, 0);
    dc->HSSetShader(_hullShader.Get(), nullptr, 0);
    dc->DSSetShader(_domainShader.Get(), nullptr, 0);
    dc->GSSetShader(nullptr, nullptr, 0);
    if (writeGBuffer)
    {
        if (_isDebugDraw)
            dc->PSSetShader(_debugPixelShader.Get(), nullptr, 0);
        else
            dc->PSSetShader(_gbPixelShader.Get(), nullptr, 0);

    }
    else
        dc->PSSetShader(_pixelShader.Get(), nullptr, 0);
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

    // 登録しているTerrainを描画
    for (const auto& drawInfo : _drawInfos)
    {
        // 定数バッファの更新
        _data.world = drawInfo.world;
        dc->UpdateSubresource(_constantBuffer.Get(), 0, nullptr, &_data, 0, 0);

        // シェーダーリソースビューの設定
        ID3D11ShaderResourceView* srvs[] =
        {
            drawInfo.terrain->GetMaterialMapFB(_currentTextureQuality)->GetColorSRV(Terrain::BaseColorTextureIndex).Get(),
            drawInfo.terrain->GetMaterialMapFB(_currentTextureQuality)->GetColorSRV(Terrain::NormalTextureIndex).Get(),
            drawInfo.terrain->GetMaterialMapFB(_currentTextureQuality)->GetColorSRV(Terrain::ParameterTextureIndex).Get()
        };
        // Mipmapを使用するかどうか設定
        if (_isUsingMipmap)
            srvs[0] = drawInfo.terrain->GetMipmapBaseColorSRV().Get();
        dc->PSSetShaderResources(0, _countof(srvs), srvs);
        dc->DSSetShaderResources(0, _countof(srvs), srvs);
        // 頂点バッファとインデックスバッファを設定
        UINT stride = sizeof(TerrainRenderer::Vertex);
        UINT offset = 0;
        dc->IASetVertexBuffers(0, 1, drawInfo.terrain->GetVertexBuffer().GetAddressOf(), &stride, &offset);
        dc->IASetIndexBuffer(drawInfo.terrain->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);
        // 描画
        D3D11_BUFFER_DESC bufferDesc{};
        drawInfo.terrain->GetIndexBuffer()->GetDesc(&bufferDesc);
        dc->DrawIndexed(bufferDesc.ByteWidth / sizeof(uint32_t), 0, 0);
    }

    // シェーダー解除
    dc->VSSetShader(nullptr, nullptr, 0);
    dc->HSSetShader(nullptr, nullptr, 0);
    dc->DSSetShader(nullptr, nullptr, 0);
    dc->GSSetShader(nullptr, nullptr, 0);
    dc->PSSetShader(nullptr, nullptr, 0);
}

void TerrainRenderer::RenderGrass(const RenderContext& rc, bool writeGBuffer)
{
    ID3D11DeviceContext* dc = rc.deviceContext;

    dc->UpdateSubresource(_grassConstantBuffer.Get(), 0, nullptr, &_dataGrass, 0, 0);
    dc->GSSetConstantBuffers(2, 1, _grassConstantBuffer.GetAddressOf());
    dc->HSSetConstantBuffers(2, 1, _grassConstantBuffer.GetAddressOf());
    dc->DSSetConstantBuffers(2, 1, _grassConstantBuffer.GetAddressOf());
    // シェーダー設定
    dc->VSSetShader(_grassVertexShader.Get(), nullptr, 0);
    dc->HSSetShader(_grassHullShader.Get(), nullptr, 0);
    dc->DSSetShader(_grassDomainShader.Get(), nullptr, 0);
    dc->GSSetShader(_grassGeometryShader.Get(), nullptr, 0);
    if (writeGBuffer)
        dc->PSSetShader(_grassGBPixelShader.Get(), nullptr, 0);
    else
        dc->PSSetShader(_grassPixelShader.Get(), nullptr, 0);

    // 風用のテクスチャを設定
    dc->GSSetShaderResources(5, 1, _windDistortionSRV.GetAddressOf());
    // 草のテクスチャを設定
    dc->PSSetShaderResources(4, 1, _grassColorSRV.GetAddressOf());

    // 入力レイアウト設定
    dc->IASetInputLayout(_grassVertexShader.GetInputLayout());
    // バッファ
    UINT stride = sizeof(Terrain::StreamOutVertex);
    UINT offset = 0;
    dc->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

    if (_isDrawingGrass)
    {
        // 草の描画
        for (const auto& drawInfo : _grassDrawInfos)
        {
            dc->IASetVertexBuffers(0, 1, drawInfo.terrain->GetStreamOutVertexBuffer().GetAddressOf(), &stride, &offset);

            dc->DrawAuto();
        }
    }

    ID3D11ShaderResourceView* nullSRVs[3] = {};
    dc->GSSetShaderResources(5, 1, nullSRVs);
    dc->PSSetShaderResources(4, 1, nullSRVs);
    // シェーダーを解除
    dc->VSSetShader(nullptr, nullptr, 0);
    dc->HSSetShader(nullptr, nullptr, 0);
    dc->DSSetShader(nullptr, nullptr, 0);
    dc->GSSetShader(nullptr, nullptr, 0);
    dc->PSSetShader(nullptr, nullptr, 0);

}
