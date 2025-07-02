#include "Terrain.h"

#include "../HRTrace.h"
#include "../../Library/Graphics/GpuResourceManager.h"
#include "../../Library/Exporter/Exporter.h"
#include "../../Library/DebugSupporter/DebugSupporter.h"

#include <imgui.h>

Terrain::Terrain(ID3D11Device* device)
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

    // 地形用テクスチャの読み込み
    GpuResourceManager::LoadTextureFromFile(
        device,
        L"./Data/Texture/Terrain/001_COLOR.png",
        _colorSRVs[0].ReleaseAndGetAddressOf(),
        nullptr);
    GpuResourceManager::LoadTextureFromFile(
        device,
        L"./Data/Texture/Terrain/002_COLOR.png",
        _colorSRVs[1].ReleaseAndGetAddressOf(),
        nullptr);
    GpuResourceManager::LoadTextureFromFile(
        device,
        L"./Data/Texture/Terrain/003_COLOR.jpg",
        _colorSRVs[2].ReleaseAndGetAddressOf(),
        nullptr);
    GpuResourceManager::LoadTextureFromFile(
        device,
        L"./Data/Texture/Terrain/001_NORMAL.png",
        _normalSRVs[0].ReleaseAndGetAddressOf(),
        nullptr);
    GpuResourceManager::LoadTextureFromFile(
        device,
        L"./Data/Texture/Terrain/002_NORMAL.png",
        _normalSRVs[1].ReleaseAndGetAddressOf(),
        nullptr);
    GpuResourceManager::LoadTextureFromFile(
        device,
        L"./Data/Texture/Terrain/003_NORMAL.png",
        _normalSRVs[2].ReleaseAndGetAddressOf(),
        nullptr);
    // パラメータマップの読み込み
    _parameterMapFB = std::make_unique<FrameBuffer>(device, ParameterMapSize, ParameterMapSize, true);
	// データマップのフレームバッファを作成
	_dataMapFB = std::make_unique<FrameBuffer>(device, ParameterMapSize, ParameterMapSize, true);

    // ストリームアウトを使用してプリミティブを取得するシェーダー
    {
        D3D11_SO_DECLARATION_ENTRY declaration[] =
        {
            {0,"SV_POSITION",	0,0,4,0},
            {0,"WORLD_POSITION",0,0,3,0},
            {0,"NORMAL",		0,0,3,0},
            {0,"TEXCOORD",		0,0,2,0},
            {0,"BLEND_RATE",	0,0,4,0},
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
}
// 描画処理
void Terrain::Render(const RenderContext& rc, const DirectX::XMFLOAT4X4& world, bool writeGBuffer)
{
    ID3D11DeviceContext* dc = rc.deviceContext;

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
        _colorSRVs[0].Get(),
        _colorSRVs[1].Get(),
        _colorSRVs[2].Get(),
        _normalSRVs[0].Get(),
        _normalSRVs[1].Get(),
        _normalSRVs[2].Get(),
    };
    dc->PSSetShaderResources(0, _countof(srvs), srvs);
    // パラメータマップ設定
    dc->DSSetShaderResources(ParameterMapIndex, 1, _parameterMapFB->GetColorSRV().GetAddressOf());
    dc->PSSetShaderResources(ParameterMapIndex, 1, _parameterMapFB->GetColorSRV().GetAddressOf());
	// データマップ設定
    dc->GSSetShaderResources(DataMapIndex, 1, _dataMapFB->GetColorSRV().GetAddressOf());
    // 頂点バッファとインデックスバッファを設定
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    dc->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);
    dc->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
    // 描画
    dc->DrawIndexed(static_cast<UINT>(_indices.size()), 0, 0);

    // シェーダーリソースビューを解除
    ID3D11ShaderResourceView* nullSRVs[] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
    dc->PSSetShaderResources(0, _countof(nullSRVs), nullSRVs);
    dc->DSSetShaderResources(ParameterMapIndex, 1, nullSRVs);
    dc->PSSetShaderResources(ParameterMapIndex, 1, nullSRVs);
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
// デバッグ描画
void Terrain::DebugRender(const DirectX::XMFLOAT4X4& world)
{
    auto& streamOutData = GetStreamOutData();
    if (streamOutData.empty())
        return;

    if (!_drawWireframe)
        return;

    for (size_t i = 0; i < streamOutData.size(); i += 3)
    {
        const auto& v1 = streamOutData[i + 0];
        const auto& v2 = streamOutData[i + 1];
        const auto& v3 = streamOutData[i + 2];
        Vector4 color = Vector4::Red;
		color.x = Vector4::Red.x * v1.cost;
        Debug::Renderer::AddVertex(v1.worldPosition, color);
        color.x = Vector4::Red.x * v2.cost;
        Debug::Renderer::AddVertex(v2.worldPosition, color);
        color.x = Vector4::Red.x * v2.cost;
        Debug::Renderer::AddVertex(v2.worldPosition, color);
        color.x = Vector4::Red.x * v3.cost;
        Debug::Renderer::AddVertex(v3.worldPosition, color);
        color.x = Vector4::Red.x * v3.cost;
        Debug::Renderer::AddVertex(v3.worldPosition, color);
        color.x = Vector4::Red.x * v1.cost;
        Debug::Renderer::AddVertex(v1.worldPosition, color);
    }
}
// GUI描画
void Terrain::DrawGui()
{
    // ワイヤーフレーム描画フラグを切り替えるチェックボックス
    ImGui::Checkbox(u8"ワイヤーフレーム描画", &_drawWireframe);
	ImGui::Text(u8"パラメータマップ");
    ImGui::Image(_parameterMapFB->GetColorSRV().Get(), ImVec2(256, 256), ImVec2(0, 0), ImVec2(1, 1));
	ImGui::Text(u8"データマップ");
    ImGui::Image(_dataMapFB->GetColorSRV().Get(), ImVec2(256, 256), ImVec2(0, 0), ImVec2(1, 1));
	ImGui::ColorEdit4(u8"Color", &_data.baseColor.x, ImGuiColorEditFlags_NoInputs);
    ImGui::SliderFloat(u8"Edge Factor", &_data.edgeFactor, 1.0f, 128.0f, "%.1f");
    ImGui::SliderFloat(u8"Inner Factor", &_data.innerFactor, 1.0f, 128.0f, "%.1f");
    ImGui::SliderFloat(u8"Height Scaler", &_data.heightScaler, 0.1f, 10.0f, "%.1f");
    ImGui::SliderFloat(u8"Tilling Scale", &_data.tillingScale, 0.1f, 10.0f, "%.1f");
    ImGui::SliderFloat(u8"Emissive", &_data.emissive, 0.0f, 1.0f, "%.2f");
    ImGui::SliderFloat(u8"Metalness", &_data.metalness, 0.0f, 1.0f, "%.2f");
    ImGui::SliderFloat(u8"Roughness", &_data.roughness, 0.0f, 1.0f, "%.2f");
    ImGui::Checkbox(u8"頂点書き出し", &_streamOut);
    ImGui::Image(_colorSRVs[0].Get(), ImVec2(256, 256), ImVec2(0, 0), ImVec2(1, 1));
    ImGui::Image(_normalSRVs[0].Get(), ImVec2(256, 256), ImVec2(0, 0), ImVec2(1, 1));
    ImGui::Image(_colorSRVs[1].Get(), ImVec2(256, 256), ImVec2(0, 0), ImVec2(1, 1));
    ImGui::Image(_normalSRVs[1].Get(), ImVec2(256, 256), ImVec2(0, 0), ImVec2(1, 1));
    ImGui::Image(_colorSRVs[2].Get(), ImVec2(256, 256), ImVec2(0, 0), ImVec2(1, 1));
    ImGui::Image(_normalSRVs[2].Get(), ImVec2(256, 256), ImVec2(0, 0), ImVec2(1, 1));
    ImGui::Separator();
}
// パラメータマップの書き出し
void Terrain::SaveParameterMap(ID3D11Device* device, ID3D11DeviceContext* dc, const wchar_t* parameterMapPath)
{
    if (Exporter::SaveDDSFile(device, dc,
        _parameterMapFB->GetColorSRV().Get(),
        parameterMapPath))
    {
    }
}
// データマップの書き出し
void Terrain::SaveDataMap(ID3D11Device* device, ID3D11DeviceContext* dc, const wchar_t* dataMapPath)
{
    if (Exporter::SaveDDSFile(device, dc,
        _dataMapFB->GetColorSRV().Get(),
        dataMapPath))
    {
    }
}
// 地形メッシュの頂点とインデックスを生成
void Terrain::CreateTerrainMesh(ID3D11Device* device)
{
    // 地形メッシュの頂点とインデックスを生成
    _vertices.clear();
    _vertices =
    {
        {{-1.0f,  0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},// 左下
        {{-1.0f,  0.0f,  1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},// 左上
        {{ 1.0f,  0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},// 右下
        {{ 1.0f,  0.0f,  1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},// 右上
    };
    _indices.clear();
    _indices = {
        0, 1, 2, // 左下, 左上, 右下
        1, 3, 2  // 左上, 右上, 右下
    };

    // 頂点バッファとインデックスバッファを作成
    HRESULT hr{ S_OK };
    D3D11_BUFFER_DESC buffer_desc{};
    D3D11_SUBRESOURCE_DATA subresource_data{};
    buffer_desc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * _vertices.size());
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    buffer_desc.CPUAccessFlags = 0;
    buffer_desc.MiscFlags = 0;
    buffer_desc.StructureByteStride = 0;
    subresource_data.pSysMem = _vertices.data();
    subresource_data.SysMemPitch = 0;
    subresource_data.SysMemSlicePitch = 0;
    hr = device->CreateBuffer(&buffer_desc, &subresource_data,
        _vertexBuffer.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    buffer_desc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * _indices.size());
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    subresource_data.pSysMem = _indices.data();
    hr = device->CreateBuffer(&buffer_desc, &subresource_data,
        _indexBuffer.ReleaseAndGetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
}
