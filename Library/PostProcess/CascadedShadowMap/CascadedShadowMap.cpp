#define NOMINMAX

#include "CascadedShadowMap.h"

#include "../../Library/Graphics/Graphics.h"
#include "../../ResourceManager/GpuResourceManager.h"

#include <array>

#include "../../HRTrace.h"

#include <imgui.h>

std::array<DirectX::XMFLOAT4, 8> ExtractFrustumCorners(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
	std::array<DirectX::XMFLOAT4, 8> frustumCorners =
	{
		DirectX::XMFLOAT4{-1.0f, -1.0f, -1.0f, 1.0f},
		DirectX::XMFLOAT4{-1.0f, -1.0f,  1.0f, 1.0f},
		DirectX::XMFLOAT4{-1.0f,  1.0f, -1.0f, 1.0f},
		DirectX::XMFLOAT4{-1.0f,  1.0f,  1.0f, 1.0f},
		DirectX::XMFLOAT4{ 1.0f, -1.0f, -1.0f, 1.0f},
		DirectX::XMFLOAT4{ 1.0f, -1.0f,  1.0f, 1.0f},
		DirectX::XMFLOAT4{ 1.0f,  1.0f, -1.0f, 1.0f},
		DirectX::XMFLOAT4{ 1.0f,  1.0f,  1.0f, 1.0f}
	};

	const DirectX::XMMATRIX INV_VIEW_PROJECTION = DirectX::XMMatrixInverse(nullptr,
		DirectX::XMLoadFloat4x4(&view) * DirectX::XMLoadFloat4x4(&projection));
	for (std::array<DirectX::XMFLOAT4, 8>::reference frustumCorner : frustumCorners)
	{
		DirectX::XMStoreFloat4(&frustumCorner,
			DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat4(&frustumCorner), INV_VIEW_PROJECTION));
	}

	return frustumCorners;
}

