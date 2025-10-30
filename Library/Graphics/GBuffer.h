#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <cstdint>
#include <memory>
#include "../Math/Vector.h"
#include "../PostProcess/FrameBuffer.h"
#include "../../Library/Renderer/TextureRenderer.h"

// GBufferで使用するレンダーターゲットの枚数
static constexpr UINT GBUFFER_RTV_COUNT = 4;
// DirectX11で同時に登録できるレンダーターゲットの上限
static constexpr UINT MAX_GBUFFER_COUNT = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT;

static constexpr UINT GBUFFER_COLOR_MAP_INDEX = 0;
static constexpr UINT GBUFFER_NORMAL_MAP_INDEX = 1;
static constexpr UINT GBUFFER_EMISSIVE_MAP_INDEX = 2;
static constexpr UINT GBUFFER_PARAMETER_MAP_INDEX = 3;

static constexpr UINT FOG_CONSTANT_BUFFER_INDEX = 4;
static constexpr UINT SSR_CONSTANT_BUFFER_INDEX = 6;

class GBuffer
{
public:
	// 描画タイプ
	enum RenderingType
	{
		Phong,
		PBR,

		RenderingTypeMax,
	};

	struct CbFog
	{
		Vector4 fogColor = Vector4::Gray; // フォグの色
		Vector2 fogRange = Vector2(40.0f, 300.0f); // フォグの開始距離と終了距離
		Vector2 fogPadding{};
	};
	struct SSRConstants
	{
		DirectX::XMFLOAT4X4		view{};
		DirectX::XMFLOAT4X4		projection{};
		DirectX::XMFLOAT4X4		invViewProjection{};
		DirectX::XMFLOAT4X4		invProjection{};

		// 反射強度
		float refrectionIntensity = 0.5f;
		float maxDistance = 15.0f;
		float resolution = 0.3f;
		float thickness = 0.5f;
	};
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
	void Blit(TextureRenderer& textureRenderer, ID3D11DeviceContext* immediateContext);

#pragma region アクセサ
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& GetRenderTargetView(UINT index) {
		return _rtvs[index];
	}
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetRenderTargetSRV(UINT index) {
		return _renderTargetSRVs[index];
	}
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& GetDepthStencilView() {
		return _dsv;
	}
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetDepthSRV() {
		return _depthStencilSRV;
	}
	UINT GetWidth() const {
		return _width;
	}
	UINT GetHeight() const {
		return _height;
	}
#pragma endregion
private:
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> _rtvs[MAX_GBUFFER_COUNT];
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _renderTargetSRVs[MAX_GBUFFER_COUNT];
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> _dsv;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _depthStencilSRV;
	D3D11_VIEWPORT _viewports[MAX_GBUFFER_COUNT];
	UINT _width = 0;
	UINT _height = 0;

	UINT _viewportCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
	D3D11_VIEWPORT _cachedViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE]{};
	ID3D11RenderTargetView* _cachedRTVs[MAX_GBUFFER_COUNT]{};
	ID3D11DepthStencilView* _cachedDSV{};

	Vector2 _textureSize{};
	Microsoft::WRL::ComPtr<ID3D11PixelShader> _gbufferPS[RenderingType::RenderingTypeMax];
	Microsoft::WRL::ComPtr<ID3D11PixelShader> _depthWritePS;
	std::unique_ptr<FrameBuffer> _frameBuffer;

	// フォグ
	struct CbFog _fogConstants;
	Microsoft::WRL::ComPtr<ID3D11Buffer> _fogConstantBuffer;

	int _renderingType = RenderingType::PBR;
	bool _useSSR = false;

	// SSR用
	SSRConstants _ssrConstants;
	std::unique_ptr<SpriteResource> _ssrFullscreenQuad;
	Microsoft::WRL::ComPtr<ID3D11Buffer> _ssrConstantBuffer;
};