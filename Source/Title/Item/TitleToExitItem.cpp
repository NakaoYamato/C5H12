#include "TitleToExitItem.h"
#include "../../Library/Input/Input.h"
#include "../../Library/Graphics/Graphics.h"

#include <imgui.h>

TitleToExitItem::TitleToExitItem(TitleMediator* titleMediator, const std::string& itemName)
	: TitleItemBase(titleMediator, itemName)
{
	_sprites["Button"].LoadTexture(
		L"./Data/Texture/Title/Exit.png",
		Sprite::CenterAlignment::CenterCenter);
	_sprites["Button"].SetPosition(Vector2(1150.0f, 80.0f));

	// アクティブ状態にする
	_isActive = true;
}
// 更新処理
void TitleToExitItem::Update(float elapsedTime)
{
	if (!_isActive)
		return;

	// ImGuiのウィンドウを選択していたら処理しない
	if (ImGui::IsWindowHovered(ImGuiHoveredFlags_::ImGuiHoveredFlags_AnyWindow) ||
		ImGui::IsWindowFocused(ImGuiFocusedFlags_::ImGuiFocusedFlags_AnyWindow))
		return;

	Vector2 mousePos{};
	mousePos.x = _INPUT_IS_AXIS("MousePositionX");
	mousePos.y = _INPUT_IS_AXIS("MousePositionY");
	if (_sprites["Button"].IsHit(mousePos))
	{
		// マウスカーソルが当たっている場合、拡大
		_sprites["Button"].SetScale(Vector2(1.1f, 1.1f));
		// 入力処理
		if (_INPUT_IS_RELEASED("OK"))
		{
			// アプリ終了
			PostMessage(Graphics::Instance().GetHwnd(), WM_CLOSE, 0, 0);
		}
	}
	else
	{
		// マウスカーソルが離れた場合、元のサイズに戻す
		_sprites["Button"].SetScale(Vector2::One);
	}
}
// 描画
void TitleToExitItem::Render(Scene* scene, const RenderContext& rc)
{
	if (!_isActive)
		return;
	_sprites["Button"].Render(rc, Vector2::Zero, Vector2::One);
}
// コマンドを実行
void TitleToExitItem::ExecuteCommand(const std::string& command)
{
	// 常にアクティブなので何もしない
}