CascadedShadowMap::CascadedShadowMap(ID3D11Device* device, UINT width, UINT height, UINT cascadeCount) :
	cascadeCount(cascadeCount),
	cascadedMatrices(cascadeCount),
	cascadedPlaneDistances(cascadeCount + 1)
{
	HRESULT hr = S_OK;

	D3D11_TEXTURE2D_DESC texture2d_desc = {};
	texture2d_desc.Width = width;
	texture2d_desc.Height = height;
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize = cascadeCount;
	texture2d_desc.Format = DXGI_FORMAT_R32_TYPELESS;
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.SampleDesc.Quality = 0;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texture2d_desc.CPUAccessFlags = 0;
	texture2d_desc.MiscFlags = 0;
	hr = device->CreateTexture2D(&texture2d_desc, 0, depthStencilBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc = {};
	depth_stencil_view_desc.Format = DXGI_FORMAT_D32_FLOAT;
	depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	depth_stencil_view_desc.Texture2DArray.FirstArraySlice = 0;
	depth_stencil_view_desc.Texture2DArray.ArraySize = static_cast<UINT>(cascadeCount);
	depth_stencil_view_desc.Texture2DArray.MipSlice = 0;
	depth_stencil_view_desc.Flags = 0;
	hr = device->CreateDepthStencilView(depthStencilBuffer.Get(), &depth_stencil_view_desc, depthStencilView.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc = {};
	shader_resource_view_desc.Format = DXGI_FORMAT_R32_FLOAT; // DXGI_FORMAT_R24_UNORM_X8_TYPELESS : DXGI_FORMAT_R32_FLOAT : DXGI_FORMAT_R16_UNORM
	shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	shader_resource_view_desc.Texture2DArray.ArraySize = static_cast<UINT>(cascadeCount);
	shader_resource_view_desc.Texture2DArray.MipLevels = 1;
	shader_resource_view_desc.Texture2DArray.FirstArraySlice = 0;
	shader_resource_view_desc.Texture2DArray.MostDetailedMip = 0;
	hr = device->CreateShaderResourceView(depthStencilBuffer.Get(), &shader_resource_view_desc, shaderResourceView.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// モデルの影を描画するときに必要な定数バッファ 
	(void)GpuResourceManager::CreateConstantBuffer(
		device,
		sizeof(Constants),
		constantBuffer.GetAddressOf());
	// カスケードシャドウマップのパラメーター定数バッファ
	(void)GpuResourceManager::CreateConstantBuffer(
		device,
		sizeof(ParametricConstants),
		parametricConstantBuffer.GetAddressOf());
}

void CascadedShadowMap::Activate(const RenderContext& rc,
	const UINT& cbSlot)
{
	ID3D11DeviceContext* immediateContext = rc.deviceContext;
	DirectX::XMFLOAT4X4 cameraProjection = rc.camera->projection_;
	DirectX::XMFLOAT4X4 cameraView = rc.camera->view_;
	DirectX::XMFLOAT4 lightDirection = rc.lightDirection;/*TODO : (0,-1,0,0)でエラー*/

	immediateContext->RSGetViewports(&viewportCount, cachedViewports);
	immediateContext->OMGetRenderTargets(1, cachedRenderTargetView.ReleaseAndGetAddressOf(), cachedDepthStencilView.ReleaseAndGetAddressOf());

	// near/far value from perspective projection matrix
	float m33 = cameraProjection._33;
	float m43 = cameraProjection._43;
	float zn = -m43 / m33;
	float zf = (m33 * zn) / (m33 - 1);
	zf = criticalDepthValue > 0 ? std::min(zf, criticalDepthValue) : zf;

	// calculates split plane distances in view space
	for (size_t cascade_index = 0; cascade_index < cascadeCount; ++cascade_index)
	{
		float idc = cascade_index / static_cast<float>(cascadeCount);
		float logarithmic_split_scheme = zn * pow(zf / zn, idc);
		float uniform_split_scheme = zn + (zf - zn) * idc;
		cascadedPlaneDistances.at(cascade_index) = logarithmic_split_scheme * splitSchemeWeight + uniform_split_scheme * (1 - splitSchemeWeight);
	}
	// make sure border values are accurate
	cascadedPlaneDistances.at(0) = zn;
	cascadedPlaneDistances.at(cascadeCount) = zf;

	for (size_t cascade_index = 0; cascade_index < cascadeCount; ++cascade_index)
	{
		float near_plane = fitToCascade ? cascadedPlaneDistances.at(cascade_index) : zn;
		float far_plane = cascadedPlaneDistances.at(cascade_index + 1);

		DirectX::XMFLOAT4X4 cascaded_projection = cameraProjection;
		cascaded_projection._33 = far_plane / (far_plane - near_plane);
		cascaded_projection._43 = -near_plane * far_plane / (far_plane - near_plane);

		std::array<DirectX::XMFLOAT4, 8> corners = ExtractFrustumCorners(cameraView, cascaded_projection);

		DirectX::XMFLOAT4 center = { 0, 0, 0, 1 };
		for (DirectX::XMFLOAT4 corner : corners)
		{
			center.x += corner.x;
			center.y += corner.y;
			center.z += corner.z;
		}
		center.x /= corners.size();
		center.y /= corners.size();
		center.z /= corners.size();

		DirectX::XMMATRIX V;
		V = DirectX::XMMatrixLookAtLH(
			DirectX::XMVectorSet(center.x - lightDirection.x, center.y - lightDirection.y, center.z - lightDirection.z, 1.0f),
			DirectX::XMVectorSet(center.x, center.y, center.z, 1.0f),
			DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

		float min_x = std::numeric_limits<float>::max();
		float max_x = std::numeric_limits<float>::lowest();
		float min_y = std::numeric_limits<float>::max();
		float max_y = std::numeric_limits<float>::lowest();
		float min_z = std::numeric_limits<float>::max();
		float max_z = std::numeric_limits<float>::lowest();
		for (DirectX::XMFLOAT4 corner : corners)
		{
			DirectX::XMStoreFloat4(&corner, DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat4(&corner), V));
			min_x = std::min(min_x, corner.x);
			max_x = std::max(max_x, corner.x);
			min_y = std::min(min_y, corner.y);
			max_y = std::max(max_y, corner.y);
			min_z = std::min(min_z, corner.z);
			max_z = std::max(max_z, corner.z);
		}

#if 1
		zMult = std::max<float>(1.0f, zMult);
		if (min_z < 0)
		{
			min_z *= zMult;
		}
		else
		{
			min_z /= zMult;
		}
		if (max_z < 0)
		{
			max_z /= zMult;
		}
		else
		{
			max_z *= zMult;
		}
#endif

		DirectX::XMMATRIX P = DirectX::XMMatrixOrthographicOffCenterLH(min_x, max_x, min_y, max_y, min_z, max_z);
		DirectX::XMStoreFloat4x4(&cascadedMatrices.at(cascade_index), V * P);
	}

	Constants data;
	data.cascadedMatrices[0] = cascadedMatrices.at(0);
	data.cascadedMatrices[1] = cascadedMatrices.at(1);
	data.cascadedMatrices[2] = cascadedMatrices.at(2);
	data.cascadedMatrices[3] = cascadedMatrices.at(3);

	data.cascadedPlaneDistances[0] = cascadedPlaneDistances.at(1);
	data.cascadedPlaneDistances[1] = cascadedPlaneDistances.at(2);
	data.cascadedPlaneDistances[2] = cascadedPlaneDistances.at(3);
	data.cascadedPlaneDistances[3] = cascadedPlaneDistances.at(4);

	immediateContext->UpdateSubresource(constantBuffer.Get(), 0, 0, &data, 0, 0);
	immediateContext->VSSetConstantBuffers(cbSlot, 1, constantBuffer.GetAddressOf());
	immediateContext->PSSetConstantBuffers(cbSlot, 1, constantBuffer.GetAddressOf());

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> null_render_target_view;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> null_depth_stencil_view;
	immediateContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1, 0);
	immediateContext->OMSetRenderTargets(1, null_render_target_view.GetAddressOf(), depthStencilView.Get());
	immediateContext->RSSetViewports(1, &viewport);
}

void CascadedShadowMap::ClearAndActivate(const RenderContext& rc,
	const UINT& cbSlot)
{
	Clear(rc.deviceContext);
	Activate(rc, cbSlot);
}

void CascadedShadowMap::Deactivate(const RenderContext& rc)
{
	rc.deviceContext->RSSetViewports(viewportCount, cachedViewports);
	rc.deviceContext->OMSetRenderTargets(1, cachedRenderTargetView.GetAddressOf(), cachedDepthStencilView.Get());
}

// ImGui描画
void CascadedShadowMap::DrawGui()
{
#if USE_IMGUI
	if (ImGui::Begin(u8"カスケードシャドウマップ"))
	{
		ImGui::SliderFloat("criticalDepthValue", &criticalDepthValue, 0.0f, +1000.0f);
		ImGui::SliderFloat("splitSchemeWeight", &splitSchemeWeight, 0.0f, +1.0f);
		ImGui::SliderFloat("zMult", &zMult, 1.0f, +100.0f);
		ImGui::Checkbox("fitToCascade", &fitToCascade);

		ImGui::SliderFloat("shadowColor", &parametricConstants.shadowColor, +0.0f, +1.0f);
		ImGui::DragFloat("shadowDepthBias", &parametricConstants.shadowDepthBias, 0.00001f, 0.0f, 0.01f, "%.8f");
		ImGui::DragFloat("shiftVolume", &parametricConstants.shiftVolume);
		bool flag = parametricConstants.colorizeCascadedLayer != 0.0f ? true : false;
		ImGui::Checkbox("colorizeCascadedLayer", &flag);
		parametricConstants.colorizeCascadedLayer = flag ? 1.0f : 0.0f;
	}
	ImGui::End();
#endif
}

void CascadedShadowMap::UpdateCSMConstants(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	// カスケードシャドウマップのシーン定数バッファ : スロット0
	Graphics::Instance().GetConstantBufferManager()->SetCB(dc, 0,
		ConstantBufferType::SceneCB, ConstantUpdateTarget::Pixel);

	// カスケードシャドウマップのパラメーター定数バッファ : スロット2
	dc->UpdateSubresource(parametricConstantBuffer.Get(), 0, 0, &parametricConstants, 0, 0);
	dc->PSSetConstantBuffers(2, 1, parametricConstantBuffer.GetAddressOf());

	// モデルの影を描画するときに必要な定数バッファ : スロット3
	dc->VSSetConstantBuffers(3, 1, constantBuffer.GetAddressOf());
	dc->PSSetConstantBuffers(3, 1, constantBuffer.GetAddressOf());
}
