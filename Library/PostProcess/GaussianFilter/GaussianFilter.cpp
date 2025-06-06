#include "GaussianFilter.h"
#include <imgui.h>

#include "../../Graphics/GpuResourceManager.h"
#include "../../HRTrace.h"

GaussianFilter::GaussianFilter(ID3D11Device* device, uint32_t width, uint32_t height) :
	PostProcessBase(device, width, height, "./Data/Shader/UpsamplingPS.cso", sizeof(Constants))
{
	// ピクセルシェーダ作成
	(void)GpuResourceManager::CreatePsFromCso(device, "./Data/Shader/DownsamplingPS.cso",
		_pixelShaders[PIXEL_SHADER_TYPE::DOWNSAMPLING_PS].ReleaseAndGetAddressOf());
	(void)GpuResourceManager::CreatePsFromCso(device, "./Data/Shader/HorizontalFilterPS.cso",
		_pixelShaders[PIXEL_SHADER_TYPE::HORIZONTAL_PS].ReleaseAndGetAddressOf());
	(void)GpuResourceManager::CreatePsFromCso(device, "./Data/Shader/VerticalFilterPS.cso",
		_pixelShaders[PIXEL_SHADER_TYPE::VERTICAL_PS].ReleaseAndGetAddressOf());

	for (size_t i = 0; i < DownSampledCount; ++i)
	{
		_pingPongFrameBuffer[i][0] = std::make_unique<FrameBuffer>(device, width >> i, height >> i, false);
		_pingPongFrameBuffer[i][1] = std::make_unique<FrameBuffer>(device, width >> i, height >> i, false);
	}
	_frameBuffer = std::make_unique<FrameBuffer>(device, width, height, false);

	{
		D3D11_SAMPLER_DESC sampler_desc{};
		sampler_desc.MipLODBias = 0;
		sampler_desc.MaxAnisotropy = 16;
		sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		sampler_desc.MinLOD = 0;
		sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
		sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		sampler_desc.BorderColor[0] = 0;
		sampler_desc.BorderColor[1] = 0;
		sampler_desc.BorderColor[2] = 0;
		sampler_desc.BorderColor[3] = 0;
		HRESULT hr = device->CreateSamplerState(&sampler_desc, _samplerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		D3D11_RASTERIZER_DESC rasterizer_desc{};
		rasterizer_desc.FillMode = D3D11_FILL_SOLID;
		rasterizer_desc.CullMode = D3D11_CULL_BACK;
		rasterizer_desc.FrontCounterClockwise = FALSE;
		rasterizer_desc.DepthBias = 0;
		rasterizer_desc.DepthBiasClamp = 0;
		rasterizer_desc.SlopeScaledDepthBias = 0;
		rasterizer_desc.DepthClipEnable = TRUE;
		rasterizer_desc.ScissorEnable = FALSE;
		rasterizer_desc.MultisampleEnable = FALSE;
		rasterizer_desc.AntialiasedLineEnable = FALSE;
		hr = device->CreateRasterizerState(&rasterizer_desc, _rasterizerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		D3D11_DEPTH_STENCIL_DESC depth_stencil_desc{};
		depth_stencil_desc.DepthEnable = FALSE;
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		hr = device->CreateDepthStencilState(&depth_stencil_desc, _depthStencilState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		D3D11_BLEND_DESC blend_desc{};
		blend_desc.AlphaToCoverageEnable = FALSE;
		blend_desc.IndependentBlendEnable = FALSE;
		blend_desc.RenderTarget[0].BlendEnable = FALSE;
		blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device->CreateBlendState(&blend_desc, _blendState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// 初期値の設定
	_startData = GetCurrentData();
}

void GaussianFilter::Render(ID3D11DeviceContext* immediateContext, 
	ID3D11ShaderResourceView** shaderResourceView, uint32_t startSlot, uint32_t numViews)
{
	ID3D11ShaderResourceView* nullSRV{};
	ID3D11ShaderResourceView* cachedSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT]{};
	immediateContext->PSGetShaderResources(0, DownSampledCount, cachedSRV);
	// 現在設定しているステートを取得
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>  cachedDSS;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>  cachedRS;
	Microsoft::WRL::ComPtr<ID3D11BlendState>  cachedBS;
	FLOAT blendFactor[4];
	UINT sampleMask;
	immediateContext->OMGetDepthStencilState(cachedDSS.GetAddressOf(), 0);
	immediateContext->RSGetState(cachedRS.GetAddressOf());
	immediateContext->OMGetBlendState(cachedBS.GetAddressOf(), blendFactor, &sampleMask);

	// ブルーム用ステート設定
	immediateContext->OMSetDepthStencilState(_depthStencilState.Get(), 0);
	immediateContext->RSSetState(_rasterizerState.Get());
	immediateContext->OMSetBlendState(_blendState.Get(), nullptr, 0xFFFFFFFF);
	immediateContext->PSSetSamplers(8, 1, _samplerState.GetAddressOf());

	// 定数バッファの更新
	UpdateConstantBuffer(immediateContext, _constantBuffer.Get());
	immediateContext->PSSetConstantBuffers(1, 1, _constantBuffer.GetAddressOf());

	// Gaussian blur
	// Efficient Gaussian blur with linear sampling
	// http://rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/
	// Downsampling
	_pingPongFrameBuffer[0][0]->Clear(immediateContext, { 0, 0, 0, 1 });
	_pingPongFrameBuffer[0][0]->Activate(immediateContext);
	// 1番にGlowExtractionが入っている
	_fullscreenQuad->Blit(immediateContext, shaderResourceView, 0, 1,
		_pixelShaders[PIXEL_SHADER_TYPE::DOWNSAMPLING_PS].Get());
	_pingPongFrameBuffer[0][0]->Deactivate(immediateContext);
	immediateContext->PSSetShaderResources(0, 1, &nullSRV);

	// Ping-pong gaussian blur
	_pingPongFrameBuffer[0][1]->Clear(immediateContext, { 0, 0, 0, 1 });
	_pingPongFrameBuffer[0][1]->Activate(immediateContext);
	_fullscreenQuad->Blit(immediateContext, _pingPongFrameBuffer[0][0]->GetColorSRV().GetAddressOf(), 0, 1,
		_pixelShaders[PIXEL_SHADER_TYPE::HORIZONTAL_PS].Get());
	_pingPongFrameBuffer[0][1]->Deactivate(immediateContext);
	immediateContext->PSSetShaderResources(0, 1, &nullSRV);

	_pingPongFrameBuffer[0][0]->Clear(immediateContext, { 0, 0, 0, 1 });
	_pingPongFrameBuffer[0][0]->Activate(immediateContext);
	_fullscreenQuad->Blit(immediateContext, _pingPongFrameBuffer[0][1]->GetColorSRV().GetAddressOf(), 0, 1,
		_pixelShaders[PIXEL_SHADER_TYPE::VERTICAL_PS].Get());
	_pingPongFrameBuffer[0][0]->Deactivate(immediateContext);
	immediateContext->PSSetShaderResources(0, 1, &nullSRV);

	for (size_t downsampledIndex = 1; downsampledIndex < DownSampledCount; ++downsampledIndex)
	{
		// Downsampling
		_pingPongFrameBuffer[downsampledIndex][0]->Clear(immediateContext, { 0, 0, 0, 1 });
		_pingPongFrameBuffer[downsampledIndex][0]->Activate(immediateContext);
		_fullscreenQuad->Blit(immediateContext, _pingPongFrameBuffer[downsampledIndex - 1][0]->GetColorSRV().GetAddressOf(), 0, 1,
			_pixelShaders[PIXEL_SHADER_TYPE::DOWNSAMPLING_PS].Get());
		_pingPongFrameBuffer[downsampledIndex][0]->Deactivate(immediateContext);
		immediateContext->PSSetShaderResources(0, 1, &nullSRV);

		// Ping-pong gaussian blur
		_pingPongFrameBuffer[downsampledIndex][1]->Activate(immediateContext);
		_pingPongFrameBuffer[downsampledIndex][1]->Clear(immediateContext, { 0, 0, 0, 1 });
		_fullscreenQuad->Blit(immediateContext, _pingPongFrameBuffer[downsampledIndex][0]->GetColorSRV().GetAddressOf(), 0, 1,
			_pixelShaders[PIXEL_SHADER_TYPE::HORIZONTAL_PS].Get());
		_pingPongFrameBuffer[downsampledIndex][1]->Deactivate(immediateContext);
		immediateContext->PSSetShaderResources(0, 1, &nullSRV);

		_pingPongFrameBuffer[downsampledIndex][0]->Clear(immediateContext, { 0, 0, 0, 1 });
		_pingPongFrameBuffer[downsampledIndex][0]->Activate(immediateContext);
		_fullscreenQuad->Blit(immediateContext, _pingPongFrameBuffer[downsampledIndex][1]->GetColorSRV().GetAddressOf(), 0, 1,
			_pixelShaders[PIXEL_SHADER_TYPE::VERTICAL_PS].Get());
		_pingPongFrameBuffer[downsampledIndex][0]->Deactivate(immediateContext);
		immediateContext->PSSetShaderResources(0, 1, &nullSRV);
	}

	// Upsampling
	std::vector<ID3D11ShaderResourceView*> srvs;
	for (size_t downsampledIndex = 0; downsampledIndex < DownSampledCount; ++downsampledIndex)
	{
		srvs.push_back(_pingPongFrameBuffer[downsampledIndex][0]->GetColorSRV().Get());
	}
	PostProcessBase::Render(immediateContext, srvs.data(), 0, DownSampledCount);
	immediateContext->PSSetShaderResources(0, 1, &nullSRV);

	// 変更したステートを元に戻す
	immediateContext->OMSetDepthStencilState(cachedDSS.Get(), 0);
	immediateContext->RSSetState(cachedRS.Get());
	immediateContext->OMSetBlendState(cachedBS.Get(), blendFactor, sampleMask);

	immediateContext->PSSetShaderResources(0, DownSampledCount, cachedSRV);
	for (ID3D11ShaderResourceView* cached_shader_resource_view : cachedSRV)
	{
		if (cached_shader_resource_view) cached_shader_resource_view->Release();
	}
}

// デバッグGui描画
void GaussianFilter::DrawGui()
{
	ImGui::SliderFloat("intensity", &_data.intensity, +0.0f, +5.0f);
}

// 定数バッファの更新
void GaussianFilter::UpdateConstantBuffer(ID3D11DeviceContext* immediateContext, 
	ID3D11Buffer* constantBuffer)
{
	immediateContext->UpdateSubresource(constantBuffer, 0, 0, &_data, 0, 0);
}

std::unordered_map<std::string, float> GaussianFilter::GetCurrentData()
{
	std::unordered_map<std::string, float> parameter;
	parameter["intensity"] = _data.intensity;
	return parameter;
}

void GaussianFilter::SetData(std::unordered_map<std::string, float>& parameter)
{
	{
		auto iter = parameter.find("intensity");
		if (iter != parameter.end())
			_data.intensity = (*iter).second;
	}
}
