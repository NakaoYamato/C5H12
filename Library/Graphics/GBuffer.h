#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <cstdint>
#include "../Math/Vector.h"

static constexpr UINT MAX_GBUFFER_COUNT = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT;

class GBuffer
{
public:
	GBuffer(ID3D11Device* device, UINT width, UINT height, UINT bufferCount);
	virtual ~GBuffer();

	void Clear(ID3D11DeviceContext* immediateContext,
		const Vector4& color = Vector4(0.0f, 0.0f, 0.0f, 1.0f),
		const float& depth = 1.0f) const;

	void Activate(ID3D11DeviceContext* immediateContext);

	void ClearAndActive(ID3D11DeviceContext* immediateContext,
		const Vector4& color = Vector4(0.0f, 0.0f, 0.0f, 1.0f),
		const float& depth = 1.0f);

	void Deactivate(ID3D11DeviceContext* immediateContext);

#pragma region アクセサ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetRenderTargetSRV(UINT index) {
		return renderTargetSRVs[index];
	}
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetDepthStencilSRV() {
		return depthStencilSRV;
	}
#pragma endregion

private:
	const UINT bufferCount;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtvs[MAX_GBUFFER_COUNT];
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> renderTargetSRVs[MAX_GBUFFER_COUNT];
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> dsv;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> depthStencilSRV;
	D3D11_VIEWPORT viewports[MAX_GBUFFER_COUNT];

	UINT viewportCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
	D3D11_VIEWPORT cachedViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE]{};
	ID3D11RenderTargetView* cachedRTVs[MAX_GBUFFER_COUNT]{};
	ID3D11DepthStencilView* cachedDSV{};
};