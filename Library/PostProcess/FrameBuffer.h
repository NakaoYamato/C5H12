#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <cstdint>
#include <DirectXMath.h>
#include "../Math/Vector.h"

/// <summary>
/// オフスクリーンレンダリング用バッファ
/// </summary>
class FrameBuffer
{
public:
	struct Constants
	{
		Vector4 color;
	};

public:
	FrameBuffer(ID3D11Device* device, uint32_t width, uint32_t height, bool hasDepthstencil = true/*BLOOM*/);
	FrameBuffer() = delete;
	virtual ~FrameBuffer() = default;

	void Clear(ID3D11DeviceContext* immediateContext,
		const Vector4& color = Vector4(0.0f, 0.0f, 0.0f, 0.0f),
		const float& depth = 1.0f) const;

	void Activate(ID3D11DeviceContext* immediateContext);

	void ClearAndActivate(ID3D11DeviceContext* immediateContext,
		const Vector4& color = Vector4(0.0f, 0.0f, 0.0f, 0.0f),
		const float& depth = 1.0f);

	void Deactivate(ID3D11DeviceContext* immediateContext);
	void Deactivate(ID3D11DeviceContext* immediateContext,
		const Vector4& high_luminance_color);

	// アクセサ
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& GetRTV() { return rtv; }
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& GetDSV() { return dsv; }
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetColorSRV() { return srvs[0]; }
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetDepthSRV() { return srvs[1]; }

private:
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> dsv;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srvs[2];
	D3D11_VIEWPORT viewport;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		constantBuffer{};

	UINT viewportCount{ D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE };
	D3D11_VIEWPORT cachedViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE]{};
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> cachedRTV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> cachedDSV;
};