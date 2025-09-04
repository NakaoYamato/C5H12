#pragma once

#include "../TitleMediator.h"

class TitleToOfflineItem : public TitleItemBase
{
public:
	TitleToOfflineItem(TitleMediator* titleMediator, const std::string& itemName);
	// XVˆ—
	void Update(float elapsedTime) override;
	// •`‰æ
	void Render(Scene* scene, const RenderContext& rc) override;
};