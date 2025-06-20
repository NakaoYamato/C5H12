#pragma once

#include "../../Source/Mediator/TitleMediator.h"

class TitleToHomeItem : public TitleItemBase
{
public:
	TitleToHomeItem(TitleMediator* titleMediator, const std::string& itemName);
	// XVˆ—
	void Update(float elapsedTime) override;
	// •`‰æ
	void Render(Scene* scene, const RenderContext& rc) override;
};