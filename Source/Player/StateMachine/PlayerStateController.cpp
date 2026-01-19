#include "PlayerStateController.h"

#include "../../Library/Component/Animator.h"
#include "../PlayerController.h"
#include "../../Library/Math/Random.h"
#include "../../Library/Algorithm/Converter.h"

#include "States/PlayerNonCombatStates.h"
#include "States/GreatSword/PlayerGreatSwordStates.h"

#include "../../Source/InGame/InGameCanvasActor.h"
#include "../../Source/Stage/SafetyZone.h"

#include <imgui.h>

// オブジェクトとの接触時の処理
void PlayerStateController::OnContact(CollisionData& collisionData)
{
	// セーフティゾーンと接触したか確認
	auto safetyZone = collisionData.other->GetComponent<SafetyZone>();
	if (safetyZone)
	{
		// 抜刀しているなら納刀状態に移行
		if (IsCombatState() && GetStateName() != "ToNonCombat")
		{
			ChangeState("ToNonCombat");
		}
	}
}

// 抜刀移行
void PlayerStateController::ChangeToCombatState(const std::string& mainStateName)
{
	_isCombatState = true;
	ChangeState(mainStateName);
}

// 納刀移行
void PlayerStateController::ChangeToNonCombatState(const std::string& mainStateName)
{
	_isCombatState = false;
	ChangeState(mainStateName);
}

/// 移動方向に向く
void PlayerStateController::RotationMovement(float elapsedTime, float rotationSpeed)
{
	auto charactorController = _player->GetActor()->GetComponent<CharactorController>();
	if (charactorController == nullptr)
		return;
	charactorController->UpdateRotation(elapsedTime, _player->GetMovement() * rotationSpeed);
}

// ステートマシン取得
std::shared_ptr<PlayerStateController::SM> PlayerStateController::GetStateMachine()
{
	return _isCombatState
		? _combatStateMachine
		: _stateMachine;
}

// 汎用遷移
void PlayerStateController::ChangeItemState()
{
	// アイテムの種類で遷移先を変更
	switch (GetItemController()->GetCurrentItemType())
	{
	case ItemType::Drinkable:
		// アイテム使用
		if (GetItemController()->Use())
			ChangeState("Drink");
		break;
	case ItemType::Useable:

		break;
	case ItemType::GrindingStone:
		// 砥石使用
		if (GetItemController()->Use())
			ChangeState("Grind");
		break;
	}
}

#pragma region 武器種ごとのステートマシン設定
void PlayerStateController::SetGreatSwordStateMachine()
{
	_combatStateMachine = std::make_shared<SM>();

	// ステート設定
	_combatStateMachine->RegisterState(std::make_shared<PlayerGreatSwordIdleState>(this));
	_combatStateMachine->RegisterState(std::make_shared<PlayerGreatSwordRunState>(this));
	_combatStateMachine->RegisterState(std::make_shared<PlayerGreatSwordEvadeState>(this));
	_combatStateMachine->RegisterState(std::make_shared<PlayerGreatSwordAttack1State>(this));
	_combatStateMachine->RegisterState(std::make_shared<PlayerGreatSwordAttack2State>(this));
	_combatStateMachine->RegisterState(std::make_shared<PlayerGreatSwordGuardState>(this));
	_combatStateMachine->RegisterState(std::make_shared<PlayerGreatSwordHitState>(this));
	_combatStateMachine->RegisterState(std::make_shared<PlayerGreatSwordHitKnockDownState>(this));
	_combatStateMachine->RegisterState(std::make_shared<PlayerGreatSwordDownState>(this));
	_combatStateMachine->RegisterState(std::make_shared<PlayerGreatSwordToNonCombatState>(this));
}
#pragma endregion

