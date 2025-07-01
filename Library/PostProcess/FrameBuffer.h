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
	FrameBuffer(ID3D11Device* device, uint32_t width, uint32_t height, bool hasDepthstencil = true);
	FrameBuffer() = delete;
	virtual ~FrameBuffer() = default;

	void Clear(ID3D11DeviceContext* immediateContext,
		const Vector4& color = Vector4::Zero,
		const float& depth = 1.0f) const;

	void Activate(ID3D11DeviceContext* immediateContext);

	void ClearAndActivate(ID3D11DeviceContext* immediateContext,
		const Vector4& color = Vector4::Zero,
		const float& depth = 1.0f);

	void Deactivate(ID3D11DeviceContext* immediateContext);
	void Deactivate(ID3D11DeviceContext* immediateContext,
		const Vector4& high_luminance_color);

#pragma region アクセサ
	uint32_t GetWidth() const { return _width; }
    uint32_t GetHeight() const { return _height; }
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& GetRTV() { return _rtv; }
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& GetDSV() { return _dsv; }
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetColorSRV() { return _srvs[0]; }
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetDepthSRV() { return _srvs[1]; }
#pragma endregion

private:
	uint32_t _width{};
	uint32_t _height{};
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> _rtv;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> _dsv;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _srvs[2];
	D3D11_VIEWPORT _viewport;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		_constantBuffer{};

	UINT _viewportCount{ D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE };
	D3D11_VIEWPORT _cachedViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE]{};
	ID3D11RenderTargetView* _cachedRTVs[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> _cachedDSV;

};