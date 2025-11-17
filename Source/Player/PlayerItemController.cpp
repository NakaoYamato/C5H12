#include "PlayerItemController.h"

#include "../../Library/Scene/Scene.h"
#include "../../Source/InGame/InGameCanvasActor.h"
#include <imgui.h>

// 開始処理
void PlayerItemController::Start()
{
	// アイテムUIコントローラー取得
	auto canvasActor = GetActor()->GetScene()->GetActorManager().FindByClass<InGameCanvasActor>(ActorTag::UI);
	for (auto& child : canvasActor->GetChildren())
	{
		auto itemUIController = child->GetComponent<ItemUIController>();
		if (itemUIController)
		{
			_itemUIController = itemUIController;
			break;
		}
	}
	_userDataManager = ResourceManager::Instance().GetResourceAs<UserDataManager>("UserDataManager");
}

// 更新処理
void PlayerItemController::Update(float elapsedTime)
{
	auto itemUIController = _itemUIController.lock();
	if (!itemUIController)
		return;

	itemUIController->SetCurrentIndex(_currentIndex);
}

// GUI描画
void PlayerItemController::DrawGui()
{
}

// 開く
void PlayerItemController::Open()
{
	if (auto itemUIController = _itemUIController.lock())
	{
		itemUIController->Open();
	}
}

// 閉じる
void PlayerItemController::Close()
{
	if (auto itemUIController = _itemUIController.lock())
	{
		itemUIController->Close();
	}
}

// 使う
void PlayerItemController::Use()
{
	auto userDataManager = _userDataManager.lock();
	if (!userDataManager)
		return;
	auto item = userDataManager->GetPouchItem(_currentIndex);

	if (item->itemIndex < 0)
		return;

	// 所持数があれば使用
	if (item->quantity > 0)
	{
		item->quantity--;

		// 0になったらアイテムを外す
		if (item->quantity == 0)
		{
			item->itemIndex = -1;
		}
	}
}

bool PlayerItemController::IsClosed() const
{
	if (auto itemUIController = _itemUIController.lock())
	{
		return itemUIController->IsClosed();
	}
	return false;
}

bool PlayerItemController::IsOpen() const
{
	if (auto itemUIController = _itemUIController.lock())
	{
		return itemUIController->IsOpen();
	}
	return false;
}

void PlayerItemController::AddIndex(int addIndex)
{
	_currentIndex += addIndex;
	// ポーチの整理
	if (auto userDataManager = _userDataManager.lock())
		userDataManager->SortPouchItems();
}
