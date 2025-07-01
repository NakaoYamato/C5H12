#include "TextureRenderer.h"

/// ‰Šú‰»
void TextureRenderer::Initialize(ID3D11Device* device)
{
	_fullscreenQuad = std::make_unique<SpriteResource>(device,
		L"",
		"./Data/Shader/FullscreenQuadVS.cso",
		"./Data/Shader/FullscreenQuadPS.cso");
}

void TextureRenderer::Blit(
	ID3D11DeviceContext* dc,
	ID3D11ShaderResourceView** srv,
	uint32_t startSlot, 
	uint32_t numViews, 
	ID3D11PixelShader* pixelShader)
{
	_fullscreenQuad->Blit(dc, srv, startSlot, numViews, pixelShader);
}
