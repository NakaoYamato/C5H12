#include "ChestUIController.h"

#include "../../Library/Actor/UI/UIActor.h"
#include "../../Library/Scene/Scene.h"

#include "../../Source/Player/PlayerActor.h"

#include <imgui.h>

// 開始処理
void ChestUIController::Start()
{
	for (auto& child : GetActor()->GetChildren())
	{
		if (child->GetComponent<ChestSelectMenuController>())
		{
			_selectUI = child->GetComponent<ChestSelectMenuController>();
            _selectUI.lock()->GetActor()->SetIsActive(false);
            continue;
		}
		if (child->GetComponent<ChestArmorMenuController>())
		{
            _armorUI = child->GetComponent<ChestArmorMenuController>();
            _armorUI.lock()->GetActor()->SetIsActive(false);
            continue;
		}
		if (child->GetComponent<ChestItemMenuController>())
		{
            _itemUI = child->GetComponent<ChestItemMenuController>();
            _itemUI.lock()->GetActor()->SetIsActive(false);
            continue;
		}
	}

	for (auto& child : GetActor()->GetScene()->GetMainCameraActor()->GetChildren())
	{
		auto changeArmorCamera = child->GetComponent<ChangeArmorCamera>();
		if (changeArmorCamera)
		{
			_changeArmorCamera = changeArmorCamera;
			break;
		}
	}
}

// 更新処理
void ChestUIController::Update(float elapsedTime)
{
    switch (_state)
    {
    case ChestUIController::SelectMenu:
        SelectInput();
        break;
    case ChestUIController::ItemMenu:
        ItemMenuInput();
        break;
    case ChestUIController::ArmorMenu:
        ArmorMenuInput();
        break;

    case ChestUIController::ToArmorMenu:
        UpdateToArmorMenu(elapsedTime);
        break;
    case ChestUIController::FromArmorMenu:
        UpdateFromArmorMenu(elapsedTime);
        break;
    }

    _inputState = InputState::None;
}

// GUI描画
void ChestUIController::DrawGui()
{
}

// ステート初期化
void ChestUIController::InitState()
{
    _state = State::SelectMenu;
}

// チェストUIを開く
void ChestUIController::Open()
{
    this->GetActor()->SetIsActive(true);
    InitState();
    if (auto selectUI = _selectUI.lock())
    {
        selectUI->GetActor()->SetIsActive(true);
		selectUI->ResetIndex();
    }
    if (auto armorUI = _armorUI.lock())
    {
        armorUI->Reset();
    }
	if (auto itemUI = _itemUI.lock())
	{
		itemUI->Reset();
	}

	// プレイヤーを探す
    auto& players = GetActor()->GetScene()->GetActorManager().FindByTag(ActorTag::Player);
    for (auto& player : players)
    {
        auto playerActor = std::dynamic_pointer_cast<PlayerActor>(player);
        if (playerActor && playerActor->IsUserControlled())
        {
			_playerActor = playerActor;
			break;
        }
    }
}

// チェストUIを閉じる
void ChestUIController::Close()
{
    InitState();
	if (auto selectUI = _selectUI.lock())
        selectUI->GetActor()->SetIsActive(false);
	if (auto armorUI = _armorUI.lock())
		armorUI->GetActor()->SetIsActive(false);
	if (auto itemUI = _itemUI.lock())
		itemUI->GetActor()->SetIsActive(false);

	this->GetActor()->SetIsActive(false);

	_chestActor.reset();
}

// Selectメニューの入力処理
void ChestUIController::SelectInput()
{
    auto selectUI = _selectUI.lock();
    auto itemUI = _itemUI.lock();
    if (!selectUI || !itemUI)
        return;

    switch (_inputState)
    {
    case ChestUIController::InputState::Up:
        selectUI->AddIndex(-1);
        break;
    case ChestUIController::InputState::Down:
        selectUI->AddIndex(1);
        break;
    case ChestUIController::InputState::Select:
        switch (selectUI->GetSelectIndex())
        {
        case ChestSelectMenuController::SelectMenuOption::ItemOption:
            selectUI->GetActor()->SetIsActive(false);
            itemUI->GetActor()->SetIsActive(true);

            _state = ChestUIController::ItemMenu;
            break;
        case ChestSelectMenuController::SelectMenuOption::ArmorOption:
            GetActor()->GetScene()->GetFade()->Start(Fade::Type::FadeOut, _fadeTime);
            selectUI->GetActor()->SetIsActive(false);

            _state = ChestUIController::ToArmorMenu;
            break;
        }
        break;
    case ChestUIController::InputState::Back:
        // UIを閉じる
        Close();
        break;
    }
}

