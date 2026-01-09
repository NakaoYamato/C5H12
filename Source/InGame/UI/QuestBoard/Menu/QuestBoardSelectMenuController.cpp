#include "QuestBoardSelectMenuController.h"

#include "../QuestBoardUIController.h"
#include "../../Library/Actor/UI/UIActor.h"

#include <imgui.h>

// 開始処理
void QuestBoardSelectMenuController::Start()
{
    _spriteRenderer = this->GetActor()->GetComponent<SpriteRenderer>();

    if (auto spriteRenderer = _spriteRenderer.lock())
    {
        if (!spriteRenderer->IsLoaded())
        {
			spriteRenderer->LoadTexture(NewOrder, L"");
			spriteRenderer->LoadTexture(AcceptMenu, L"");
			spriteRenderer->LoadTexture(FrontSpr, L"");
        }
    }
}

// GUI描画
void QuestBoardSelectMenuController::DrawGui()
{
}

// インデックス追加
// direction
// 0b0001:up 
// 0b0010:down 
// 0b0100:left 
// 0b1000:right
void QuestBoardSelectMenuController::AddIndex(int direction)
{
	if (direction & static_cast<unsigned int>(QuestBoardUIController::InputDirection::Up))
		_index = static_cast<SelectMenuOption>(static_cast<int>(_index) - 1);
	else if (direction & static_cast<unsigned int>(QuestBoardUIController::InputDirection::Down))
		_index = static_cast<SelectMenuOption>(static_cast<int>(_index) + 1);
	else
		return;
	// indexの制限
	if (_index < SelectMenuOption::NewOrderOption)
		_index = SelectMenuOption::AcceptOption;
	else if (_index >= SelectMenuOption::MaxOption)
		_index = SelectMenuOption::NewOrderOption;
	auto spriteRenderer = _spriteRenderer.lock();
	if (!spriteRenderer)
		return;

	// 背景位置更新
	Vector2 position{};
	switch (_index)
	{
	case QuestBoardSelectMenuController::SelectMenuOption::NewOrderOption:
		position = spriteRenderer->GetRectTransform(NewOrder).GetWorldPosition();
		break;
	case QuestBoardSelectMenuController::SelectMenuOption::AcceptOption:
		position = spriteRenderer->GetRectTransform(AcceptMenu).GetWorldPosition();
		break;
	}
	spriteRenderer->GetRectTransform(FrontSpr).SetLocalPosition(position);
}

// リセット
void QuestBoardSelectMenuController::Reset()
{
	_index = SelectMenuOption::NewOrderOption;
	if (auto spriteRenderer = _spriteRenderer.lock())
	{
		spriteRenderer->Update(0.0f);
		// 背景位置更新
		Vector2 position = spriteRenderer->GetRectTransform(NewOrder).GetWorldPosition();
		spriteRenderer->GetRectTransform(FrontSpr).SetLocalPosition(position);
	}
}

// 前の状態へ戻る
// メニューを閉じる場合はtrueを返す
bool QuestBoardSelectMenuController::PreviousState()
{
    return true;
}
