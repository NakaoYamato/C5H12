#define NOMINMAX

#include "CascadedShadowMap.h"

#include "../../Library/Graphics/Graphics.h"
#include "../../Graphics/GpuResourceManager.h"

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
	_cascadeCount(cascadeCount),
	_cascadedMatrices(cascadeCount),
	_cascadedPlaneDistances(cascadeCount + 1)
{
	HRESULT hr = S_OK;

	D3D11_TEXTURE2D_DESC texture2dDesc = {};
	texture2dDesc.Width = width;
	texture2dDesc.Height = height;
	texture2dDesc.MipLevels = 1;
	texture2dDesc.ArraySize = cascadeCount;
	texture2dDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	texture2dDesc.SampleDesc.Count = 1;
	texture2dDesc.SampleDesc.Quality = 0;
	texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
	texture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texture2dDesc.CPUAccessFlags = 0;
	texture2dDesc.MiscFlags = 0;
	hr = device->CreateTexture2D(&texture2dDesc, 0, _depthStencilBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	depthStencilViewDesc.Texture2DArray.FirstArraySlice = 0;
	depthStencilViewDesc.Texture2DArray.ArraySize = static_cast<UINT>(cascadeCount);
	depthStencilViewDesc.Texture2DArray.MipSlice = 0;
	depthStencilViewDesc.Flags = 0;
	hr = device->CreateDepthStencilView(_depthStencilBuffer.Get(), &depthStencilViewDesc, _depthStencilView.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
	shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT; // DXGI_FORMAT_R24_UNORM_X8_TYPELESS : DXGI_FORMAT_R32_FLOAT : DXGI_FORMAT_R16_UNORM
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	shaderResourceViewDesc.Texture2DArray.ArraySize = static_cast<UINT>(cascadeCount);
	shaderResourceViewDesc.Texture2DArray.MipLevels = 1;
	shaderResourceViewDesc.Texture2DArray.FirstArraySlice = 0;
	shaderResourceViewDesc.Texture2DArray.MostDetailedMip = 0;
	hr = device->CreateShaderResourceView(_depthStencilBuffer.Get(), &shaderResourceViewDesc, _shaderResourceView.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

	_viewport.Width = static_cast<float>(width);
	_viewport.Height = static_cast<float>(height);
	_viewport.MinDepth = 0.0f;
	_viewport.MaxDepth = 1.0f;
	_viewport.TopLeftX = 0.0f;
	_viewport.TopLeftY = 0.0f;

	// モデルの影を描画するときに必要な定数バッファ 
	(void)GpuResourceManager::CreateConstantBuffer(
		device,
		sizeof(Constants),
		_constantBuffer.GetAddressOf());
	// カスケードシャドウマップのパラメーター定数バッファ
	(void)GpuResourceManager::CreateConstantBuffer(
		device,
		sizeof(ParametricConstants),
		_parametricConstantBuffer.GetAddressOf());

	_fullscreenQuad = std::make_unique<SpriteResource>(device,
		L"",
		"./Data/Shader/HLSL/Sprite/FullscreenQuadVS.cso",
		"./Data/Shader/HLSL/PostProcess/CascadedShadowMap/CascadedShadowMapPS.cso");

    // デバッグ表示用SRV作成
	_debugSRVs.resize(cascadeCount);
	for (UINT i = 0; i < cascadeCount; ++i)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC debugSRVDesc = {};
		debugSRVDesc.Format = DXGI_FORMAT_R32_FLOAT;
		debugSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;

		debugSRVDesc.Texture2DArray.MostDetailedMip = 0;
		debugSRVDesc.Texture2DArray.MipLevels = 1;
		debugSRVDesc.Texture2DArray.FirstArraySlice = i; // i番目のカスケードを指定
		debugSRVDesc.Texture2DArray.ArraySize = 1;       // 1枚だけ切り抜く

		hr = device->CreateShaderResourceView(_depthStencilBuffer.Get(), &debugSRVDesc, _debugSRVs[i].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
	}
}
// 更新処理
void CascadedShadowMap::Update(float elapsedTime)
{
	_createShadow = false;
	_createShadowTimer += elapsedTime;
	if (_createShadowTimer >= _createShadowInterval)
	{
		_createShadowTimer = 0.0f;
		_createShadow = true;
	}
}
// 影の生成開始
void CascadedShadowMap::Activate(const RenderContext& rc,
	const UINT& cbSlot)
{
	ID3D11DeviceContext* immediateContext = rc.deviceContext;
	DirectX::XMFLOAT4X4 cameraProjection = rc.camera->GetProjection();
	DirectX::XMFLOAT4X4 cameraView = rc.camera->GetView();
	DirectX::XMFLOAT4 lightDirection = rc.lightDirection;

	immediateContext->RSGetViewports(&_viewportCount, _cachedViewports);
	immediateContext->OMGetRenderTargets(1, _cachedRTV.ReleaseAndGetAddressOf(), _cachedDSV.ReleaseAndGetAddressOf());

	// near/far value from perspective projection matrix
	float m33 = cameraProjection._33;
	float m43 = cameraProjection._43;
	float zn = -m43 / m33;
	float zf = (m33 * zn) / (m33 - 1);
	zf = _criticalDepthValue > 0 ? std::min(zf, _criticalDepthValue) : zf;

	// calculates split plane distances in view space
	for (size_t cascade_index = 0; cascade_index < _cascadeCount; ++cascade_index)
	{
		float idc = cascade_index / static_cast<float>(_cascadeCount);
		float logarithmic_split_scheme = zn * pow(zf / zn, idc);
		float uniform_split_scheme = zn + (zf - zn) * idc;
		_cascadedPlaneDistances.at(cascade_index) = logarithmic_split_scheme * _splitSchemeWeight + uniform_split_scheme * (1 - _splitSchemeWeight);
	}
	// make sure border values are accurate
	_cascadedPlaneDistances.at(0) = zn;
	_cascadedPlaneDistances.at(_cascadeCount) = zf;

	for (size_t cascade_index = 0; cascade_index < _cascadeCount; ++cascade_index)
	{
		float near_plane = _fitToCascade ? _cascadedPlaneDistances.at(cascade_index) : zn;
		float far_plane = _cascadedPlaneDistances.at(cascade_index + 1);

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

		// Upベクトルを動的に決定する
		DirectX::XMVECTOR lightDirVec = DirectX::XMLoadFloat4(&lightDirection);
		DirectX::XMVECTOR upVec = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		// ライト方向とUpベクトルの内積を取り、平行に近いか判定
		DirectX::XMVECTOR dotVec = DirectX::XMVector3Dot(DirectX::XMVector3Normalize(lightDirVec), upVec);
		float dot = DirectX::XMVectorGetX(dotVec);

		// 平行に近い場合(絶対値が1に近い場合)、UpベクトルをZ軸に変更
		if (std::abs(dot) > 0.99f)
		{
			upVec = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		}

		DirectX::XMMATRIX V;
		V = DirectX::XMMatrixLookAtLH(
			DirectX::XMVectorSet(center.x - lightDirection.x, center.y - lightDirection.y, center.z - lightDirection.z, 1.0f),
			DirectX::XMVectorSet(center.x, center.y, center.z, 1.0f),
			upVec);

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
		_zMult = std::max<float>(1.0f, _zMult);
		if (min_z < 0)
		{
			min_z *= _zMult;
		}
		else
		{
			min_z /= _zMult;
		}
		if (max_z < 0)
		{
			max_z /= _zMult;
		}
		else
		{
			max_z *= _zMult;
		}
#endif
		// シャドウマップの解像度（コンストラクタで渡されたwidthを使用）
		float shadowMapResolution = static_cast<float>(_viewport.Width);

		// 正射影の幅と高さ
		float frustumWidth = max_x - min_x;
		float frustumHeight = max_y - min_y;

		// 1テクセルあたりのワールド空間でのサイズ
		float worldUnitsPerTexelX = frustumWidth / shadowMapResolution;
		float worldUnitsPerTexelY = frustumHeight / shadowMapResolution;

		// min_x, min_y をテクセルサイズに合わせてスナップ（床関数で丸める）
		min_x = floorf(min_x / worldUnitsPerTexelX) * worldUnitsPerTexelX;
		max_x = min_x + frustumWidth; // 幅は変えずに再計算

		min_y = floorf(min_y / worldUnitsPerTexelY) * worldUnitsPerTexelY;
		max_y = min_y + frustumHeight;

		DirectX::XMMATRIX P = DirectX::XMMatrixOrthographicOffCenterLH(min_x, max_x, min_y, max_y, min_z, max_z);
		DirectX::XMStoreFloat4x4(&_cascadedMatrices.at(cascade_index), V * P);
	}

	Constants data;
	data.cascadedMatrices[0] = _cascadedMatrices.at(0);
	data.cascadedMatrices[1] = _cascadedMatrices.at(1);
	data.cascadedMatrices[2] = _cascadedMatrices.at(2);
	data.cascadedMatrices[3] = _cascadedMatrices.at(3);

	data.cascadedPlaneDistances[0] = _cascadedPlaneDistances.at(1);
	data.cascadedPlaneDistances[1] = _cascadedPlaneDistances.at(2);
	data.cascadedPlaneDistances[2] = _cascadedPlaneDistances.at(3);
	data.cascadedPlaneDistances[3] = _cascadedPlaneDistances.at(4);

	immediateContext->UpdateSubresource(_constantBuffer.Get(), 0, 0, &data, 0, 0);
	immediateContext->VSSetConstantBuffers(cbSlot, 1, _constantBuffer.GetAddressOf());
	immediateContext->DSSetConstantBuffers(cbSlot, 1, _constantBuffer.GetAddressOf());
	immediateContext->PSSetConstantBuffers(cbSlot, 1, _constantBuffer.GetAddressOf());

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> null_render_target_view;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> null_depth_stencil_view;
	immediateContext->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1, 0);
	immediateContext->OMSetRenderTargets(1, null_render_target_view.GetAddressOf(), _depthStencilView.Get());
	immediateContext->RSSetViewports(1, &_viewport);
}
// 深度バッファのクリア
void CascadedShadowMap::Clear(ID3D11DeviceContext* immediateContext)
{
	immediateContext->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1, 0);
}
// 影の生成開始（クリアしてから）
void CascadedShadowMap::ClearAndActivate(const RenderContext& rc)
{
	Clear(rc.deviceContext);
	Activate(rc, _CASCADED_SHADOW_MAP_CB_SLOT_INDEX);
}
// 影の生成終了
void CascadedShadowMap::Deactivate(const RenderContext& rc)
{
	rc.deviceContext->RSSetViewports(_viewportCount, _cachedViewports);
	rc.deviceContext->OMSetRenderTargets(1, _cachedRTV.GetAddressOf(), _cachedDSV.Get());
}
// ImGui描画
void CascadedShadowMap::DrawGui()
{
#if USE_IMGUI
	if (ImGui::Begin(u8"カスケードシャドウマップ"))
	{
		ImGui::DragFloat(u8"影生成タイマー", &_createShadowTimer);
		ImGui::DragFloat(u8"影生成間隔", &_createShadowInterval, 0.001f, 0.0f, 1.0f);

		ImGui::Separator();
		ImGui::SliderFloat("criticalDepthValue", &_criticalDepthValue, 0.0f, +1000.0f);
		ImGui::SliderFloat("splitSchemeWeight", &_splitSchemeWeight, 0.0f, +1.0f);
		ImGui::SliderFloat("zMult", &_zMult, 1.0f, +100.0f);
		ImGui::Checkbox("fitToCascade", &_fitToCascade);

		ImGui::SliderFloat("shadowColor", &_parametricConstants.shadowColor, +0.0f, +1.0f);
		ImGui::DragFloat("shadowDepthBias", &_parametricConstants.shadowDepthBias, 0.00001f, 0.0f, 0.01f, "%.8f");
		bool flag = _parametricConstants.colorizeCascadedLayer != 0.0f ? true : false;
		ImGui::Checkbox("colorizeCascadedLayer", &flag);
		_parametricConstants.colorizeCascadedLayer = flag ? 1.0f : 0.0f;

		if (ImGui::TreeNode(u8"シャドウマップデバッグ表示"))
		{
			// 表示サイズ（ウィンドウ幅に合わせて調整などお好みで）
			float aspect = _viewport.Width / _viewport.Height;
			float dispW = 200.0f;
			float dispH = dispW / aspect;

			for (size_t i = 0; i < _debugSRVs.size(); ++i)
			{
				ImGui::Text("Cascade %d", i);

				if (_debugSRVs[i])
				{
					ImGui::Image(
						(ImTextureID)_debugSRVs[i].Get(),
						ImVec2(dispW, dispH),
						ImVec2(0, 0), ImVec2(1, 1),
						ImVec4(1, 1, 1, 1),
						ImVec4(1, 1, 1, 1)
					);
				}
			}

            ImGui::TreePop();
		}
	}
	ImGui::End();
#endif
}
// カスケードシャドウマップ描画のための定数バッファ更新
void CascadedShadowMap::UpdateCSMConstants(const RenderContext& rc)
{
	ID3D11DeviceContext* dc = rc.deviceContext;

	// カスケードシャドウマップのシーン定数バッファ : スロット0
	Graphics::Instance().GetConstantBufferManager()->SetCB(dc, 0,
		ConstantBufferType::SceneCB, ConstantUpdateTarget::Pixel);

	// カスケードシャドウマップのパラメーター定数バッファ : スロット2
	dc->UpdateSubresource(_parametricConstantBuffer.Get(), 0, 0, &_parametricConstants, 0, 0);
	dc->PSSetConstantBuffers(2, 1, _parametricConstantBuffer.GetAddressOf());

	// モデルの影を描画するときに必要な定数バッファ : スロット3
	dc->VSSetConstantBuffers(3, 1, _constantBuffer.GetAddressOf());
	dc->PSSetConstantBuffers(3, 1, _constantBuffer.GetAddressOf());
}
// 影の描画
void CascadedShadowMap::Blit(ID3D11DeviceContext* immediateContext,
	ID3D11ShaderResourceView* colorSRV, 
	ID3D11ShaderResourceView* depthSRV,
	ID3D11ShaderResourceView* normalSRV)
{
	ID3D11ShaderResourceView* srvs[]
	{
		colorSRV, depthSRV, normalSRV, GetDepthMap().Get()
	};
	// 描画処理
	_fullscreenQuad->Blit(immediateContext,
		srvs,
		0, _countof(srvs));
}
