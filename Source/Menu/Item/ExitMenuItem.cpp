#include "ExitMenuItem.h"

#include "../../Library/Graphics/Graphics.h"
#include <imgui.h>

ExitMenuItem::ExitMenuItem(MenuMediator* menuMediator, const std::string& itemName)
	: MenuItemBase(menuMediator, itemName)
{
	_sprites["Base"].LoadTexture(L"./Data/Texture/Menu/ExitButton.png", Sprite::CenterAlignment::LeftCenter);
	_sprites["Base"].SetColor(Vector4(1.0f, 1.0f, 1.0f, 0.5f)); // 半透明に設定
}

void ExitMenuItem::Update(float elapsedTime)
{
	if (_menuMediator->GetMenuInput()->IsInput(MenuInput::InputType::Select))
	{
		// アプリ終了
		PostMessage(Graphics::Instance().GetHwnd(), WM_CLOSE, 0, 0);
	}
}

void ExitMenuItem::Render(Scene* scene, const RenderContext& rc, const Vector2& offset, const Vector2& offsetScale)
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
		IsSelected() ? Vector4::Red : Vector4::White,
		0.0f,
		_textOffset,
		_textSize);
}

void ExitMenuItem::DrawGui()
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

void ExitMenuItem::ExecuteCommand(const std::string& command)
{
}
