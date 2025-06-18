#include "MenuCategory.h"

#include <imgui.h>

MenuCategory::MenuCategory(MenuMediator* menuMediator, const std::string& categoryName)
	: MenuCategoryBase(menuMediator, categoryName)
{
	_sprites["Base"].LoadTexture(L"./Data/Texture/Menu/CategoryFrame.png", Sprite::CenterAlignment::LeftCenter);
}

void MenuCategory::Render(Scene* scene, const RenderContext& rc, const Vector2& offset, const Vector2& offsetScale)
{		
	// スプライトの描画処理を実装
	for (auto& [name, sprite] : _sprites)
	{
		sprite.Render(rc, offset, offsetScale);
	}

	scene->GetTextRenderer().Draw(
		FontType::MSGothic, 
		_menuName.c_str(),
		(*_sprites.begin()).second.GetPosition() + offset,
		IsSelected() ? Vector4::Red : Vector4::White,
		0.0f,
		_textOffset,
		_textSize);
}

void MenuCategory::DrawGui()
{
	// スプライトのGUI描画
	for (auto& [name, spriteData] : _sprites)
	{
		if (ImGui::TreeNode(name.c_str()))
		{
			spriteData.DrawGui();
			ImGui::TreePop();
		}
	}
	ImGui::DragFloat2(u8"テキストオフセット", &_textOffset.x, 1.0f, -1000.0f, 1000.0f);
	ImGui::DragFloat2(u8"テキストサイズ", &_textSize.x, 1.0f, -1000.0f, 1000.0f);
}
