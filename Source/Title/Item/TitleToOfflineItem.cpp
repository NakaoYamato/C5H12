#include "TitleToOfflineItem.h"
#include "../../Library/Input/Input.h"
#include "../../Source/Scene/Game/SceneGame.h"

TitleToOfflineItem::TitleToOfflineItem(TitleMediator* titleMediator, const std::string& itemName)
	: TitleItemBase(titleMediator, itemName)
{
	_sprites["Button"].LoadTexture(
		L"./Data/Texture/Title/ToOffline.png",
		Sprite::CenterAlignment::CenterCenter);
	_sprites["Button"].GetRectTransform().SetLocalPosition(Vector2(600.0f, 600.0f));
}
// 更新処理
void TitleToOfflineItem::Update(float elapsedTime)
{
	Vector2 mousePos{};
	mousePos.x = Input::Instance().GetMouseInput()->GetCurrentCursorPosX();
	mousePos.y = Input::Instance().GetMouseInput()->GetCurrentCursorPosY();
	if (_sprites["Button"].IsHit(mousePos))
	{
		// マウスカーソルが当たっている場合、拡大
		_sprites["Button"].GetRectTransform().SetLocalScale(Vector2(1.1f, 1.1f));
		// 入力処理
		if (_INPUT_RELEASED("OK"))
		{
			// 次へ移行
			SceneGame::NetworkEnabled = false;
			SceneManager::Instance().ChangeScene(SceneMenuLevel::Game, "Game");
		}
	}
	else
	{
		// マウスカーソルが離れた場合、元のサイズに戻す
		_sprites["Button"].GetRectTransform().SetLocalScale(Vector2::One);
	}
}
// 描画
void TitleToOfflineItem::Render(Scene* scene, const RenderContext& rc)
{
	_sprites["Button"].Render(rc, scene->GetTextureRenderer());
}