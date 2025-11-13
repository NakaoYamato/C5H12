#pragma once

#include <vector>
#include <string>
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <memory>

#include "../../Library/2D/SpriteResource.h"
#include "../../Library/Graphics/RenderContext.h"
#include "../../Library/Graphics/Texture.h"
#include "../../Library/Material/Material.h"
#include "../Math/Vector.h"

class TextureRenderer
{
public:
	struct Vertex
	{
		Vector3 position{};
		Vector4 color{};
		Vector2 texcoord{};
	};

public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="device"></param>
	void Initialize(ID3D11Device* device);

	void Render(const RenderContext& rc,
		ID3D11ShaderResourceView*const* srv,
		const D3D11_TEXTURE2D_DESC& textureDesc,
		const Vector2& position = Vector2::Zero,
		const Vector2& scale = Vector2::One,
		const Vector2& texPos = Vector2::Zero,
		const Vector2& texSize = Vector2::Zero,
		const Vector2& center = Vector2::Zero,
		float			angle = 0.0f,
		const Vector4& color = Vector4::White,
		Material* material = nullptr);

	void Render(const RenderContext& rc,
		Texture*		texture,
		const Vector2&	position = Vector2::Zero,
		const Vector2&	scale = Vector2::One,
		const Vector2&	texPos = Vector2::Zero,
		const Vector2&	texSize = Vector2::Zero,
		const Vector2&	center = Vector2::Zero,
		float			angle = 0.0f,
		const Vector4&	color = Vector4::White,
		Material*		material = nullptr);

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
	Microsoft::WRL::ComPtr<ID3D11Buffer>		_vertexBuffer;
};