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
	BaseColorSRV,
	WorldNormalSRV,
	EmissiveSRV,
	ParameterSRV,

	GBufferSRVTypeMAX,
};

class GBuffer
{
public:
	GBuffer(ID3D11Device* device, UINT width, UINT height);
	virtual ~GBuffer();

	void Clear(ID3D11DeviceContext* immediateContext,
		const Vector4& color = Vector4(0.0f, 0.0f, 0.0f, 0.0f),
		const float& depth = 1.0f) const;

	void Activate(ID3D11DeviceContext* immediateContext);

	void ClearAndActivate(ID3D11DeviceContext* immediateContext,
		const Vector4& color = Vector4(0.0f, 0.0f, 0.0f, 0.0f),
		const float& depth = 1.0f);

	void Deactivate(ID3D11DeviceContext* immediateContext);

	void DrawGui();

	// GBufferのデータを書き出し
	void Blit(ID3D11DeviceContext* immediateContext);

#pragma region アクセサ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetRenderTargetSRV(UINT index) {
		return renderTargetSRVs_[index];
	}
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetDepthSRV() {
		return depthStencilSRV_;
	}

	bool IsActive()const { return isActive_; }
#pragma endregion
private:
	const UINT bufferCount_ = GBufferSRVType::GBufferSRVTypeMAX;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtvs_[MAX_GBUFFER_COUNT];
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> renderTargetSRVs_[MAX_GBUFFER_COUNT];
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> dsv_;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> depthStencilSRV_;
	D3D11_VIEWPORT viewports_[MAX_GBUFFER_COUNT];

	UINT viewportCount_ = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
	D3D11_VIEWPORT cachedViewports_[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE]{};
	ID3D11RenderTargetView* cachedRTVs_[MAX_GBUFFER_COUNT]{};
	ID3D11DepthStencilView* cachedDSV_{};

	Vector2 textureSize_{};
	std::unique_ptr<Sprite> fullscreenQuad_;

	bool drawGui_ = false;
	bool isActive_ = true;
};