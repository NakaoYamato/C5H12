#include "FrameBuffer.h"
#include "../HRTrace.h"
#include "../../Graphics/GpuResourceManager.h"

FrameBuffer::FrameBuffer(ID3D11Device* device, uint32_t width, uint32_t height, bool hasDepthstencil)
{
	HRESULT hr{ S_OK };
	Microsoft::WRL::ComPtr<ID3D11Texture2D> renderTargetBuffer;
	D3D11_TEXTURE2D_DESC texture2dDesc{};
	texture2dDesc.Width = width;
	texture2dDesc.Height = height;
	texture2dDesc.MipLevels = 1;
	texture2dDesc.ArraySize = 1;
	texture2dDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	texture2dDesc.SampleDesc.Count = 1;
	texture2dDesc.SampleDesc.Quality = 0;
	texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
	texture2dDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texture2dDesc.CPUAccessFlags = 0;
	texture2dDesc.MiscFlags = 0;
	hr = device->CreateTexture2D(&texture2dDesc, 0, renderTargetBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc{};
	renderTargetViewDesc.Format = texture2dDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	hr = device->CreateRenderTargetView(renderTargetBuffer.Get(), &renderTargetViewDesc,
		_rtv.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{};
	shaderResourceViewDesc.Format = texture2dDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;
	hr = device->CreateShaderResourceView(renderTargetBuffer.Get(), &shaderResourceViewDesc,
		_srvs[0].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

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
			_srvs[1].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	_viewport.Width = static_cast<float>(width);
	_viewport.Height = static_cast<float>(height);
	_viewport.MinDepth = 0.0f;
	_viewport.MaxDepth = 1.0f;
	_viewport.TopLeftX = 0.0f;
	_viewport.TopLeftY = 0.0f;

	//ID3D11Buffer‚ÉÝ’è‚·‚éƒf[ƒ^
	(void)GpuResourceManager::CreateConstantBuffer(device,
		sizeof(Constants),
		_constantBuffer.ReleaseAndGetAddressOf());
}

void FrameBuffer::Clear(ID3D11DeviceContext* immediateContext,
	const Vector4& color, const float& depth) const
{
	float c[4]{ color.x, color.y, color.z, color.w };
	immediateContext->ClearRenderTargetView(_rtv.Get(), c);
	if (_dsv)
		immediateContext->ClearDepthStencilView(_dsv.Get(), D3D11_CLEAR_DEPTH, depth, 0);
}

void FrameBuffer::Activate(ID3D11DeviceContext* immediateContext)
{
	_viewportCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
	immediateContext->RSGetViewports(&_viewportCount, _cachedViewports);
	immediateContext->OMGetRenderTargets(1, _cachedRTV.ReleaseAndGetAddressOf(),
		_cachedDSV.ReleaseAndGetAddressOf());

	immediateContext->RSSetViewports(1, &_viewport);
	immediateContext->OMSetRenderTargets(1, _rtv.GetAddressOf(),
		_dsv.Get());
}

void FrameBuffer::ClearAndActivate(ID3D11DeviceContext* immediateContext,
	const Vector4& color, const float& depth)
{
	Clear(immediateContext, color, depth);
	Activate(immediateContext);
}

void FrameBuffer::Deactivate(ID3D11DeviceContext* immediateContext)
{
	immediateContext->RSSetViewports(_viewportCount, _cachedViewports);
	immediateContext->OMSetRenderTargets(1, _cachedRTV.GetAddressOf(),
		_cachedDSV.Get());
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
