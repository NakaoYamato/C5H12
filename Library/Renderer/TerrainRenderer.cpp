#include "TerrainRenderer.h"

#include "../HRTrace.h"
#include "../../Library/Graphics/GpuResourceManager.h"

#include <Mygui.h>

// 初期化
void TerrainRenderer::Initialize(ID3D11Device* device)
{
	const UINT MaxTess = static_cast<UINT>(MaxTessellation);
	const UINT StreamOutMaxVertex =	
        (DivisionCount * 2) * 
        (DivisionCount * 2 - 1) * 
        ((MaxTess - 1) * (MaxTess - 2)) / 2;

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
            {   "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        // 頂点シェーダー
        GpuResourceManager::CreateVsFromCso(
            device,
            "./Data/Shader/HLSL/Terrain/TerrainVS.cso",
            _vertexShader.ReleaseAndGetAddressOf(),
            _inputLayout.ReleaseAndGetAddressOf(),
            inputElementDesc,
            static_cast<UINT>(_countof(inputElementDesc)));
        // ハルシェーダー
        GpuResourceManager::CreateHsFromCso(
            device,
            "./Data/Shader/HLSL/Terrain/TerrainHS.cso",
            _hullShader.ReleaseAndGetAddressOf());
        // ドメインシェーダー
        GpuResourceManager::CreateDsFromCso(
            device,
            "./Data/Shader/HLSL/Terrain/TerrainDS.cso",
            _domainShader.ReleaseAndGetAddressOf());
        // LODを使用しないハルシェーダー
		GpuResourceManager::CreateHsFromCso(
			device,
			"./Data/Shader/HLSL/Terrain/TerrainNonLODHS.cso",
			_nonLODHullShader.ReleaseAndGetAddressOf());
        // ピクセルシェーダー
        GpuResourceManager::CreatePsFromCso(
            device,
            "./Data/Shader/HLSL/Terrain/TerrainPS.cso",
            _pixelShader.ReleaseAndGetAddressOf());
        // GBufferへ書き込む用のピクセルシェーダー
        GpuResourceManager::CreatePsFromCso(
            device,
            "./Data/Shader/HLSL/Terrain/TerrainGBPS.cso",
            _gbPixelShader.ReleaseAndGetAddressOf());
    }

	// 静的描画用シェーダー作成
    {
		// 頂点シェーダー作成
		GpuResourceManager::CreateVsFromCso(
			device,
			"./Data/Shader/HLSL/Terrain/TerrainStaticVS.cso",
			_staticVertexShader.ReleaseAndGetAddressOf(),
            nullptr,
            nullptr,
            0);
    }

    // 草描画用シェーダー作成
	{
        // 頂点シェーダー作成
        GpuResourceManager::CreateVsFromCso(
            device, 
            "./Data/Shader/HLSL/Terrain/Grass/TerrainGrassVS.cso",
            _grassVertexShader.GetAddressOf(),
            nullptr, 
            nullptr, 
            0);
        // ハルシェーダー
        GpuResourceManager::CreateHsFromCso(
            device,
            "./Data/Shader/HLSL/Terrain/Grass/TerrainGrassHS.cso",
            _grassHullShader.ReleaseAndGetAddressOf());
        // ドメインシェーダー
        GpuResourceManager::CreateDsFromCso(
            device,
            "./Data/Shader/HLSL/Terrain/Grass/TerrainGrassDS.cso",
            _grassDomainShader.ReleaseAndGetAddressOf());
		// ジオメトリシェーダー作成
		GpuResourceManager::CreateGsFromCso(
			device,
			"./Data/Shader/HLSL/Terrain/Grass/TerrainGrassGS.cso",
			_grassGeometryShader.GetAddressOf());
		// ピクセルシェーダー作成
		GpuResourceManager::CreatePsFromCso(
			device,
			"./Data/Shader/HLSL/Terrain/Grass/TerrainGrassPS.cso",
            _grassPixelShader.GetAddressOf());
        // GBufferへ書き込む用のピクセルシェーダー
        GpuResourceManager::CreatePsFromCso(
            device,
            "./Data/Shader/HLSL/Terrain/Grass/TerrainGrassGBPS.cso",
            _grassGBPixelShader.ReleaseAndGetAddressOf());
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
        GpuResourceManager::CreateGsWithStreamOutFromCso(
            device,
            "./Data/Shader/HLSL/Terrain/TerrainGS.cso",
            _streamOutGeometryShader.ReleaseAndGetAddressOf(),
            declaration, _countof(declaration),
            bufferStrides, _countof(bufferStrides),
            0);
    }

    // ストリームアウトプットされた情報の受け取り用バッファ
    {
        D3D11_BUFFER_DESC desc{};
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.ByteWidth = sizeof(Terrain::StreamOutVertex) * StreamOutMaxVertex;
        desc.BindFlags = D3D11_BIND_STREAM_OUTPUT;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        HRESULT hr = device->CreateBuffer(&desc, nullptr,
            _streamOutVertexBuffer.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
    }

    // CPUからアクセスするためのバッファ
    {
        D3D11_BUFFER_DESC desc{};
        desc.Usage = D3D11_USAGE_STAGING;
        desc.ByteWidth = sizeof(Terrain::StreamOutVertex) * StreamOutMaxVertex;
        desc.BindFlags = 0;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        HRESULT hr = device->CreateBuffer(&desc, nullptr,
            _streamOutCopyBuffer.ReleaseAndGetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
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
// 描画登録
void TerrainRenderer::Draw(Terrain* terrain, const DirectX::XMFLOAT4X4& world, bool isExportingVertices)
{
	DrawInfo drawInfo;
	drawInfo.terrain = terrain;
	drawInfo.world = world;
	drawInfo.isExportingVertices = isExportingVertices;

    if (!isExportingVertices && terrain->GetStreamOutData().size() != 0 && _isStaticDraw)
    {
		_staticDrawInfos.push_back(drawInfo);
    }
    else
    {
        _drawInfos.push_back(drawInfo);
    }

	// 草の描画登録
	_grassDrawInfos.push_back(drawInfo);
}
// 描画処理
void TerrainRenderer::Render(const RenderContext& rc, bool writeGBuffer)
{
    ID3D11DeviceContext* dc = rc.deviceContext;

    if (_isWireFrame)
        dc->RSSetState(rc.renderState->GetRasterizerState(RasterizerState::WireCullNone));
    else
        dc->RSSetState(rc.renderState->GetRasterizerState(RasterizerState::SolidCullNone));
	// サンプラーを設定
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
    // シェーダー設定
    dc->IASetInputLayout(_inputLayout.Get());
    dc->VSSetShader(_vertexShader.Get(), nullptr, 0);
    dc->HSSetShader(_hullShader.Get(), nullptr, 0);
    dc->DSSetShader(_domainShader.Get(), nullptr, 0);
    if (writeGBuffer)
        dc->PSSetShader(_gbPixelShader.Get(), nullptr, 0);
    else
        dc->PSSetShader(_pixelShader.Get(), nullptr, 0);
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

	// 登録しているTerrainを描画
	for (const auto& drawInfo : _drawInfos)
	{
		// 定数バッファの更新
        _data.world = drawInfo.world;
		dc->UpdateSubresource(_constantBuffer.Get(), 0, nullptr, &_data, 0, 0);

		// 頂点情報を書き出す場合ストリームアウトを有効にする
        if (drawInfo.isExportingVertices)
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

		// シェーダーリソースビューの設定
		ID3D11ShaderResourceView* srvs[] =
		{
			drawInfo.terrain->GetMaterialMapFB()->GetColorSRV(Terrain::BaseColorTextureIndex).Get(),
			drawInfo.terrain->GetMaterialMapFB()->GetColorSRV(Terrain::NormalTextureIndex).Get()
		};
		dc->PSSetShaderResources(0, _countof(srvs), srvs);
		dc->DSSetShaderResources(0, _countof(srvs), srvs);
        // パラメータマップ設定
		dc->DSSetShaderResources(ParameterMapSRVIndex, 1, drawInfo.terrain->GetParameterMapFB()->GetColorSRV().GetAddressOf());
		dc->PSSetShaderResources(ParameterMapSRVIndex, 1, drawInfo.terrain->GetParameterMapFB()->GetColorSRV().GetAddressOf());
        // 頂点バッファとインデックスバッファを設定
        UINT stride = sizeof(TerrainRenderer::Vertex);
        UINT offset = 0;
        dc->IASetVertexBuffers(0, 1, drawInfo.terrain->GetVertexBuffer().GetAddressOf(), &stride, &offset);
        dc->IASetIndexBuffer(drawInfo.terrain->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);
		// 描画
        D3D11_BUFFER_DESC bufferDesc{};
        drawInfo.terrain->GetIndexBuffer()->GetDesc(&bufferDesc);
        dc->DrawIndexed(bufferDesc.ByteWidth / sizeof(uint32_t), 0, 0);

        // 書き出した頂点情報をTerrainに送る
        if (drawInfo.isExportingVertices)
        {
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
                std::vector<Terrain::StreamOutVertex> streamOut;
				// 頂点数を計算
				UINT size = 3 * static_cast<UINT>(_data.edgeFactor) +
                    static_cast<UINT>(((_data.innerFactor - 1) * (_data.innerFactor - 2)) / 2);
				size *= static_cast<UINT>(std::pow(DivisionCount, 2)) * 2 * 3 * 3; // パッチ数
				//UINT size = 3 * 3 * 9 * static_cast<UINT>(_data.edgeFactor) * static_cast<UINT>(_data.innerFactor);
                streamOut.resize(size);
				CopyMemory(streamOut.data(), mapped_resource.pData, sizeof(Terrain::StreamOutVertex) * size);
				// マップ解除
				dc->Unmap(_streamOutCopyBuffer.Get(), 0);
				// Terrainに頂点情報を設定
				drawInfo.terrain->SetStreamOutData(dc, streamOut);
			}
			else
			{
				// マップ解除
				dc->Unmap(_streamOutCopyBuffer.Get(), 0);
            }
        }
	}

    dc->RSSetState(rc.renderState->GetRasterizerState(RasterizerState::SolidCullNone));
    // シェーダー解除
    dc->VSSetShader(nullptr, nullptr, 0);
    dc->HSSetShader(nullptr, nullptr, 0);
    dc->DSSetShader(nullptr, nullptr, 0);
    dc->GSSetShader(nullptr, nullptr, 0);
    dc->PSSetShader(nullptr, nullptr, 0);

    // 静的描画
    {
        dc->IASetInputLayout(nullptr);
        dc->VSSetShader(_staticVertexShader.Get(), nullptr, 0);
        if (writeGBuffer)
            dc->PSSetShader(_gbPixelShader.Get(), nullptr, 0);
        else
            dc->PSSetShader(_pixelShader.Get(), nullptr, 0);
        // 頂点バッファ設定
        ID3D11Buffer* clearBuffer[] = { nullptr };
        UINT strides[] = { 0 };
        UINT offsets[] = { 0 };
        dc->IASetVertexBuffers(0, 1, clearBuffer, strides, offsets);
        dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        for (const auto& drawInfo : _staticDrawInfos)
        {
            // 定数バッファの更新
            _data.world = drawInfo.world;
            dc->UpdateSubresource(_constantBuffer.Get(), 0, nullptr, &_data, 0, 0);

            // シェーダーリソースビューの設定
            ID3D11ShaderResourceView* srvs[] =
            {
                drawInfo.terrain->GetMaterialMapFB()->GetColorSRV(Terrain::BaseColorTextureIndex).Get(),
                drawInfo.terrain->GetMaterialMapFB()->GetColorSRV(Terrain::NormalTextureIndex).Get(),
                drawInfo.terrain->GetStreamOutSRV().Get(),
            };
            dc->VSSetShaderResources(0, _countof(srvs), srvs);
            dc->PSSetShaderResources(0, _countof(srvs), srvs);

            dc->Draw(static_cast<UINT>(drawInfo.terrain->GetStreamOutData().size()), 0);
        }
    }

    // シェーダーリソースビューを解除
    ID3D11ShaderResourceView* nullSRVs[3] = {};
    dc->PSSetShaderResources(0, _countof(nullSRVs), nullSRVs);
    dc->VSSetShaderResources(0, _countof(nullSRVs), nullSRVs);
    dc->DSSetShaderResources(0, _countof(nullSRVs), nullSRVs);
    dc->DSSetShaderResources(ParameterMapSRVIndex, 1, nullSRVs);
    dc->PSSetShaderResources(ParameterMapSRVIndex, 1, nullSRVs);
    dc->GSSetShaderResources(ParameterMapSRVIndex, 1, nullSRVs);

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
    dc->IASetInputLayout(nullptr);
    // バッファクリア
    ID3D11Buffer* clearBuffer[] = { nullptr };
    UINT strides[] = { 0 };
    UINT offsets[] = { 0 };
    dc->IASetVertexBuffers(0, 1, clearBuffer, strides, offsets);
    dc->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

    if (_isDrawingGrass)
    {
        // 草の描画
        for (const auto& drawInfo : _grassDrawInfos)
        {
            // シェーダーリソースビューの設定
            ID3D11ShaderResourceView* srvs[] =
            {
                drawInfo.terrain->GetStreamOutSRV().Get(),
            };
            dc->VSSetShaderResources(0, _countof(srvs), srvs);
            dc->PSSetShaderResources(0, _countof(srvs), srvs);

            // 頂点数分コール
            dc->Draw(static_cast<UINT>(drawInfo.terrain->GetStreamOutData().size()), 0);
        }
    }

    dc->GSSetShaderResources(5, 1, nullSRVs);
    dc->PSSetShaderResources(4, 1, nullSRVs);
    // シェーダーを解除
    dc->VSSetShader(nullptr, nullptr, 0);
    dc->HSSetShader(nullptr, nullptr, 0);
    dc->DSSetShader(nullptr, nullptr, 0);
    dc->GSSetShader(nullptr, nullptr, 0);
    dc->PSSetShader(nullptr, nullptr, 0);


    // 登録しているTerrainを描画した後はクリア
    _drawInfos.clear();
    _staticDrawInfos.clear();
    _grassDrawInfos.clear();
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
			ImGui::Checkbox(u8"静的描画", &_isStaticDraw);
			ImGui::Checkbox(u8"草を描画", &_isDrawingGrass);
			ImGui::Checkbox(u8"ワイヤーフレーム", &_isWireFrame);
			ImGui::Separator();

            if (ImGui::TreeNode(u8"定数バッファ"))
            {
                ImGui::SliderFloat(u8"Edge Factor", &_data.edgeFactor, 1.0f, MaxTessellation, "%.1f");
                ImGui::SliderFloat(u8"Inner Factor", &_data.innerFactor, 1.0f, MaxTessellation, "%.1f");
                ImGui::SliderFloat(u8"LOD Distance", &_data.lodDistanceMax, 1.0f, 200.0f, "%.1f");
				ImGui::SliderFloat(u8"LOD Low Factor", &_data.lodLowFactor, 1.0f, MaxTessellation, "%.1f");

                ImGui::SliderFloat(u8"Emissive", &_data.emissive, 0.0f, 1.0f, "%.2f");
                ImGui::SliderFloat(u8"Metalness", &_data.metalness, 0.0f, 1.0f, "%.2f");
                ImGui::SliderFloat(u8"Roughness", &_data.roughness, 0.0f, 1.0f, "%.2f");
                ImGui::TreePop();
            }
            if (ImGui::TreeNode(u8"草の定数バッファ"))
            {
                ImGui::SliderFloat(u8"分割数", &_dataGrass.grassTessellation, 1.0f, MaxTessellation, "%.1f");
                ImGui::SliderFloat(u8"LOD", &_dataGrass.lodDistanceMax, 1.0f, 200.0f, "%.1f");
                ImGui::DragFloat(u8"高さ", &_dataGrass.height, 0.01f, 0.01f, 10.0f, "%.2f");
                ImGui::DragFloat(u8"幅", &_dataGrass.width, 0.001f, 0.001f, 10.0f, "%.2f");
				ImGui::DragFloat(u8"枯れ具合", &_dataGrass.witherdFactor, 0.01f, 0.01f, 1.0f, "%.2f");
                ImGui::DragFloat(u8"曲率", &_dataGrass.curvature, 0.01f, 0.01f, 10.0f, "%.2f");
                ImGui::DragFloat(u8"高さのズレ", &_dataGrass.heightVariance, 0.01f, 0.01f, 10.0f, "%.2f");
                ImGui::DragFloat(u8"ノイズ", &_dataGrass.parlinNoiseDistribution, 0.01f, 0.01f, 10.0f, "%.2f");
                ImGui::ColorEdit4(u8"Specular Color", &_dataGrass.specularColor.x);
                ImGui::TreePop();
            }
        }
		ImGui::End();
    }
}
