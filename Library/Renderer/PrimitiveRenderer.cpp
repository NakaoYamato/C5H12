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
	_pixelShader.Load(device, "./Data/Shader/HLSL/PrimitiveRenderer/PrimitiveRendererPS.cso");

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
}

// 描画
void PrimitiveRenderer::Draw(const RenderInfo& info)
{
	_renderInfos.push_back(info);
}

// 描画実行
void PrimitiveRenderer::Render(ID3D11DeviceContext* dc, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	dc->RSGetViewports(&numViewports, &viewport);

	// シェーダー設定
	dc->VSSetShader(vertexShader.Get(), nullptr, 0);
	dc->IASetInputLayout(inputLayout.Get());

	Microsoft::WRL::ComPtr<ID3D11Buffer> cachePSBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> cacheVSBuffer;
	dc->VSGetConstantBuffers(CBIndex, 1, cacheVSBuffer.ReleaseAndGetAddressOf());
	dc->PSGetConstantBuffers(CBIndex, 1, cachePSBuffer.ReleaseAndGetAddressOf());

	// 描画
	for (auto& info : _renderInfos)
	{
		// ピクセルシェーダ設定
		if (info.pixelShader)
			dc->PSSetShader(info.pixelShader->Get(), nullptr, 0);
		else
			dc->PSSetShader(_pixelShader.Get(), nullptr, 0);

		// テクスチャ設定
		if (info.colorSRV)
			dc->PSSetShaderResources(ColorSRVIndex, 1, info.colorSRV);
		if (info.parameterSRV)
			dc->PSSetShaderResources(ParameterSRVIndex, 1, info.parameterSRV);

		// 定数バッファ設定
		if (info.constantBuffer)
		{
			dc->VSSetConstantBuffers(CBIndex, 1, info.constantBuffer->GetAddressOf());
			dc->PSSetConstantBuffers(CBIndex, 1, info.constantBuffer->GetAddressOf());
		}

		// 頂点バッファ設定
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		dc->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

		// 描画
		UINT totalVertexCount = static_cast<UINT>(info.vertices.size());
		UINT start = 0;
		UINT count = (totalVertexCount < VertexCapacity) ? totalVertexCount : VertexCapacity;
		while (start < totalVertexCount)
		{
			D3D11_MAPPED_SUBRESOURCE mappedSubresource;
			HRESULT hr = dc->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

			memcpy(mappedSubresource.pData, &info.vertices[start], sizeof(Vertex) * count);

			dc->Unmap(vertexBuffer.Get(), 0);

			dc->Draw(count, 0);

			start += count;
			if ((start + count) > totalVertexCount)
			{
				count = totalVertexCount - start;
			}
		}
	}
	// 描画情報クリア
	_renderInfos.clear();

	// シェーダ解除
	dc->VSSetShader(NULL, 0, 0);
	dc->PSSetShader(NULL, 0, 0);
	// 定数バッファ設定を戻す
	dc->VSSetConstantBuffers(CBIndex, 1, cacheVSBuffer.GetAddressOf());
	dc->PSSetConstantBuffers(CBIndex, 1, cachePSBuffer.GetAddressOf());
}
