#include "TitleToHomeItem.h"

#include "../../Library/Input/Input.h"

TitleToHomeItem::TitleToHomeItem(TitleMediator* titleMediator, const std::string& itemName)
	: TitleItemBase(titleMediator, itemName)
{
	_sprites["StartButton"].LoadTexture(
		L"./Data/Texture/Title/Start.png",
		Sprite::CenterAlignment::CenterCenter);
	_sprites["StartButton"].GetRectTransform().SetLocalPosition(Vector2(600.0f, 600.0f));
}
// 更新処理
void TitleToHomeItem::Update(float elapsedTime)
{
	Vector2 mousePos{};
	mousePos.x = Input::Instance().GetMouseInput()->GetCurrentCursorPosX();
	mousePos.y = Input::Instance().GetMouseInput()->GetCurrentCursorPosY();
	if (_sprites["StartButton"].IsHit(mousePos))
	{
		// スタートボタンにマウスカーソルが当たっている場合、拡大
		_sprites["StartButton"].GetRectTransform().SetLocalScale(Vector2(1.1f, 1.1f));
		// 入力処理
		if (_INPUT_RELEASED("OK"))
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
		_sprites["StartButton"].GetRectTransform().SetLocalScale(Vector2::One);
	}
}
// 描画
void TitleToHomeItem::Render(Scene* scene, const RenderContext& rc)
{
	_sprites["StartButton"].Render(rc, scene->GetTextureRenderer());
}
