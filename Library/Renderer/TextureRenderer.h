#pragma once

#include <vector>
#include <string>
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <memory>

#include "../../Library/2D/Sprite.h"
#include "../Math/Vector.h"

class TextureRenderer
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="device"></param>
	void Initialize(ID3D11Device* device);

	/// <summary>
	/// FullscreenQuadの代わり
	/// </summary>
	/// <param name="dc"></param>
	/// <param name="srv">srv</param>
	/// <param name="startSlot">srvをセットする位置</param>
	/// <param name="numViews"></param>
	void Blit(
		ID3D11DeviceContext* dc,
		ID3D11ShaderResourceView** srv,
		uint32_t startSlot, 
		uint32_t numViews,
		ID3D11PixelShader* pixelShader = nullptr);

private:
	std::unique_ptr<Sprite>			_fullscreenQuad;
};