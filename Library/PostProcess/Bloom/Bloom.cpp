#include "Bloom.h"
#include "../../ResourceManager/GpuResourceManager.h"
#include "../../HRTrace.h"

#include <imgui.h>

Bloom::Bloom(ID3D11Device* device, uint32_t width, uint32_t height) :
    PostProcessBase(device, width, height, "./Data/Shader/BloomUpsamplingPS.cso", sizeof(Constants))
{
	// ピクセルシェーダ作成
	(void)GpuResourceManager::CreatePsFromCso(device, "./Data/Shader/GlowExtractionPS.cso",
		bloomPixelShaders[BLOOM_PIXEL_TYPE::GLOW_EXTRACTION_PS].ReleaseAndGetAddressOf());
	(void)GpuResourceManager::CreatePsFromCso(device, "./Data/Shader/BloomDownsamplingPS.cso",
		bloomPixelShaders[BLOOM_PIXEL_TYPE::DOWNSAMPLING_PS].ReleaseAndGetAddressOf());
	(void)GpuResourceManager::CreatePsFromCso(device, "./Data/Shader/BloomHorizontalPS.cso",
		bloomPixelShaders[BLOOM_PIXEL_TYPE::HORIZONTAL_PS].ReleaseAndGetAddressOf());
	(void)GpuResourceManager::CreatePsFromCso(device, "./Data/Shader/BloomVerticalPS.cso",
		bloomPixelShaders[BLOOM_PIXEL_TYPE::VERTICAL_PS].ReleaseAndGetAddressOf());

	glowExtraction = std::make_unique<FrameBuffer>(device, width, height, false);
	for (size_t i = 0; i < DownSampledCount; ++i)
	{
		bloomFrameBuffer[i][0] = std::make_unique<FrameBuffer>(device, width >> i, height >> i, false);
		bloomFrameBuffer[i][1] = std::make_unique<FrameBuffer>(device, width >> i, height >> i, false);
	}
	frameBuffer = std::make_unique<FrameBuffer>(device,	width, height, false);

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
		HRESULT hr = device->CreateSamplerState(&sampler_desc, samplerState.GetAddressOf());
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
		hr = device->CreateRasterizerState(&rasterizer_desc, rasterizerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

		D3D11_DEPTH_STENCIL_DESC depth_stencil_desc{};
		depth_stencil_desc.DepthEnable = FALSE;
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		hr = device->CreateDepthStencilState(&depth_stencil_desc, depthStencilState.GetAddressOf());
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
		hr = device->CreateBlendState(&blend_desc, blendState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}

	// 初期値の設定
	startData = GetCurrentData();
}

// 描画処理
void Bloom::Render(ID3D11DeviceContext* immediateContext, ID3D11ShaderResourceView** shaderResourceView, uint32_t startSlot, uint32_t numViews)
{
	ID3D11ShaderResourceView* null_shader_resource_view{};
	ID3D11ShaderResourceView* cached_shader_resource_views[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT]{};
	immediateContext->PSGetShaderResources(0, DownSampledCount, cached_shader_resource_views);
	// 現在設定しているステートを取得
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>  cached_depth_stencil_state;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>  cached_rasterizer_state;
	Microsoft::WRL::ComPtr<ID3D11BlendState>  cached_blend_state;
	FLOAT blend_factor[4];
	UINT sample_mask;
	immediateContext->OMGetDepthStencilState(cached_depth_stencil_state.GetAddressOf(), 0);
	immediateContext->RSGetState(cached_rasterizer_state.GetAddressOf());
	immediateContext->OMGetBlendState(cached_blend_state.GetAddressOf(), blend_factor, &sample_mask);

	// ブルーム用ステート設定
	immediateContext->OMSetDepthStencilState(depthStencilState.Get(), 0);
	immediateContext->RSSetState(rasterizerState.Get());
	immediateContext->OMSetBlendState(blendState.Get(), nullptr, 0xFFFFFFFF);
	immediateContext->PSSetSamplers(8, 1, samplerState.GetAddressOf());

	// 定数バッファの更新
	UpdateConstantBuffer(immediateContext, constantBuffer.Get());
	immediateContext->PSSetConstantBuffers(1, 1, constantBuffer.GetAddressOf());

	// 高輝度抽出
	glowExtraction->Clear(immediateContext, { 0, 0, 0, 1 });
	glowExtraction->Activate(immediateContext);
	fullscreenQuad->Blit(immediateContext, shaderResourceView, 0, 1,
		bloomPixelShaders[BLOOM_PIXEL_TYPE::GLOW_EXTRACTION_PS].Get());
	glowExtraction->Deactivate(immediateContext);
	immediateContext->PSSetShaderResources(0, 1, &null_shader_resource_view);

	// Gaussian blur
	// Efficient Gaussian blur with linear sampling
	// http://rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/
	// Downsampling
	bloomFrameBuffer[0][0]->Clear(immediateContext, { 0, 0, 0, 1 });
	bloomFrameBuffer[0][0]->Activate(immediateContext);
	fullscreenQuad->Blit(immediateContext, glowExtraction->GetColorSRV().GetAddressOf(), 0, 1,
		bloomPixelShaders[BLOOM_PIXEL_TYPE::DOWNSAMPLING_PS].Get());
	bloomFrameBuffer[0][0]->Deactivate(immediateContext);
	immediateContext->PSSetShaderResources(0, 1, &null_shader_resource_view);

	// Ping-pong gaussian blur
	bloomFrameBuffer[0][1]->Clear(immediateContext, { 0, 0, 0, 1 });
	bloomFrameBuffer[0][1]->Activate(immediateContext);
	fullscreenQuad->Blit(immediateContext, bloomFrameBuffer[0][0]->GetColorSRV().GetAddressOf(), 0, 1,
		bloomPixelShaders[BLOOM_PIXEL_TYPE::HORIZONTAL_PS].Get());
	bloomFrameBuffer[0][1]->Deactivate(immediateContext);
	immediateContext->PSSetShaderResources(0, 1, &null_shader_resource_view);

	bloomFrameBuffer[0][0]->Clear(immediateContext, { 0, 0, 0, 1 });
	bloomFrameBuffer[0][0]->Activate(immediateContext);
	fullscreenQuad->Blit(immediateContext, bloomFrameBuffer[0][1]->GetColorSRV().GetAddressOf(), 0, 1,
		bloomPixelShaders[BLOOM_PIXEL_TYPE::VERTICAL_PS].Get());
	bloomFrameBuffer[0][0]->Deactivate(immediateContext);
	immediateContext->PSSetShaderResources(0, 1, &null_shader_resource_view);

	for (size_t downsampledIndex = 1; downsampledIndex < DownSampledCount; ++downsampledIndex)
	{
		// Downsampling
		bloomFrameBuffer[downsampledIndex][0]->Clear(immediateContext, { 0, 0, 0, 1 });
		bloomFrameBuffer[downsampledIndex][0]->Activate(immediateContext);
		fullscreenQuad->Blit(immediateContext, bloomFrameBuffer[downsampledIndex - 1][0]->GetColorSRV().GetAddressOf(), 0, 1,
			bloomPixelShaders[BLOOM_PIXEL_TYPE::DOWNSAMPLING_PS].Get());
		bloomFrameBuffer[downsampledIndex][0]->Deactivate(immediateContext);
		immediateContext->PSSetShaderResources(0, 1, &null_shader_resource_view);

		// Ping-pong gaussian blur
		bloomFrameBuffer[downsampledIndex][1]->Activate(immediateContext);
		bloomFrameBuffer[downsampledIndex][1]->Clear(immediateContext, { 0, 0, 0, 1 });
		fullscreenQuad->Blit(immediateContext, bloomFrameBuffer[downsampledIndex][0]->GetColorSRV().GetAddressOf(), 0, 1,
			bloomPixelShaders[BLOOM_PIXEL_TYPE::HORIZONTAL_PS].Get());
		bloomFrameBuffer[downsampledIndex][1]->Deactivate(immediateContext);
		immediateContext->PSSetShaderResources(0, 1, &null_shader_resource_view);

		bloomFrameBuffer[downsampledIndex][0]->Clear(immediateContext, { 0, 0, 0, 1 });
		bloomFrameBuffer[downsampledIndex][0]->Activate(immediateContext);
		fullscreenQuad->Blit(immediateContext, bloomFrameBuffer[downsampledIndex][1]->GetColorSRV().GetAddressOf(), 0, 1,
			bloomPixelShaders[BLOOM_PIXEL_TYPE::VERTICAL_PS].Get());
		bloomFrameBuffer[downsampledIndex][0]->Deactivate(immediateContext);
		immediateContext->PSSetShaderResources(0, 1, &null_shader_resource_view);
	}

	// Upsampling
	std::vector<ID3D11ShaderResourceView*> srvs;
	for (size_t downsampledIndex = 0; downsampledIndex < DownSampledCount; ++downsampledIndex)
	{
		srvs.push_back(bloomFrameBuffer[downsampledIndex][0]->GetColorSRV().Get());
	}
	PostProcessBase::Render(immediateContext, srvs.data(), 0, DownSampledCount);
	immediateContext->PSSetShaderResources(0, 1, &null_shader_resource_view);

	// 変更したステートを元に戻す
	immediateContext->OMSetDepthStencilState(cached_depth_stencil_state.Get(), 0);
	immediateContext->RSSetState(cached_rasterizer_state.Get());
	immediateContext->OMSetBlendState(cached_blend_state.Get(), blend_factor, sample_mask);

	immediateContext->PSSetShaderResources(0, DownSampledCount, cached_shader_resource_views);
	for (ID3D11ShaderResourceView* cached_shader_resource_view : cached_shader_resource_views)
	{
		if (cached_shader_resource_view) cached_shader_resource_view->Release();
	}
}

// デバッグGui描画
void Bloom::DrawGui()
{
	if (ImGui::Begin(u8"ブルーム"))
	{
		if (ImGui::Button("reset"))
			ClearData();
		ImGui::SliderFloat("extractionThreshold", &data.extractionThreshold, +0.0f, +1.0f);
		ImGui::SliderFloat("intensity", &data.intensity, +0.0f, +5.0f);
		if (ImGui::TreeNode("Resource"))
		{
			static float textureSize = 512.0f;
			ImGui::DragFloat("TextureSize", &textureSize);
			ImGui::Image(GetColorSRV().Get(),
				{ textureSize ,textureSize });

			ImGui::TreePop();
		}
	}
	ImGui::End();
}

std::unordered_map<std::string, float> Bloom::GetCurrentData()
{
	std::unordered_map<std::string, float> parameter;
	parameter["extractionThreshold"] = data.extractionThreshold;
	parameter["intensity"] = data.intensity;
	return parameter;
}

void Bloom::SetData(std::unordered_map<std::string, float>& parameter)
{
	{
		auto iter = parameter.find("extractionThreshold");
		if (iter != parameter.end())
			data.extractionThreshold = (*iter).second;
	}
	{
		auto iter = parameter.find("intensity");
		if (iter != parameter.end())
			data.intensity = (*iter).second;
	}
}

// 定数バッファの更新
void Bloom::UpdateConstantBuffer(ID3D11DeviceContext* immediateContext, ID3D11Buffer* constantBuffer)
{
	immediateContext->UpdateSubresource(constantBuffer, 0, 0, &data, 0, 0);
}
