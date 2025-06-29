#include "Terrain.h"

#include "../HRTrace.h"
#include "../../Library/Graphics/GpuResourceManager.h"

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
        L"./Data/Texture/Terrain/001.png",
        _baseSRVs[0].ReleaseAndGetAddressOf(),
        nullptr);
    GpuResourceManager::LoadTextureFromFile(
        device,
        L"./Data/Texture/Terrain/002.png",
        _baseSRVs[1].ReleaseAndGetAddressOf(),
        nullptr);
    GpuResourceManager::LoadTextureFromFile(
        device,
        L"./Data/Texture/Terrain/003.png",
        _baseSRVs[2].ReleaseAndGetAddressOf(),
        nullptr);
    // ハイトマップの読み込み
    GpuResourceManager::MakeDummyTexture(
        device,
        _heightMapSRV.ReleaseAndGetAddressOf(),
        0x00000000, 1
    );
    //GpuResourceManager::LoadTextureFromFile(
    //    device,
    //    L"./Data/Texture/TerrainHeightMap.png",
    //    _heightMapSRV.ReleaseAndGetAddressOf(),
    //    nullptr);
}

void Terrain::Render(const RenderContext& rc, DirectX::XMFLOAT4X4 world, bool writeGBuffer)
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
    // 地形用テクスチャ設定
    ID3D11ShaderResourceView* srvs[] =
    {
        _baseSRVs[0].Get(),
        _baseSRVs[1].Get(),
        _baseSRVs[2].Get(),
    };
    dc->PSSetShaderResources(0, _countof(srvs), srvs);
    // ハイトマップ設定
    dc->DSSetShaderResources(5, 1, _heightMapSRV.GetAddressOf());
    dc->PSSetShaderResources(5, 1, _heightMapSRV.GetAddressOf());
    // 頂点バッファとインデックスバッファを設定
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    dc->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);
    dc->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
    // 描画
    dc->DrawIndexed(static_cast<UINT>(_indices.size()), 0, 0);

    // シェーダーリソースビューを解除
    ID3D11ShaderResourceView* nullSRVs[] = { nullptr, nullptr, nullptr };
    dc->PSSetShaderResources(0, _countof(nullSRVs), nullSRVs);
    dc->DSSetShaderResources(5, 1, nullSRVs);
    dc->PSSetShaderResources(5, 1, nullSRVs);
    // シェーダーを解除
    dc->VSSetShader(nullptr, nullptr, 0);
    dc->HSSetShader(nullptr, nullptr, 0);
    dc->DSSetShader(nullptr, nullptr, 0);
    dc->GSSetShader(nullptr, nullptr, 0);
    dc->PSSetShader(nullptr, nullptr, 0);
}

void Terrain::DrawGui()
{
    ImGui::SliderFloat("Edge Factor", &_data.edgeFactor, 1.0f, 128.0f, "%.1f");
    ImGui::SliderFloat("Inner Factor", &_data.innerFactor, 1.0f, 128.0f, "%.1f");
    ImGui::SliderFloat("Height Scaler", &_data.heightScaler, 0.1f, 10.0f, "%.1f");
    ImGui::SliderFloat("Tilling Scale", &_data.tillingScale, 0.1f, 10.0f, "%.1f");
    ImGui::SliderFloat("Emissive", &_data.emissive, 0.0f, 1.0f, "%.2f");
    ImGui::SliderFloat("Metalness", &_data.metalness, 0.0f, 1.0f, "%.2f");
    ImGui::SliderFloat("Roughness", &_data.roughness, 0.0f, 1.0f, "%.2f");
    ImGui::Image(_baseSRVs[0].Get(), ImVec2(256, 256), ImVec2(0, 0), ImVec2(1, 1));
    ImGui::Image(_baseSRVs[1].Get(), ImVec2(256, 256), ImVec2(0, 0), ImVec2(1, 1));
    ImGui::Image(_baseSRVs[2].Get(), ImVec2(256, 256), ImVec2(0, 0), ImVec2(1, 1));
    ImGui::Image(_heightMapSRV.Get(), ImVec2(256, 256), ImVec2(0, 0), ImVec2(1, 1));
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
