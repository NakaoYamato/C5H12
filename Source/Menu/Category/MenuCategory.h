#pragma once

#include "../../Source/Mediator/MenuMediator.h"

class MenuCategory : public MenuCategoryBase
{
public:
	MenuCategory(MenuMediator* menuMediator, const std::string& categoryName);
	~MenuCategory() override = default;
	void Update(float elapsedTime) override {}
	void Render(Scene* scene, const RenderContext& rc, const Vector2& offset, const Vector2& offsetScale) override;
	void DrawGui() override;
	void ExecuteCommand(const std::string& command) override
	{
		//if (_menuMediator)
		//	_menuMediator->ExecuteCommand(command);
	}

	// テキストのオフセット位置を設定
	void SetTextOffset(const Vector2& offset) { _textOffset = offset; }
private:
	// テキストのオフセット位置
	Vector2 _textOffset = Vector2(-10.0f, 20.0f);
	// テキストのサイズ
	Vector2 _textSize	= Vector2(0.5f, 0.5f);
};