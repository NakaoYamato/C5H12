#include "MenuCategory.h"

#include <imgui.h>

MenuCategory::MenuCategory(MenuMediator* menuMediator, const std::string& categoryName)
	: MenuCategoryBase(menuMediator, categoryName)
{
	_sprites["Base"].LoadTexture(L"./Data/Texture/Menu/CategoryFrame.png", Sprite::CenterAlignment::LeftCenter);
}

void MenuCategory::Update(float elapsedTime)
{
	auto menuInput = _menuMediator->GetMenuInput();
	if (menuInput == nullptr || !menuInput->IsActive())
		return;

	if (menuInput->IsInput(MenuInput::InputType::Select))
		_menuMediator->ReceiveCommand(
			_menuName,
			MenuMediator::ToItem,
			MenuMediator::OpenCommand,
			0.0f);
	else if (menuInput->IsInput(MenuInput::InputType::Back))
		_menuMediator->ReceiveCommand(
			_menuName,
			MenuMediator::ToAllObject,
			MenuMediator::EscapeCommand,
			0.0f);
	else if (menuInput->IsInput(MenuInput::InputType::Left))
		_menuMediator->ReceiveCommand( 
			_menuName,
			MenuMediator::ToBackCategory,
			MenuMediator::SelectionCommand,
			0.0f);
	else if (menuInput->IsInput(MenuInput::InputType::Right))
		_menuMediator->ReceiveCommand(
			_menuName,
			MenuMediator::ToNextCategory,
			MenuMediator::SelectionCommand,
			0.0f);
	else if (menuInput->IsInput(MenuInput::InputType::Up))
		_menuMediator->ReceiveCommand(
			_menuName,
			MenuMediator::ToBackItem,
			MenuMediator::SelectionCommand,
			0.0f);
	else if (menuInput->IsInput(MenuInput::InputType::Down))
		_menuMediator->ReceiveCommand(
			_menuName,
			MenuMediator::ToNextItem,
			MenuMediator::SelectionCommand,
			0.0f);
}

void MenuCategory::Render(Scene* scene, const RenderContext& rc, const Vector2& offset, const Vector2& offsetScale)
{		
	// スプライトの描画処理を実装
	for (auto& [name, sprite] : _sprites)
	{
		sprite.Render(rc, scene->GetTextureRenderer());
	}

	scene->GetTextRenderer().Draw(
		FontType::MSGothic, 
		_menuName.c_str(),
		(*_sprites.begin()).second.GetRectTransform().GetWorldPosition() + offset,
		IsActive() ? Vector4::Red : Vector4::White,
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

void MenuCategory::ExecuteCommand(const MenuMediator::CommandData& command)
{
	MenuObjectBase::ExecuteCommand(command);

	//// アイテムがアクティブな場合は、非アクティブ化
	//if (command.command == MenuMediator::ActivateCommand && command.target == MenuMediator::ToAllItem)
	//{
	//	SetActive(false);
	//}
}
