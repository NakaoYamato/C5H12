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
        break;
    case ChestUIController::ItemMenu:
        break;
    case ChestUIController::ArmorMenu:
        break;

    case ChestUIController::ToArmorMenu:
        UpdateToArmorMenu(elapsedTime);
        break;
    case ChestUIController::FromArmorMenu:
        UpdateFromArmorMenu(elapsedTime);
        break;
    }
}

// 3D描画後の描画処理
void ChestUIController::DelayedRender(const RenderContext& rc)
{
    auto& textRenderer = GetActor()->GetScene()->GetTextRenderer();


    switch (_state)
    {
    case ChestUIController::SelectMenu:
        break;
    case ChestUIController::ItemMenu:
        break;
    case ChestUIController::ArmorMenu:
        break;
    default:
        break;
    }
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

// インデックス追加
void ChestUIController::AddIndex(int val)
{
    auto selectUI = _selectUI.lock();
    auto armorUI = _armorUI.lock();
    if (!selectUI || !armorUI)
        return;

    switch (_state)
    {
    case ChestUIController::SelectMenu:
        selectUI->AddIndex(val);
        break;
    case ChestUIController::ItemMenu:
        break;
    case ChestUIController::ArmorMenu:
        armorUI->AddIndex(val);
        break;
    default:
        break;
    }
}

// 次へ進む
void ChestUIController::NextState()
{
	auto selectUI = _selectUI.lock();
	auto armorUI = _armorUI.lock();
	if (!selectUI || !armorUI)
		return;

    switch (_state)
    {
    case ChestUIController::SelectMenu:
        switch (selectUI->GetSelectIndex())
        {
        case ChestSelectMenuController::SelectMenuOption::ItemOption:
            break;
        case ChestSelectMenuController::SelectMenuOption::ArmorOption:
            GetActor()->GetScene()->GetFade()->Start(Fade::Type::FadeOut, _fadeTime);
            selectUI->GetActor()->SetIsActive(false);

			_state = ChestUIController::ToArmorMenu;
            break;
        case ChestSelectMenuController::SelectMenuOption::TestOption:
            break;
        }
        break;
    case ChestUIController::ItemMenu:
        break;
    case ChestUIController::ArmorMenu:
        break;
    }
}

// 前の状態へ戻る
void ChestUIController::PreviousState()
{
    auto selectUI = _selectUI.lock();
    auto armorUI = _armorUI.lock();
    if (!selectUI || !armorUI)
        return;

    switch (_state)
    {
    case ChestUIController::SelectMenu:
        // UIを閉じる
        Close();
        break;
    case ChestUIController::ItemMenu:
        break;
    case ChestUIController::ArmorMenu:
        GetActor()->GetScene()->GetFade()->Start(Fade::Type::FadeOut, _fadeTime);
        armorUI->GetActor()->SetIsActive(false);

        _state = ChestUIController::FromArmorMenu;
        break;
    }
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
}

// チェストUIを閉じる
void ChestUIController::Close()
{
    InitState();
	if (auto selectUI = _selectUI.lock())
        selectUI->GetActor()->SetIsActive(false);
	if (auto armorUI = _armorUI.lock())
		armorUI->GetActor()->SetIsActive(false);

	this->GetActor()->SetIsActive(false);

	_chestActor.reset();
}

void ChestUIController::UpdateToArmorMenu(float elapsedTime)
{
    if (!GetActor()->GetScene()->GetFade()->IsFading())
    {
        GetActor()->GetScene()->GetFade()->Start(Fade::Type::FadeIn, _fadeTime);
        auto armorUI = _armorUI.lock();
        armorUI->GetActor()->SetIsActive(true);
        auto chestActor = _chestActor.lock();

        _state = ChestUIController::ArmorMenu;

        // プレイヤーをチェストの前に移動、回転させる
        auto& players = GetActor()->GetScene()->GetActorManager().FindByTag(ActorTag::Player);
        for (auto& player : players)
        {
            auto playerActor = std::dynamic_pointer_cast<PlayerActor>(player);
            if (playerActor && playerActor->IsUserControlled())
            {
                playerActor->GetTransform().SetPosition(
                    chestActor->GetTransform().GetWorldPosition() - chestActor->GetTransform().GetAxisZ());
                playerActor->GetTransform().SetAngleY(chestActor->GetTransform().GetAngle().y + DirectX::XMConvertToRadians(180.0f));
            }
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
