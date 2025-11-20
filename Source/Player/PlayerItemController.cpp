#include "PlayerItemController.h"

#include "PlayerController.h"
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
	_playerController = GetActor()->GetComponent<PlayerController>();
	_userDataManager = ResourceManager::Instance().GetResourceAs<UserDataManager>("UserDataManager");
}

// 更新処理
void PlayerItemController::Update(float elapsedTime)
{
	auto itemUIController = _itemUIController.lock();
	if (!itemUIController)
		return;

	itemUIController->SetCurrentIndex(_currentIndex);

	if (_function)
	{
		switch (_function->GetState())
		{
		case ItemFunctionBase::State::Execute:
			_function->Execute(elapsedTime);
			break;
		case ItemFunctionBase::State::End:
			_function->End();
			_function = nullptr;
			if (auto playerController = _playerController.lock())
			{
				playerController->SetIsAbleToUseItem(true);
			}
			break;
		}
	}
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
bool PlayerItemController::Use()
{
	auto userDataManager = _userDataManager.lock();
	if (!userDataManager)
		return false;
	_function = userDataManager->UseItem(_currentIndex, GetActor().get());

	return _function != nullptr;
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
}
