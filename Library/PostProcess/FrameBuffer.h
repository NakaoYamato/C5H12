#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <cstdint>
#include <DirectXMath.h>
#include <vector>
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
	FrameBuffer(
		ID3D11Device* device, 
		uint32_t width,
		uint32_t height, 
		bool hasDepthstencil = true,
		std::vector<DXGI_FORMAT> formats = std::vector<DXGI_FORMAT>({ DXGI_FORMAT_R16G16B16A16_FLOAT }));
	FrameBuffer() = delete;
	virtual ~FrameBuffer() = default;
	// RTVとDSVに設定されているSRVを指定色でクリア
	void Clear(ID3D11DeviceContext* immediateContext,
		const Vector4& color = Vector4::Zero,
		const float& depth = 1.0f) const;
	// すべてのRTVとDSVをアクティブにする
	void Activate(ID3D11DeviceContext* immediateContext);
	// すべてのRTVとDSVをアクティブにし、指定色でクリアする
	void ClearAndActivate(ID3D11DeviceContext* immediateContext,
		const Vector4& color = Vector4::Zero,
		const float& depth = 1.0f);
	// すべてのRTVとDSVを非アクティブにする
	void Deactivate(ID3D11DeviceContext* immediateContext);
	void Deactivate(ID3D11DeviceContext* immediateContext,
		const Vector4& high_luminance_color);
	// 指定のRTVとDSVに設定されているSRVを指定色でクリア
	void Clear(size_t index,
		ID3D11DeviceContext* immediateContext,
		const Vector4& color = Vector4::Zero,
		const float& depth = 1.0f) const;
	// 指定のRTVとDSVをアクティブにする
	void Activate(size_t index, ID3D11DeviceContext* immediateContext);
	// 指定のRTVとDSVをアクティブにし、指定色でクリアする
	void ClearAndActivate(size_t index, 
		ID3D11DeviceContext* immediateContext,
		const Vector4& color = Vector4::Zero,
		const float& depth = 1.0f);

#pragma region アクセサ
	uint32_t GetWidth() const { return _width; }
    uint32_t GetHeight() const { return _height; }
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& GetRTV(size_t index = 0) { return _rtvs[index]; }
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& GetDSV() { return _dsv; }
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetColorSRV(size_t index = 0) { return _srvs[index]; }
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetDepthSRV() { return _dsvSRV; }
#pragma endregion

private:
	uint32_t _width{};
	uint32_t _height{};
	std::vector<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>> _rtvs;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> _dsv;
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> _srvs;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _dsvSRV;
	D3D11_VIEWPORT _viewport;
	Microsoft::WRL::ComPtr<ID3D11Buffer>		_constantBuffer{};

	UINT _viewportCount{ D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE };
	D3D11_VIEWPORT _cachedViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE]{};
	ID3D11RenderTargetView* _cachedRTVs[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> _cachedDSV;

};