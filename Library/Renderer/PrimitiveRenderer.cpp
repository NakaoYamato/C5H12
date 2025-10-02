#include "PrimitiveRenderer.h"

#include <memory>

#include "../../Library/HRTrace.h"
#include "../../Library/Graphics/GpuResourceManager.h"

#include "../../Library/Shader/Primitive/Simple/SimplePrimitiveShader.h"
#include "../../Library/Shader/Primitive/Locus/LocusPrimitiveShader.h"

// 初期化処理
void PrimitiveRenderer::Initialize(ID3D11Device* device)
{
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	// シェーダ読み込み
	_shaders["Simple"] = std::make_unique<SimplePrimitiveShader>(device, inputElementDesc, static_cast<UINT>(_countof(inputElementDesc)));
	_shaders["Locus"] = std::make_unique<LocusPrimitiveShader>(device, inputElementDesc, static_cast<UINT>(_countof(inputElementDesc)));

	// 頂点バッファ
	D3D11_BUFFER_DESC desc{};
	desc.ByteWidth = sizeof(Vertex) * VertexCapacity;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	HRESULT hr = device->CreateBuffer(&desc, nullptr, _vertexBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	// 定数バッファ
	_constantBuffer.Create(device, sizeof(CbPrimitive));
}

// 描画
void PrimitiveRenderer::Draw(const RenderInfo& info)
{
	_renderInfos.push_back(info);
}

// 描画実行
void PrimitiveRenderer::Render(RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	dc->RSGetViewports(&numViewports, &viewport);

	Microsoft::WRL::ComPtr<ID3D11Buffer> cachePSBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> cacheVSBuffer;
	dc->VSGetConstantBuffers(CBIndex, 1, cacheVSBuffer.ReleaseAndGetAddressOf());
	dc->PSGetConstantBuffers(CBIndex, 1, cachePSBuffer.ReleaseAndGetAddressOf());

	// 定数バッファ設定
	dc->VSSetConstantBuffers(CBIndex, 1, _constantBuffer.GetAddressOf());
	dc->PSSetConstantBuffers(CBIndex, 1, _constantBuffer.GetAddressOf());

	// 描画
	for (auto& info : _renderInfos)
	{
		auto* shader = _shaders["Simple"].get();
		if (_shaders.find(info.material->GetShaderName()) != _shaders.end())
			shader = _shaders[info.material->GetShaderName()].get();

		// シェーダ開始
		shader->Begin(rc);

		// 頂点バッファ設定
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		dc->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);

		// 定数バッファ更新
		CbPrimitive cb{};
		cb.vertexCount = static_cast<UINT>(info.vertices.size());
		cb.viewportSize = Vector2(
			static_cast<float>(viewport.Width),
			static_cast<float>(viewport.Height));
		_constantBuffer.Update(dc, &cb);

		// 描画
		UINT totalVertexCount = static_cast<UINT>(info.vertices.size());
		UINT start = 0;
		UINT count = (totalVertexCount < VertexCapacity) ? totalVertexCount : VertexCapacity;
		while (start < totalVertexCount)
		{
			D3D11_MAPPED_SUBRESOURCE mappedSubresource;
			HRESULT hr = dc->Map(_vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

			memcpy(mappedSubresource.pData, &info.vertices[start], sizeof(Vertex) * count);

			dc->Unmap(_vertexBuffer.Get(), 0);

			// シェーダ更新
			shader->Update(rc, info.material);
			// 描画
			dc->Draw(count, 0);

			start += count;
			if ((start + count) > totalVertexCount)
			{
				count = totalVertexCount - start;
			}
		}

		// シェーダ終了
		shader->End(rc);
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
