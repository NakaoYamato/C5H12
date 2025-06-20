#pragma once

#include "../../Source/Mediator/TitleMediator.h"

class TitleToExitItem : public TitleItemBase
{
public:
	TitleToExitItem(TitleMediator* titleMediator, const std::string& itemName);
	// 更新処理
	void Update(float elapsedTime) override;
	// 描画
	void Render(Scene* scene, const RenderContext& rc) override;
	// コマンドを実行
	void ExecuteCommand(const TitleMediator::CommandData& commandData) override;
};