#pragma once

#include "../../Source/Mediator/TitleMediator.h"

class TitleToOnlineSettingItem : public TitleItemBase
{
public:
	TitleToOnlineSettingItem(TitleMediator* titleMediator, const std::string& itemName);
	// 更新処理
	void Update(float elapsedTime) override;
	// 描画
	void Render(Scene* scene, const RenderContext& rc) override;
private:
	// ネットワークアドレス
	static std::string NetworkAddress;
};