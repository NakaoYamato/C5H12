#pragma once

#include "../MenuMediator.h"

class MenuExitItem : public MenuItemBase
{
public:
	MenuExitItem(MenuMediator* menuMediator, const std::string& itemName);
	void Update(float elapsedTime) override;
	void Render(Scene* scene, const RenderContext& rc, const Vector2& offset, const Vector2& offsetScale) override;
	void DrawGui() override;
private:
	// テキストのオフセット位置
	Vector2 _textOffset = Vector2(-10.0f, 20.0f);
	// テキストのサイズ
	Vector2 _textSize = Vector2(0.5f, 0.5f);
};