#include "TitleToHomeItem.h"

#include "../../Library/Input/Input.h"

TitleToHomeItem::TitleToHomeItem(TitleMediator* titleMediator, const std::string& itemName)
	: TitleItemBase(titleMediator, itemName)
{
	_sprites["StartButton"].LoadTexture(
		L"./Data/Texture/Title/Start.png",
		Sprite::CenterAlignment::CenterCenter);
	_sprites["StartButton"].SetPosition(Vector2(600.0f, 600.0f));
}
// 更新処理
void TitleToHomeItem::Update(float elapsedTime)
{
	if (!_isActive)
		return;

	Vector2 mousePos{};
	mousePos.x = _INPUT_IS_AXIS("MousePositionX");
	mousePos.y = _INPUT_IS_AXIS("MousePositionY");
	if (_sprites["StartButton"].IsHit(mousePos))
	{
		// スタートボタンにマウスカーソルが当たっている場合、拡大
		_sprites["StartButton"].SetScale(Vector2(1.1f, 1.1f));
		// 入力処理
		if (_INPUT_IS_RELEASED("OK"))
		{
			// 次へ移行
			_titleMediator->ReceiveCommand(this->GetName(), TitleMediator::AllItem, TitleMediator::DeactivateCommand, 0.0f);
			_titleMediator->ReceiveCommand(this->GetName(), TitleMediator::ToOnlineItem, TitleMediator::ActivateCommand, 1.0f);
			_titleMediator->ReceiveCommand(this->GetName(), TitleMediator::ToOfflineItem, TitleMediator::ActivateCommand, 1.0f);
		}
	}
	else
	{
		// スタートボタンからマウスカーソルが離れた場合、元のサイズに戻す
		_sprites["StartButton"].SetScale(Vector2::One);
	}
}
// 描画
void TitleToHomeItem::Render(Scene* scene, const RenderContext& rc)
{
	if (!_isActive)
		return;

	_sprites["StartButton"].Render(rc, Vector2::Zero, Vector2::One);
}
