#include "TitleToOnlineItem.h"

#include "../../Library/Input/Input.h"

TitleToOnlineItem::TitleToOnlineItem(TitleMediator* titleMediator, const std::string& itemName)
	: TitleItemBase(titleMediator, itemName)
{
	_sprites["Button"].LoadTexture(
		L"./Data/Texture/Title/ToOnline.png",
		Sprite::CenterAlignment::CenterCenter);
	_sprites["Button"].SetPosition(Vector2(600.0f, 400.0f));
}
// 更新処理
void TitleToOnlineItem::Update(float elapsedTime)
{
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
			// 次へ移行
			_titleMediator->ReceiveCommand(this->GetName(), TitleMediator::AllItem, TitleMediator::DeactivateCommand, 0.0f);
			_titleMediator->ReceiveCommand(this->GetName(), TitleMediator::ToOnlineSettingItem, TitleMediator::ActivateCommand, 1.0f);
		}
	}
	else
	{
		// マウスカーソルが離れた場合、元のサイズに戻す
		_sprites["Button"].SetScale(Vector2::One);
	}
}
// 描画
void TitleToOnlineItem::Render(Scene* scene, const RenderContext& rc)
{
	_sprites["Button"].Render(rc, Vector2::Zero, Vector2::One);
}
