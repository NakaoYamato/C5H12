#include "PrimitiveRenderer.h"

#include <memory>

#include "../../Library/HRTrace.h"
#include "../../Library/Graphics/GpuResourceManager.h"

// 初期化処理
void PrimitiveRenderer::Initialize(ID3D11Device* device)
{
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	// 頂点シェーダー
	GpuResourceManager::CreateVsFromCso(
		device,
		"./Data/Shader/HLSL/PrimitiveRenderer/PrimitiveRendererVS.cso",
		vertexShader.ReleaseAndGetAddressOf(),
		inputLayout.ReleaseAndGetAddressOf(),
		inputElementDesc,
		_countof(inputElementDesc)
	);

	// ピクセルシェーダー
	GpuResourceManager::CreatePsFromCso(
		device,
		"./Data/Shader/HLSL/PrimitiveRenderer/PrimitiveRendererPS.cso",
		pixelShader.ReleaseAndGetAddressOf()
	);

	// 定数バッファ
	(void)GpuResourceManager::CreateConstantBuffer(
		device,
		sizeof(CbScene),
		constantBuffer.GetAddressOf());

	// 頂点バッファ
	D3D11_BUFFER_DESC desc{};
	desc.ByteWidth = sizeof(Vertex) * VertexCapacity;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	HRESULT hr = device->CreateBuffer(&desc, nullptr, vertexBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	// ノイズテクスチャの読み込み
	GpuResourceManager::LoadTextureFromFile(
		device,
		L"./Data/Texture/Noise/Noise000.png",
		_noiseSRV.ReleaseAndGetAddressOf(),
		nullptr);
	// 距離テクスチャの読み込み
	GpuResourceManager::LoadTextureFromFile(
		device,
		L"./Data/Texture/Noise/NormalMap.png",
		_distanceSRV.ReleaseAndGetAddressOf(),
		nullptr);
}

// 頂点追加
void PrimitiveRenderer::AddVertex(const Vector3& position, const Vector4& color)
{
	Vertex& v = vertices.emplace_back();
	v.position = position;
	v.color = color;
}

// 描画実行
void PrimitiveRenderer::Render(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	dc->RSGetViewports(&numViewports, &viewport);

	// シェーダー設定
	dc->VSSetShader(vertexShader.Get(), nullptr, 0);
	dc->PSSetShader(pixelShader.Get(), nullptr, 0);
	dc->IASetInputLayout(inputLayout.Get());

	static constexpr int CBIndex = 1;
	Microsoft::WRL::ComPtr<ID3D11Buffer> cachePSBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> cacheVSBuffer;
	dc->VSGetConstantBuffers(CBIndex, 1, cacheVSBuffer.ReleaseAndGetAddressOf());
	dc->PSGetConstantBuffers(CBIndex, 1, cachePSBuffer.ReleaseAndGetAddressOf());
	// 定数バッファ設定
	dc->VSSetConstantBuffers(CBIndex, 1, constantBuffer.GetAddressOf());
	dc->PSSetConstantBuffers(CBIndex, 1, constantBuffer.GetAddressOf());

	// 定数バッファ更新
	CbScene cbScene;
	cbScene.viewportSize.x = viewport.Width;
	cbScene.viewportSize.y = viewport.Height;
	cbScene.vertexCount = static_cast<UINT>(vertices.size());
	dc->UpdateSubresource(constantBuffer.Get(), 0, 0, &cbScene, 0, 0);

	// 頂点バッファ設定
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	dc->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	dc->PSSetShaderResources(3, 1, _distanceSRV.GetAddressOf());
	dc->PSSetShaderResources(4, 1, _noiseSRV.GetAddressOf());

	// 描画
	UINT totalVertexCount = static_cast<UINT>(vertices.size());
	UINT start = 0;
	UINT count = (totalVertexCount < VertexCapacity) ? totalVertexCount : VertexCapacity;

	while (start < totalVertexCount)
	{
		D3D11_MAPPED_SUBRESOURCE mappedSubresource;
		HRESULT hr = dc->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		memcpy(mappedSubresource.pData, &vertices[start], sizeof(Vertex) * count);

		dc->Unmap(vertexBuffer.Get(), 0);

		dc->Draw(count, 0);

		start += count;
		if ((start + count) > totalVertexCount)
		{
			count = totalVertexCount - start;
		}
	}

	vertices.clear();

	dc->VSSetShader(NULL, 0, 0);
	dc->PSSetShader(NULL, 0, 0);
	dc->VSSetConstantBuffers(CBIndex, 1, cacheVSBuffer.GetAddressOf());
	dc->PSSetConstantBuffers(CBIndex, 1, cachePSBuffer.GetAddressOf());
}
