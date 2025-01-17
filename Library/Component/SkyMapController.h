#pragma once

#include "Component.h"
#include "../../Library/3D/SkyMap.h"

class SkyMapController : public Component
{
public:
	SkyMapController(const wchar_t* filename);
	~SkyMapController()override {}

	// –¼‘Oæ“¾
	const char* GetName()const { return "SkyMapController"; }

	// •`‰æ‚Ì‘Oˆ—
	void RenderPreprocess(RenderContext& rc) override;

	// •`‰æˆ—
	void Render(const RenderContext& rc) override;

	// GUI•`‰æ
	void DrawGui() override {}

private:
	std::unique_ptr<SkyMap> skyMap;
};