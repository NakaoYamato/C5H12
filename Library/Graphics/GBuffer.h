#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <cstdint>
#include <memory>
#include "../Math/Vector.h"
#include "../PostProcess/FrameBuffer.h"
#include "../2D/Sprite.h"

static constexpr UINT MAX_GBUFFER_COUNT = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT;

enum GBufferSRVType
{
	DiffuseColorSRV,
	AmbientColorSRV,
	SpecularColorSRV,
	WorldPositionSRV,
	WorldNormalSRV,
	DepthSRV,

	GBufferSRVTypeMAX,
};

class GBuffer
{
public:
	GBuffer(ID3D11Device* device, UINT width, UINT height, UINT bufferCount);
	virtual ~GBuffer();

	void Clear(ID3D11DeviceContext* immediateContext,
		const Vector4& color = Vector4(0.0f, 0.0f, 0.0f, 1.0f),
		const float& depth = 1.0f) const;

	void Activate(ID3D11DeviceContext* immediateContext);

	void ClearAndActivate(ID3D11DeviceContext* immediateContext,
		const Vector4& color = Vector4(0.0f, 0.0f, 0.0f, 1.0f),
		const float& depth = 1.0f);

	void Deactivate(ID3D11DeviceContext* immediateContext);

	void DrawGui();

#pragma region アクセサ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetRenderTargetSRV(UINT index) {
		return renderTargetSRVs[index];
	}
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetDepthStencilSRV() {
		return depthStencilSRV;
	}

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetColorSRV() {
		return frameBuffer_->GetColorSRV();
	}
#pragma endregion

private:
	void CreateSRV(ID3D11DeviceContext* immediateContext);

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

	Vector2 textureSize_{};
	std::unique_ptr<FrameBuffer> frameBuffer_;
	std::unique_ptr<Sprite> fullscreenQuad_;

	bool drawGui_ = false;
};