#include "PlayerStateMachine.h"

#include "../../Library/Component/Animator.h"
#include "../PlayerController.h"
#include "../../Library/Math/Random.h"
#include "../../Library/Algorithm/Converter.h"

#include "PlayerNonCombatStates.h"
#include "PlayerGreatSwordStates.h"

#include <imgui.h>

PlayerStateMachine::PlayerStateMachine(Actor* owner)
{
    // コンポーネント取得
	_player = owner->GetComponent<PlayerController>().get();
	_animator = owner->GetComponent<Animator>().get();
	_effect = owner->GetComponent<EffectController>().get();
	_damageSender = owner->GetComponent<DamageSender>().get();

    // ステート設定
	_stateMachine.RegisterState(std::make_shared<PlayerNonCombatIdleState>(this));
	_stateMachine.RegisterState(std::make_shared<PlayerNonCombatTurnState>(this));
	_stateMachine.RegisterState(std::make_shared<PlayerNonCombatWalkState>(this));
	_stateMachine.RegisterState(std::make_shared<PlayerNonCombatRunState>(this));
	_stateMachine.RegisterState(std::make_shared<PlayerNonCombatEvadeState>(this));
	_stateMachine.RegisterState(std::make_shared<PlayerNonCombatToCombatState>(this));
    _stateMachine.RegisterState(std::make_shared<PlayerNonCombatHitState>(this));
    _stateMachine.RegisterState(std::make_shared<PlayerNonCombatHitKnockDownState>(this));
	_stateMachine.RegisterState(std::make_shared<PlayerNonCombatDownState>(this));
	_stateMachine.RegisterState(std::make_shared<PlayerNonCombatDeathState>(this));

    _stateMachine.RegisterState(std::make_shared<PlayerGreatSwordIdleState>(this));
    _stateMachine.RegisterState(std::make_shared<PlayerGreatSwordRunState>(this));
    _stateMachine.RegisterState(std::make_shared<PlayerGreatSwordEvadeState>(this));
    _stateMachine.RegisterState(std::make_shared<PlayerGreatSwordAttack1State>(this));
    _stateMachine.RegisterState(std::make_shared<PlayerGreatSwordAttack2State>(this));
    _stateMachine.RegisterState(std::make_shared<PlayerGreatSwordGuardState>(this));
    _stateMachine.RegisterState(std::make_shared<PlayerGreatSwordHitState>(this));
    _stateMachine.RegisterState(std::make_shared<PlayerGreatSwordHitKnockDownState>(this));
    _stateMachine.RegisterState(std::make_shared<PlayerGreatSwordDownState>(this));
    _stateMachine.RegisterState(std::make_shared<PlayerGreatSwordToNonCombatState>(this));
}

// 開始処理
void PlayerStateMachine::Start()
{
    // ルートモーション設定
    _animator->SetIsUseRootMotion(false);
    _animator->SetRootNodeIndex("root");
    _animator->SetRootMotionOption(Animator::RootMotionOption::None);
    // 初期ステート設定
    _stateMachine.ChangeState("Idle");
}

// 実行処理
void PlayerStateMachine::Execute(float elapsedTime)
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

    // 死亡処理
	if (_player->IsDead() && _stateMachine.GetStateName() != "Death")
	{
		_stateMachine.ChangeState("Death");
	}

    _stateMachine.Update(elapsedTime);
}

// Gui描画
void PlayerStateMachine::DrawGui()
{
    if (ImGui::Button(u8"死亡"))
    {
        _player->SetIsDead(true);
    }

    if (ImGui::TreeNode(u8"ステートマシン"))
    {
        std::string str = u8"現在のステート:";
        str += _stateMachine.GetState()->GetName();
        ImGui::Text("%s", str.c_str());
        str = u8"現在のサブステート:";
        str += _stateMachine.GetState()->GetSubStateName();
        ImGui::Text("%s", str.c_str());

        ImGui::Separator();
        for (auto& [name, state] : _stateMachine.GetStateMap())
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

void PlayerStateMachine::RotationMovement(float elapsedTime, float rotationSpeed)
{
    auto charactorController = _player->GetActor()->GetComponent<CharactorController>();
    if (charactorController == nullptr)
        return;
    charactorController->UpdateRotation(elapsedTime, _player->GetMovement() * rotationSpeed);
}

// ステート変更
void PlayerStateMachine::ChangeState(const char* mainStateName, const char* subStateName)
{
    // 遷移先が無効な場合は何もしない
    if (mainStateName == nullptr || mainStateName[0] == '\0')
        return;

    std::string currentMainState = GetStateName();
    std::string currentSubState = GetSubStateName();

    // 現在のステートと変更先が違うなら変更
    if (currentMainState != mainStateName)
        _stateMachine.ChangeState(mainStateName);
    // サブステートがあるなら変更
    if (subStateName != nullptr && subStateName[0] != '\0')
    {
        // 現在のサブステートと変更先が違うなら変更
        if (currentSubState != subStateName)
            _stateMachine.ChangeSubState(subStateName);
    }
}

// ステート名取得
const char* PlayerStateMachine::GetStateName()
{
	if (!_stateMachine.GetState())
        return nullptr;
    return _stateMachine.GetState()->GetName();
}

const char* PlayerStateMachine::GetSubStateName()
{
    if (!_stateMachine.GetState())
        return nullptr;
    return _stateMachine.GetState()->GetSubStateName();
}

#pragma region ベースステート
void PlayerHSB::OnEnter()
{
    _owner->GetAnimator()->PlayAnimation(_animationName, _isLoop, _blendSeconds);
    _owner->GetAnimator()->SetIsUseRootMotion(_isUsingRootMotion);
}


Player8WayHSB::Player8WayHSB(PlayerStateMachine* stateMachine,
    std::vector<std::string> animationNames,
    float blendSeconds,
    bool isUsingRootMotion) :
    HierarchicalStateBase(stateMachine)
{
    // 要素数チェック
    assert(animationNames.size() == Direction::NumDirections);

    // サブステート登録
    for (size_t i = 0; i < Direction::NumDirections; ++i)
    {
        RegisterSubState(std::make_shared<PlayerSSB>(
            stateMachine,
            ToString<Direction>(i),
            animationNames[i],
            blendSeconds,
            false,
            isUsingRootMotion));
    }
}

void Player8WayHSB::ChangeSubState(Direction animationIndex)
{
    HierarchicalStateBase<PlayerStateMachine>::ChangeSubState(ToString<Direction>(animationIndex));
}

void PlayerSSB::OnEnter()
{
    _owner->GetAnimator()->SetIsUseRootMotion(_isUsingRootMotion);
    _owner->GetAnimator()->PlayAnimation(_animationName, _isLoop, _blendSeconds);
}
#pragma endregion
