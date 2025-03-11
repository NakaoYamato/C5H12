#include "GBuffer.h"

#include <vector>
#include <imgui.h>

#include "../HRTrace.h"
#include "../../ResourceManager/GpuResourceManager.h"
#include "../../Converter/ToString.h"

GBuffer::GBuffer(ID3D11Device* device, UINT width, UINT height)
{
	HRESULT hr = S_OK;

	// DXGI_FORMAT_R8G8B8A8_UNORM : DXGI_FORMAT_R16G16B16A16_FLOAT : DXGI_FORMAT_R32G32B32A32_FLOAT
	auto CreateRTVandSRV = [&](
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& rtv,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srv,
		DXGI_FORMAT format)
		{
			Microsoft::WRL::ComPtr<ID3D11Texture2D> render_target_buffer;

			D3D11_TEXTURE2D_DESC texture2d_desc = {};
			texture2d_desc.Width = width;
			texture2d_desc.Height = height;
			texture2d_desc.MipLevels = 1;
			texture2d_desc.ArraySize = 1;
			texture2d_desc.Format = format;
			texture2d_desc.SampleDesc.Count = 1;
			texture2d_desc.SampleDesc.Quality = 0;
			texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
			texture2d_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			texture2d_desc.CPUAccessFlags = 0;
			texture2d_desc.MiscFlags = 0;
			hr = device->CreateTexture2D(&texture2d_desc, 0, render_target_buffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

			D3D11_RENDER_TARGET_VIEW_DESC render_target_view_desc = {};
			render_target_view_desc.Format = texture2d_desc.Format;
			render_target_view_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			hr = device->CreateRenderTargetView(render_target_buffer.Get(), &render_target_view_desc, rtv.ReleaseAndGetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

			D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc = {};
			shader_resource_view_desc.Format = texture2d_desc.Format;
			shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			shader_resource_view_desc.Texture2D.MipLevels = 1;
			hr = device->CreateShaderResourceView(render_target_buffer.Get(), &shader_resource_view_desc, srv.ReleaseAndGetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		};

	// RTVとSRVの作成
	// フォーマットを変えることで画面の質が良くなるはずだが重くなるので下げている
	CreateRTVandSRV(rtvs_[GBufferSRVType::BaseColorSRV],		renderTargetSRVs_[GBufferSRVType::BaseColorSRV],		DXGI_FORMAT_R16G16B16A16_FLOAT);
	CreateRTVandSRV(rtvs_[GBufferSRVType::WorldNormalSRV],		renderTargetSRVs_[GBufferSRVType::WorldNormalSRV],		DXGI_FORMAT_R16G16B16A16_FLOAT);
	CreateRTVandSRV(rtvs_[GBufferSRVType::EmissiveSRV],			renderTargetSRVs_[GBufferSRVType::EmissiveSRV],			DXGI_FORMAT_R16G16B16A16_FLOAT);
	CreateRTVandSRV(rtvs_[GBufferSRVType::ParameterSRV],		renderTargetSRVs_[GBufferSRVType::ParameterSRV],		DXGI_FORMAT_R16G16B16A16_FLOAT);

	Microsoft::WRL::ComPtr<ID3D11Texture2D> depth_stencil_buffer;
	D3D11_TEXTURE2D_DESC texture2d_desc = {};
	texture2d_desc.Width = width;
	texture2d_desc.Height = height;
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize = 1;
	texture2d_desc.Format = DXGI_FORMAT_R32_TYPELESS; //  DXGI_FORMAT_R24G8_TYPELESS : DXGI_FORMAT_R32_TYPELESS
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.SampleDesc.Quality = 0;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texture2d_desc.CPUAccessFlags = 0;
	texture2d_desc.MiscFlags = 0;
	hr = device->CreateTexture2D(&texture2d_desc, 0, depth_stencil_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc = {};
	depth_stencil_view_desc.Format = DXGI_FORMAT_D32_FLOAT; // DXGI_FORMAT_D24_UNORM_S8_UINT : DXGI_FORMAT_D32_FLOAT
	depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depth_stencil_view_desc.Flags = 0;
	hr = device->CreateDepthStencilView(depth_stencil_buffer.Get(), &depth_stencil_view_desc, &dsv_);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc = {};
	shader_resource_view_desc.Format = DXGI_FORMAT_R32_FLOAT; // DXGI_FORMAT_R24_UNORM_X8_TYPELESS : DXGI_FORMAT_R32_FLOAT
	shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shader_resource_view_desc.Texture2D.MipLevels = 1;
	hr = device->CreateShaderResourceView(depth_stencil_buffer.Get(), &shader_resource_view_desc, &depthStencilSRV_);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	for (UINT buffer_index = 0; buffer_index < bufferCount_; ++buffer_index)
	{
		viewports_[buffer_index].Width = static_cast<float>(width);
		viewports_[buffer_index].Height = static_cast<float>(height);
		viewports_[buffer_index].MinDepth = 0.0f;
		viewports_[buffer_index].MaxDepth = 1.0f;
		viewports_[buffer_index].TopLeftX = 0.0f;
		viewports_[buffer_index].TopLeftY = 0.0f;
	}

	textureSize_.x = static_cast<float>(width);
	textureSize_.y = static_cast<float>(height);
	fullscreenQuad_ = std::make_unique<Sprite>(device,
		L"",
		"./Data/Shader/FullscreenQuadVS.cso",
		"./Data/Shader/DeferredRenderingPS.cso");
}

GBuffer::~GBuffer()
{
}

void GBuffer::Clear(ID3D11DeviceContext* immediateContext, const Vector4& color, const float& depth) const
{
	float c[4]{ color.x, color.y, color.z, color.w };
	for (UINT buffer_index = 0; buffer_index < bufferCount_; ++buffer_index)
	{
		immediateContext->ClearRenderTargetView(rtvs_[buffer_index].Get(), c);
	}
	immediateContext->ClearDepthStencilView(dsv_.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, 0);
}

void GBuffer::Activate(ID3D11DeviceContext* immediateContext)
{
	viewportCount_ = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
	immediateContext->RSGetViewports(&viewportCount_, cachedViewports_);
	immediateContext->OMGetRenderTargets(bufferCount_, cachedRTVs_, &cachedDSV_);

	immediateContext->RSSetViewports(bufferCount_, viewports_);
	std::vector<ID3D11RenderTargetView*> tempRTVs;
	for (UINT i = 0; i < bufferCount_; ++i)
	{
		tempRTVs.push_back(rtvs_[i].Get());
	}
	immediateContext->OMSetRenderTargets(bufferCount_, tempRTVs.data(), dsv_.Get());
}

void GBuffer::ClearAndActivate(ID3D11DeviceContext* immediateContext, const Vector4& color, const float& depth)
{
	Clear(immediateContext, color, depth);
	Activate(immediateContext);
}

void GBuffer::Deactivate(ID3D11DeviceContext* immediateContext)
{
#if 0
	// Note that unlike some other resource methods in Direct3D, all currently bound render targets will be unbound by calling OMSetRenderTargets(0, nullptr, nullptr);.
	immediateContext->OMSetRenderTargets(0, NULL, NULL);
#endif
	immediateContext->RSSetViewports(viewportCount_, cachedViewports_);
	immediateContext->OMSetRenderTargets(bufferCount_, cachedRTVs_, cachedDSV_);
	for (UINT buffer_index = 0; buffer_index < bufferCount_; ++buffer_index)
	{
		if (cachedRTVs_[buffer_index])
		{
			cachedRTVs_[buffer_index]->Release();
		}
	}
	if (cachedDSV_)
	{
		cachedDSV_->Release();
	}
}

void GBuffer::DrawGui()
{
#if USE_IMGUI
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu(u8"GBuffer"))
		{
			ImGui::Checkbox(u8"Active", &isActive_);
			ImGui::Checkbox(u8"SRV", &drawGui_);

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	if (drawGui_)
	{
		if (ImGui::Begin("GBuffer"))
		{
			static float sizeFactor = 0.3f;
			ImGui::DragFloat("TextureSize", &sizeFactor);
			Vector2 size = textureSize_ * sizeFactor;
			for (size_t i = 0; i < static_cast<size_t>(GBufferSRVType::GBufferSRVTypeMAX); ++i)
			{
				ImGui::Text(ToString<GBufferSRVType>(i).c_str());
				ImGui::Image(renderTargetSRVs_[i].Get(),
					{ size.x ,size.y });
			}

			ImGui::Text(u8"Depth");
			ImGui::Image(depthStencilSRV_.Get(),
				{ size.x ,size.y });
		}
		ImGui::End();
	}
#endif
}

// GBufferのデータを書き出し
void GBuffer::Blit(ID3D11DeviceContext* immediateContext)
{
	std::vector<ID3D11ShaderResourceView*> tempSRVs;
	for (UINT i = 0; i < bufferCount_; ++i)
	{
		tempSRVs.push_back(GetRenderTargetSRV(i).Get());
	}
	tempSRVs.push_back(depthStencilSRV_.Get());
	// 描画処理
	fullscreenQuad_->Blit(immediateContext,
		tempSRVs.data(),
		0, bufferCount_ + 1);
}
