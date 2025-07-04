#include "FrameBuffer.h"
#include "../HRTrace.h"
#include "../../Graphics/GpuResourceManager.h"

FrameBuffer::FrameBuffer(ID3D11Device* device, 
	uint32_t width,
	uint32_t height, 
	bool hasDepthstencil,
	std::vector<DXGI_FORMAT> formats)
{
    _width = width;
    _height = height;

	HRESULT hr{ S_OK };
	D3D11_TEXTURE2D_DESC texture2dDesc{};
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc{};
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{};
	// フォーマットの数だけレンダリングターゲットビューを作成
	_rtvs.resize(formats.size());
	_srvs.resize(formats.size());
	for (size_t i = 0; i < formats.size(); ++i)
	{
		auto& rtv = _rtvs[i];
		auto& srv = _srvs[i];

		Microsoft::WRL::ComPtr<ID3D11Texture2D> renderTargetBuffer;
		texture2dDesc.Width = width;
		texture2dDesc.Height = height;
		texture2dDesc.MipLevels = 1;
		texture2dDesc.ArraySize = 1;
		texture2dDesc.Format = formats[i];
		texture2dDesc.SampleDesc.Count = 1;
		texture2dDesc.SampleDesc.Quality = 0;
		texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
		texture2dDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		texture2dDesc.CPUAccessFlags = 0;
		texture2dDesc.MiscFlags = 0;
		hr = device->CreateTexture2D(&texture2dDesc, 0, renderTargetBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		renderTargetViewDesc.Format = texture2dDesc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		hr = device->CreateRenderTargetView(renderTargetBuffer.Get(), &renderTargetViewDesc,
			rtv.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		shaderResourceViewDesc.Format = texture2dDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;
		hr = device->CreateShaderResourceView(renderTargetBuffer.Get(), &shaderResourceViewDesc,
			srv.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	if (hasDepthstencil)
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
		texture2dDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		texture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		hr = device->CreateTexture2D(&texture2dDesc, 0, depthStencilBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;
		depthStencilViewDesc.Flags = 0;
		hr = device->CreateDepthStencilView(depthStencilBuffer.Get(), &depthStencilViewDesc,
			_dsv.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;
		hr = device->CreateShaderResourceView(depthStencilBuffer.Get(), &shaderResourceViewDesc,
			_dsvSRV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	_viewport.Width = static_cast<float>(width);
	_viewport.Height = static_cast<float>(height);
	_viewport.MinDepth = 0.0f;
	_viewport.MaxDepth = 1.0f;
	_viewport.TopLeftX = 0.0f;
	_viewport.TopLeftY = 0.0f;

	//ID3D11Bufferに設定するデータ
	(void)GpuResourceManager::CreateConstantBuffer(device,
		sizeof(Constants),
		_constantBuffer.ReleaseAndGetAddressOf());
}
// RTVとDSVに設定されているSRVを指定色でクリア
void FrameBuffer::Clear(ID3D11DeviceContext* immediateContext,
	const Vector4& color, const float& depth) const
{
	float c[4]{ color.x, color.y, color.z, color.w };
	for (size_t i = 0; i < _rtvs.size(); ++i)
	{
		immediateContext->ClearRenderTargetView(_rtvs[i].Get(), c);
	}
	if (_dsv)
		immediateContext->ClearDepthStencilView(_dsv.Get(), D3D11_CLEAR_DEPTH, depth, 0);
}
// すべてのRTVとDSVをアクティブにする
void FrameBuffer::Activate(ID3D11DeviceContext* immediateContext)
{
	_viewportCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
	immediateContext->RSGetViewports(&_viewportCount, _cachedViewports);
	immediateContext->OMGetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, _cachedRTVs,
		_cachedDSV.ReleaseAndGetAddressOf());

	immediateContext->RSSetViewports(1, &_viewport);
	std::vector<ID3D11RenderTargetView*> tempRTVs;
	for (size_t i = 0; i < _rtvs.size(); ++i)
	{
		tempRTVs.push_back(_rtvs[i].Get());
	}
	immediateContext->OMSetRenderTargets((UINT)_rtvs.size(), tempRTVs.data(), _dsv.Get());
}
// すべてのRTVとDSVをアクティブにし、指定色でクリアする
void FrameBuffer::ClearAndActivate(ID3D11DeviceContext* immediateContext,
	const Vector4& color, const float& depth)
{
	Clear(immediateContext, color, depth);
	Activate(immediateContext);
}
// すべてのRTVとDSVを非アクティブにする
void FrameBuffer::Deactivate(ID3D11DeviceContext* immediateContext)
{
	immediateContext->RSSetViewports(_viewportCount, _cachedViewports);
	immediateContext->OMSetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, _cachedRTVs,
		_cachedDSV.Get());
	for (int i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
	{
        if (_cachedRTVs[i])
			_cachedRTVs[i]->Release();
	}
}
void FrameBuffer::Deactivate(ID3D11DeviceContext* immediateContext,
	const Vector4& high_luminance_color)
{
	Constants data{};
	data.color = high_luminance_color;
	immediateContext->UpdateSubresource(_constantBuffer.Get(), 0, 0, &data, 0, 0);
	immediateContext->PSSetConstantBuffers(0, 1, _constantBuffer.GetAddressOf());
	Deactivate(immediateContext);
}
// 指定のRTVとDSVに設定されているSRVを指定色でクリア
void FrameBuffer::Clear(size_t index, ID3D11DeviceContext* immediateContext, const Vector4& color, const float& depth) const
{
	float c[4]{ color.x, color.y, color.z, color.w };
	immediateContext->ClearRenderTargetView(_rtvs[index].Get(), c);
	if (_dsv)
		immediateContext->ClearDepthStencilView(_dsv.Get(), D3D11_CLEAR_DEPTH, depth, 0);
}
// 指定のRTVとDSVをアクティブにする
void FrameBuffer::Activate(size_t index, ID3D11DeviceContext* immediateContext)
{
	_viewportCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
	immediateContext->RSGetViewports(&_viewportCount, _cachedViewports);
	immediateContext->OMGetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, _cachedRTVs,
		_cachedDSV.ReleaseAndGetAddressOf());

	immediateContext->RSSetViewports(1, &_viewport);
	immediateContext->OMSetRenderTargets(1, _rtvs[index].GetAddressOf(), _dsv.Get());
}
// 指定のRTVとDSVをアクティブにし、指定色でクリアする
void FrameBuffer::ClearAndActivate(size_t index, ID3D11DeviceContext* immediateContext, const Vector4& color, const float& depth)
{
	Clear(index, immediateContext, color, depth);
	Activate(index, immediateContext);
}
