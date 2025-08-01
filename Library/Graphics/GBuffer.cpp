#include "GBuffer.h"

#include <vector>
#include <imgui.h>

#include "../HRTrace.h"
#include "../../Graphics/GpuResourceManager.h"
#include "../../Algorithm/Converter.h"

GBuffer::GBuffer(ID3D11Device* device, UINT width, UINT height) :
	_width(width),
	_height(height)
{
	HRESULT hr = S_OK;

	// DXGI_FORMAT_R8G8B8A8_UNORM : DXGI_FORMAT_R16G16B16A16_FLOAT : DXGI_FORMAT_R32G32B32A32_FLOAT
	auto CreateRTVandSRV = [&](UINT incex, DXGI_FORMAT format)
		{
			Microsoft::WRL::ComPtr<ID3D11Texture2D> rtBuffer;

			D3D11_TEXTURE2D_DESC texture2dDesc = {};
			texture2dDesc.Width = width;
			texture2dDesc.Height = height;
			texture2dDesc.MipLevels = 1;
			texture2dDesc.ArraySize = 1;
			texture2dDesc.Format = format;
			texture2dDesc.SampleDesc.Count = 1;
			texture2dDesc.SampleDesc.Quality = 0;
			texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
			texture2dDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			texture2dDesc.CPUAccessFlags = 0;
			texture2dDesc.MiscFlags = 0;
			hr = device->CreateTexture2D(&texture2dDesc, 0, rtBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

			D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
			rtvDesc.Format = texture2dDesc.Format;
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			hr = device->CreateRenderTargetView(rtBuffer.Get(), &rtvDesc,
				_rtvs[incex].ReleaseAndGetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = texture2dDesc.Format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = 1;
			hr = device->CreateShaderResourceView(rtBuffer.Get(), &srvDesc,
				_renderTargetSRVs[incex].ReleaseAndGetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
		};

	// RTVとSRVの作成
	// フォーマットを変えることで画面の質が良くなるはずだが重くなるので下げている
	for (UINT bufferIndex = 0; bufferIndex < GBUFFER_RTV_COUNT; ++bufferIndex)
	{
		CreateRTVandSRV(bufferIndex, DXGI_FORMAT_R16G16B16A16_FLOAT);
	}

	Microsoft::WRL::ComPtr<ID3D11Texture2D> dsBuffer;
	D3D11_TEXTURE2D_DESC texture2dDesc = {};
	texture2dDesc.Width = width;
	texture2dDesc.Height = height;
	texture2dDesc.MipLevels = 1;
	texture2dDesc.ArraySize = 1;
	texture2dDesc.Format = DXGI_FORMAT_R32_TYPELESS; //  DXGI_FORMAT_R24G8_TYPELESS : DXGI_FORMAT_R32_TYPELESS
	texture2dDesc.SampleDesc.Count = 1;
	texture2dDesc.SampleDesc.Quality = 0;
	texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
	texture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texture2dDesc.CPUAccessFlags = 0;
	texture2dDesc.MiscFlags = 0;
	hr = device->CreateTexture2D(&texture2dDesc, 0, dsBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT; // DXGI_FORMAT_D24_UNORM_S8_UINT : DXGI_FORMAT_D32_FLOAT
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = 0;
	hr = device->CreateDepthStencilView(dsBuffer.Get(), &dsvDesc, &_dsv);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT; // DXGI_FORMAT_R24_UNORM_X8_TYPELESS : DXGI_FORMAT_R32_FLOAT
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	hr = device->CreateShaderResourceView(dsBuffer.Get(), &srvDesc, &_depthStencilSRV);
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	for (UINT bufferIndex = 0; bufferIndex < GBUFFER_RTV_COUNT; ++bufferIndex)
	{
		_viewports[bufferIndex].Width = static_cast<float>(width);
		_viewports[bufferIndex].Height = static_cast<float>(height);
		_viewports[bufferIndex].MinDepth = 0.0f;
		_viewports[bufferIndex].MaxDepth = 1.0f;
		_viewports[bufferIndex].TopLeftX = 0.0f;
		_viewports[bufferIndex].TopLeftY = 0.0f;
	}

	_textureSize.x = static_cast<float>(width);
	_textureSize.y = static_cast<float>(height);

	// ピクセルシェーダーオブジェクトの生成
	GpuResourceManager::CreatePsFromCso(device,
		"./Data/Shader/DeferredRenderingPS.cso",
		_gbufferPS[RenderingType::Phong].ReleaseAndGetAddressOf());
	GpuResourceManager::CreatePsFromCso(device,
		"./Data/Shader/PBRDeferredRenderingPS.cso",
		_gbufferPS[RenderingType::PBR].ReleaseAndGetAddressOf());
	GpuResourceManager::CreatePsFromCso(device,
		"./Data/Shader/SpriteWithDepthPS.cso",
		_depthWritePS.ReleaseAndGetAddressOf());

	_frameBuffer = std::make_unique<FrameBuffer>(device,
		width, height);

	// フォグ定数バッファの作成
	(void)GpuResourceManager::CreateConstantBuffer(device,
		sizeof(CbFog),
		_fogConstantBuffer.ReleaseAndGetAddressOf());

	// SSR用変数初期化
	_ssrFullscreenQuad = std::make_unique<SpriteResource>(device,
		L"",
		"./Data/Shader/FullscreenQuadVS.cso",
		"./Data/Shader/ScreenSpaceReflectionPS.cso");

	// 定数バッファ作成
	(void)GpuResourceManager::CreateConstantBuffer(device,
		sizeof(SSRConstants),
		_ssrConstantBuffer.ReleaseAndGetAddressOf());
}

GBuffer::~GBuffer()
{
}

void GBuffer::Clear(ID3D11DeviceContext* immediateContext, const Vector4& color, const float& depth) const
{
	float c[4]{ color.x, color.y, color.z, color.w };
	for (UINT bufferIndex = 0; bufferIndex < GBUFFER_RTV_COUNT; ++bufferIndex)
	{
		immediateContext->ClearRenderTargetView(_rtvs[bufferIndex].Get(), c);
	}
	immediateContext->ClearDepthStencilView(_dsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, 0);
}

void GBuffer::Activate(ID3D11DeviceContext* immediateContext)
{
	_viewportCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
	immediateContext->RSGetViewports(&_viewportCount, _cachedViewports);
	immediateContext->OMGetRenderTargets(GBUFFER_RTV_COUNT, _cachedRTVs, &_cachedDSV);

	immediateContext->RSSetViewports(GBUFFER_RTV_COUNT, _viewports);
	std::vector<ID3D11RenderTargetView*> tempRTVs;
	for (UINT i = 0; i < GBUFFER_RTV_COUNT; ++i)
	{
		tempRTVs.push_back(_rtvs[i].Get());
	}
	immediateContext->OMSetRenderTargets(GBUFFER_RTV_COUNT, tempRTVs.data(), _dsv.Get());
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
	immediateContext->RSSetViewports(_viewportCount, _cachedViewports);
	immediateContext->OMSetRenderTargets(GBUFFER_RTV_COUNT, _cachedRTVs, _cachedDSV);
	for (UINT bufferIndex = 0; bufferIndex < GBUFFER_RTV_COUNT; ++bufferIndex)
	{
		if (_cachedRTVs[bufferIndex])
		{
			_cachedRTVs[bufferIndex]->Release();
		}
	}
	if (_cachedDSV)
	{
		_cachedDSV->Release();
	}
}

void GBuffer::DrawGui()
{
#if USE_IMGUI
	if (ImGui::Begin("GBuffer"))
	{
		static const char* renderTypeName[] =
		{
			u8"Phong",
			u8"PBR",
		}; 
		ImGui::Combo(u8"描画タイプ", &_renderingType, renderTypeName, _countof(renderTypeName));
		if (ImGui::TreeNode(u8"フォグ設定"))
		{
			ImGui::ColorEdit4(u8"FogColor", &_fogConstants.fogColor.x);
			ImGui::DragFloat(u8"FogNear", &_fogConstants.fogRange.x, 0.1f, 0.1f, 500.0f);
			ImGui::DragFloat(u8"FogFar", &_fogConstants.fogRange.y, 0.1f, 0.1f, 500.0f);
			ImGui::TreePop();
		}

		ImGui::Checkbox(u8"SSR使用", &_useSSR);
		if (_useSSR && ImGui::TreeNode(u8"SSRパラメータ"))
		{
			ImGui::DragFloat(u8"refrectionIntensity", &_ssrConstants.refrectionIntensity, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat(u8"maxDistance", &_ssrConstants.maxDistance, 0.1f);
			ImGui::DragFloat(u8"resolution", &_ssrConstants.resolution, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat(u8"thickness", &_ssrConstants.thickness, 0.01f, 0.0f, 1.0f);
			ImGui::TreePop();
		}
		static float sizeFactor = 0.3f;
		ImGui::DragFloat("TextureSize", &sizeFactor);
		Vector2 size = _textureSize * sizeFactor;
		for (UINT bufferIndex = 0; bufferIndex < GBUFFER_RTV_COUNT; ++bufferIndex)
		{
			ImGui::Image(_renderTargetSRVs[bufferIndex].Get(),
				{ size.x ,size.y });
		}

		ImGui::Text(u8"Depth");
		ImGui::Image(_depthStencilSRV.Get(),
			{ size.x ,size.y });
	}
	ImGui::End();
#endif
}

// GBufferのデータを書き出し
void GBuffer::Blit(TextureRenderer& textureRenderer, ID3D11DeviceContext* immediateContext)
{
	_frameBuffer->ClearAndActivate(immediateContext);
	{
		// フォグ定数バッファの設定
		immediateContext->UpdateSubresource(_fogConstantBuffer.Get(), 0, 0, &_fogConstants, 0, 0);
		immediateContext->PSSetConstantBuffers(FOG_CONSTANT_BUFFER_INDEX, 1, _fogConstantBuffer.GetAddressOf());
		std::vector<ID3D11ShaderResourceView*> tempSRVs;
		for (UINT i = 0; i < GBUFFER_RTV_COUNT; ++i)
		{
			tempSRVs.push_back(GetRenderTargetSRV(i).Get());
		}
		tempSRVs.push_back(_depthStencilSRV.Get());
		// 描画タイプに合わせたライティング処理
		textureRenderer.Blit(
			immediateContext,
			tempSRVs.data(),
			0, GBUFFER_RTV_COUNT + 1,
			_gbufferPS[_renderingType].Get());
	}
	_frameBuffer->Deactivate(immediateContext);

	// 書き出し
	if (_useSSR)
	{
		// 定数バッファの設定
		immediateContext->UpdateSubresource(_ssrConstantBuffer.Get(), 0, 0, &_ssrConstants, 0, 0);
		immediateContext->PSSetConstantBuffers(SSR_CONSTANT_BUFFER_INDEX, 1, _ssrConstantBuffer.GetAddressOf());

		ID3D11ShaderResourceView* srv[] =
		{
			_frameBuffer->GetColorSRV().Get(),
			GetDepthSRV().Get(),
			GetRenderTargetSRV(GBUFFER_NORMAL_MAP_INDEX).Get()
		};
		_ssrFullscreenQuad->Blit(immediateContext,
			srv, 0, _countof(srv));
	}
	else
	{
		ID3D11ShaderResourceView* srv[] =
		{
			_frameBuffer->GetColorSRV().Get(),
			GetDepthSRV().Get()
		};
		textureRenderer.Blit(
			immediateContext,
			srv,
			0, _countof(srv),
			_depthWritePS.Get());
	}
}
