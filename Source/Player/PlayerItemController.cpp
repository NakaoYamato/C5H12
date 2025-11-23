#include "PlayerItemController.h"

#include "PlayerController.h"
#include "../../Library/Scene/Scene.h"
#include "../../Source/InGame/InGameCanvasActor.h"
#include <imgui.h>

// 開始処理
void PlayerItemController::Start()
{
	// アイテムUI、操作UI取得
	auto canvasActor = GetActor()->GetScene()->GetActorManager().FindByClass<InGameCanvasActor>(ActorTag::UI);
	for (auto& child : canvasActor->GetChildren())
	{
		auto itemUIController = child->GetComponent<ItemUIController>();
		if (itemUIController)
		{
			_itemUIController = itemUIController;
			continue;
		}
		auto operateUIController = child->GetComponent<OperateUIController>();
		if (operateUIController)
		{
			_operateUIController = operateUIController;
			continue;
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
    auto operateUIController = _operateUIController.lock();
    if (!operateUIController)
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

    // 操作UIに説明文を追加
	if (itemUIController->IsOpen())
	{
		{
			if (Input::Instance().GetCurrentInputDevice() == Input::InputType::XboxPad)
			{
				std::string description = "アイテム選択";
				std::vector<std::string> inputActionNames;
				inputActionNames.push_back("ItemPrevSlide");
				inputActionNames.push_back("ItemNextSlide");
				operateUIController->AddDescription(description, inputActionNames);
			}
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
