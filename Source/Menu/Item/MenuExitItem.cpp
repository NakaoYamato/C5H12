#include "MenuExitItem.h"

#include "../../Library/Graphics/Graphics.h"
#include <imgui.h>

MenuExitItem::MenuExitItem(MenuMediator* menuMediator, const std::string& itemName)
	: MenuItemBase(menuMediator, itemName)
{
	_sprites["Base"].LoadTexture(L"./Data/Texture/Menu/ExitButton.png", Sprite::CenterAlignment::LeftCenter);
	_sprites["Base"].SetColor(Vector4(1.0f, 1.0f, 1.0f, 0.5f)); // 半透明に設定
}

void MenuExitItem::Update(float elapsedTime)
{
	auto menuInput = _menuMediator->GetMenuInput();
	if (menuInput == nullptr || !menuInput->IsActive())
		return;

	if (menuInput->IsInput(MenuInput::InputType::Select))
	{
		// アプリ終了
		PostMessage(Graphics::Instance().GetHwnd(), WM_CLOSE, 0, 0);
	}
	else if (menuInput->IsInput(MenuInput::InputType::Back))
	{
		_menuMediator->ReceiveCommand(
			_menuName,
			MenuMediator::ToItem,
			MenuMediator::CloseCommand,
			0.0f);
	}
}

void MenuExitItem::Render(Scene* scene, const RenderContext& rc, const Vector2& offset, const Vector2& offsetScale)
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
		IsActive() ? IsOpen() ? Vector4::Green : Vector4::Red : Vector4::White,
		0.0f,
		_textOffset,
		_textSize);

	if (IsOpen())
	{

	}
}

void MenuExitItem::DrawGui()
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

