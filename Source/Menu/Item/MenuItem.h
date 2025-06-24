#pragma once

#include "../../Source/Mediator/MenuMediator.h"

class MenuItem : public MenuItemBase
{
public:
	MenuItem(MenuMediator* menuMediator, const std::string& itemName);
	~MenuItem() override = default;
	void Update(float elapsedTime) override;
	void Render(Scene* scene, const RenderContext& rc, const Vector2& offset, const Vector2& offsetScale) override;
	void DrawGui() override;
private:
	// テキストのオフセット位置
	Vector2 _textOffset = Vector2(-10.0f, 20.0f);
	// テキストのサイズ
	Vector2 _textSize = Vector2(0.5f, 0.5f);
};