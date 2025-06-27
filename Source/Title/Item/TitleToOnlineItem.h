#pragma once

#include "../TitleMediator.h"

class TitleToOnlineItem : public TitleItemBase
{
public:
	TitleToOnlineItem(TitleMediator* titleMediator, const std::string& itemName);
	// XVˆ—
	void Update(float elapsedTime) override;
	// •`‰æ
	void Render(Scene* scene, const RenderContext& rc) override;
};