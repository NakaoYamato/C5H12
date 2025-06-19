#include "MenuItem.h"

#include <imgui.h>

MenuItem::MenuItem(MenuMediator* menuMediator, const std::string& itemName)
	: MenuItemBase(menuMediator, itemName) 
{
	_sprites["Base"].LoadTexture(L"./Data/Texture/Menu/ItemBack.png", Sprite::CenterAlignment::LeftCenter);
	_sprites["Base"].SetColor(Vector4(1.0f, 1.0f, 1.0f, 0.5f)); // 半透明に設定
}

void MenuItem::Render(Scene* scene, const RenderContext& rc, const Vector2& offset, const Vector2& offsetScale)
{
	// アイテムの描画処理を実装
	for (auto& [name, sprite] : _sprites)
	{
		sprite.Render(rc, offset, offsetScale);
	}
	scene->GetTextRenderer().Draw(
		FontType::MSGothic,
		_menuName.c_str(),
		(*_sprites.begin()).second.GetPosition() + offset,
		IsSelected() ? IsOpen() ? Vector4::Green : Vector4::Red : Vector4::White,
		0.0f,
		_textOffset,
		_textSize);
}

void MenuItem::DrawGui()
{
	// スプライトのGUI描画
	for (auto& [name, spriteData] : _sprites) {
		if (ImGui::TreeNode(name.c_str())) {
			spriteData.DrawGui();
			ImGui::TreePop();
		}
	}
	ImGui::DragFloat2(u8"テキストオフセット", &_textOffset.x, 1.0f, -1000.0f, 1000.0f);
	ImGui::DragFloat2(u8"テキストサイズ", &_textSize.x, 1.0f, -1000.0f, 1000.0f);
}