// 開始時処理
void PlayerStateController::OnStart()
{
	// コンポーネント取得
	_player				= GetActor()->GetComponent<PlayerController>().get();
	_animator			= GetActor()->GetComponent<Animator>().get();
	_effect = GetActor()->GetComponent<EffectController>().get();
	_damageSender		= GetActor()->GetComponent<DamageSender>().get();
	_staminaController	= GetActor()->GetComponent<StaminaController>().get();
	_itemController		= GetActor()->GetComponent<PlayerItemController>().get();

	// 納刀時ステート設定
	_stateMachine->RegisterState(std::make_shared<PlayerNonCombatIdleState>(this));
	_stateMachine->RegisterState(std::make_shared<PlayerNonCombatTurnState>(this));
	_stateMachine->RegisterState(std::make_shared<PlayerNonCombatWalkState>(this));
	_stateMachine->RegisterState(std::make_shared<PlayerNonCombatRunState>(this));
	_stateMachine->RegisterState(std::make_shared<PlayerNonCombatEvadeState>(this));
	_stateMachine->RegisterState(std::make_shared<PlayerNonCombatToCombatState>(this));
	_stateMachine->RegisterState(std::make_shared<PlayerNonCombatHitState>(this));
	_stateMachine->RegisterState(std::make_shared<PlayerNonCombatHitKnockDownState>(this));
	_stateMachine->RegisterState(std::make_shared<PlayerNonCombatDownState>(this));
	_stateMachine->RegisterState(std::make_shared<PlayerNonCombatDeathState>(this));
	_stateMachine->RegisterState(std::make_shared<PlayerNonCombatDrinkState>(this));
	_stateMachine->RegisterState(std::make_shared<PlayerNonCombatFatigueState>(this));
	_stateMachine->RegisterState(std::make_shared<PlayerNonCombatGrindState>(this));

    // InGameアクターからOperateUIController取得
    auto canvasActor = _player->GetActor()->GetScene()->GetActorManager().FindByClass<InGameCanvasActor>(ActorTag::UI);
    for (auto& child : canvasActor->GetChildren())
    {
        auto operateUIController = child->GetComponent<OperateUIController>();
        if (operateUIController)
        {
            _operateUIController = operateUIController.get();
            break;
        }
    }

    // ルートモーション設定
    _animator->SetIsUseRootMotion(false);
    _animator->SetRootNodeIndex("root");
    _animator->SetRootMotionOption(Animator::RootMotionOption::None);
    // 部分アニメーション用パラメータ設定
    _animator->SetPartialAnimationMask("spine_01");

    // 初期ステート設定
	ChangeState("Idle");
}

// 遅延更新処理
void PlayerStateController::OnLateUpdate(float elapsedTime)
{
	if (_player->CallInvisivleEvent() && !_player->OldInvisibleEvent())
	{
		// 無敵状態に入る
		_player->GetDamageable()->SetInvisible(true);
	}
	else if (!_player->CallInvisivleEvent() && _player->OldInvisibleEvent())
	{
		// 無敵状態から抜ける
		_player->GetDamageable()->SetInvisible(false);
	}
}

// GUI描画
void PlayerStateController::OnDrawGui()
{
	ImGui::Checkbox(u8"抜刀状態", &_isCombatState);
	if (auto stateMachine = GetStateMachine())
	{
		std::string str = u8"現在のステート:";
		str += stateMachine->GetStateName();
		ImGui::Text("%s", str.c_str());
		str = u8"現在のサブステート:";
		str += stateMachine->GetSubStateName();
		ImGui::Text("%s", str.c_str());
	}
	ImGui::Separator();
	
	if (ImGui::TreeNode(u8"納刀時ステートマシン"))
	{
		for (auto& [name, state] : _stateMachine->GetStateMap())
		{
			if (ImGui::TreeNode(name.c_str()))
			{
				state->DrawGui();
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}
	ImGui::Separator();

	if (_combatStateMachine)
	{
		if (ImGui::TreeNode(u8"抜刀時ステートマシン"))
		{
			for (auto& [name, state] : _combatStateMachine->GetStateMap())
			{
				if (ImGui::TreeNode(name.c_str()))
				{
					state->DrawGui();
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
	}
}

#pragma region ベースステート
void PlayerAnimationHSB::OnEnter()
{
	_owner->GetAnimator()->PlayAnimation(_animationName, _isLoop, _blendSeconds);
	_owner->GetAnimator()->SetIsUseRootMotion(_isUsingRootMotion);
}

Player8WayHSB::Player8WayHSB(PlayerStateController* owner,
	std::vector<std::string> animationNames,
	float blendSeconds,
	bool isUsingRootMotion) :
	HierarchicalStateBase(owner)
{
	// 要素数チェック
	assert(animationNames.size() == Direction::NumDirections);

	// サブステート登録
	for (size_t i = 0; i < Direction::NumDirections; ++i)
	{
		RegisterSubState(std::make_shared<PlayerSSB>(
			owner,
			ToString<Direction>(i),
			animationNames[i],
			blendSeconds,
			false,
			isUsingRootMotion));
	}
}

void Player8WayHSB::ChangeSubState(Direction animationIndex)
{
	HierarchicalStateBase<PlayerStateController>::ChangeSubState(ToString<Direction>(animationIndex));
}

void PlayerSSB::OnEnter()
{
	_owner->GetAnimator()->SetIsUseRootMotion(_isUsingRootMotion);
	_owner->GetAnimator()->PlayAnimation(_animationName, _isLoop, _blendSeconds);
}
#pragma endregion