// Armorメニューの入力処理
void ChestUIController::ArmorMenuInput()
{
    auto armorUI = _armorUI.lock();
    if (!armorUI)
        return;

    switch (_inputState)
    {
    case ChestUIController::InputState::Up:
        if (armorUI->GetState() == ChestArmorMenuController::State::SelectArmor)
            armorUI->AddSelectArmorRowIndex(-1);
        else
            armorUI->AddIndex(-1);
        break;
    case ChestUIController::InputState::Down:
        if (armorUI->GetState() == ChestArmorMenuController::State::SelectArmor)
            armorUI->AddSelectArmorRowIndex(+1);
        else
            armorUI->AddIndex(1);
        break;
    case ChestUIController::InputState::Left:
        if (armorUI->GetState() == ChestArmorMenuController::State::SelectArmor)
            armorUI->AddSelectArmorColumnIndex(-1);
        break;
    case ChestUIController::InputState::Right:
        if (armorUI->GetState() == ChestArmorMenuController::State::SelectArmor)
            armorUI->AddSelectArmorColumnIndex(+1);
        break;
    case ChestUIController::InputState::Select:
        armorUI->NextState();
        break;
    case ChestUIController::InputState::Back:
        if (armorUI->PreviousState())
        {
            GetActor()->GetScene()->GetFade()->Start(Fade::Type::FadeOut, _fadeTime);
            armorUI->GetActor()->SetIsActive(false);

            _state = ChestUIController::FromArmorMenu;
        }
        break;
    }
}

// Itemメニューの入力処理
void ChestUIController::ItemMenuInput()
{
    auto itemUI = _itemUI.lock();
    auto selectUI = _selectUI.lock();
    if (!itemUI || !selectUI)
        return;

    switch (_inputState)
    {
    case ChestUIController::InputState::Up:
        if (itemUI->GetState() == ChestItemMenuController::State::Pourch)
			itemUI->AddIndex(-1);
        else
            itemUI->AddIndex(-1);
        break;
    case ChestUIController::InputState::Down:
        if (itemUI->GetState() == ChestItemMenuController::State::Pourch)
            itemUI->AddIndex(-1);
        else
            itemUI->AddIndex(+1);
        break;
    case ChestUIController::InputState::Left:
        itemUI->AddIndex(-1);
        break;
    case ChestUIController::InputState::Right:
        itemUI->AddIndex(+1);
        break;
    case ChestUIController::InputState::Select:
        itemUI->NextState();
        break;
    case ChestUIController::InputState::Back:
        if (itemUI->PreviousState())
        {
            itemUI->GetActor()->SetIsActive(false);
            selectUI->GetActor()->SetIsActive(true);

            _state = ChestUIController::SelectMenu;
        }
        break;
    }
}

void ChestUIController::UpdateToArmorMenu(float elapsedTime)
{
    if (!GetActor()->GetScene()->GetFade()->IsFading())
    {
        GetActor()->GetScene()->GetFade()->Start(Fade::Type::FadeIn, _fadeTime);
        auto armorUI = _armorUI.lock();
        armorUI->GetActor()->SetIsActive(true);
        // プレイヤーの装備コントローラー設定
		if (auto player = _playerActor.lock())
		{
			auto playerEquipmentController = player->GetComponent<PlayerEquipmentController>();
			armorUI->SetPlayerEquipmentController(playerEquipmentController);
		}

        auto chestActor = _chestActor.lock();

        _state = ChestUIController::ArmorMenu;

        // プレイヤーをチェストの前に移動、回転させる
		if (auto player = _playerActor.lock())
		{
            float positionY = player->GetTransform().GetWorldPosition().y;
            player->GetTransform().SetPosition(
                chestActor->GetTransform().GetWorldPosition() - chestActor->GetTransform().GetAxisZ());
            player->GetTransform().SetPositionY(positionY + 0.1f);
            player->GetTransform().SetAngleY(chestActor->GetTransform().GetAngle().y + DirectX::XMConvertToRadians(180.0f));
		}
		// カメラをチェスト用に切り替え
		if (auto changeArmorCamera = _changeArmorCamera.lock())
		{
			changeArmorCamera->Swich();
			changeArmorCamera->SetChestActor(chestActor);
		}
    }
}

void ChestUIController::UpdateFromArmorMenu(float elapsedTime)
{
    if (!GetActor()->GetScene()->GetFade()->IsFading())
    {
        GetActor()->GetScene()->GetFade()->Start(Fade::Type::FadeIn, _fadeTime);
        // カメラを元に戻す
        GetActor()->GetScene()->GetMainCameraActor()->SwitchPreviousController();

        auto selectUI = _selectUI.lock();
        selectUI->GetActor()->SetIsActive(true);

        _state = ChestUIController::SelectMenu;
    }
}
