#include "SkyMapController.h"

#include "../../Library/Graphics/Graphics.h"
SkyMapController::SkyMapController(const wchar_t* filename)
{
	skyMap = std::make_unique<SkyMap>(Graphics::Instance().GetDevice(), filename);
}

void SkyMapController::RenderPreprocess(RenderContext& rc)
{
	// RenderContext‚É“o˜^
	rc.environmentMap = skyMap->shaderResourceView.GetAddressOf();
}

void SkyMapController::Render(const RenderContext& rc)
{
	skyMap->Blit(rc);
}
