#include "PlayerGreatSwordStates.h"

#include "../../Library/Component/Animator.h"
#include "../PlayerController.h"
#include "../../Library/Algorithm/Converter.h"

#pragma region ベースステート
void PlayerHSB::OnEnter()
{
    _owner->GetAnimator()->PlayAnimation(_animationName, _isLoop, _blendSeconds);
    _owner->GetAnimator()->SetIsUseRootMotion(_isUsingRootMotion);
}

class Player8WaySSB : public StateBase<PlayerStateMachine>
{
public:
    Player8WaySSB(PlayerStateMachine* stateMachine,
        const std::string& name,
        const std::string& animationName,
        float blendSeconds,
        bool isUsingRootMotion) : 
        StateBase(stateMachine),
        _name(name),
        _animationName(animationName),
        _blendSeconds(blendSeconds),
        _isUsingRootMotion(isUsingRootMotion)
    {
    }
    const char* GetName() const override { return _name.c_str(); }
    void OnEnter() override
    {
        _owner->GetAnimator()->SetIsUseRootMotion(_isUsingRootMotion);
        _owner->GetAnimator()->PlayAnimation(_animationName, false, _blendSeconds);
    }
    void OnExecute(float elapsedTime) override
    {
    }
    void OnExit() override
    {
    }
private:
    std::string _name = "";
    std::string _animationName = "";
    float		_blendSeconds = 0.2f;
    bool		_isUsingRootMotion = false;
};

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
        RegisterSubState(std::make_shared<Player8WaySSB>(
            stateMachine,
            ToString<Direction>(i),
            animationNames[i],
            blendSeconds, 
            isUsingRootMotion));
    }
}

void Player8WayHSB::ChangeSubState(Direction animationIndex)
{
    HierarchicalStateBase<PlayerStateMachine>::ChangeSubState(ToString<Direction>(animationIndex));
}

#pragma endregion

#pragma region 待機
void PlayerGreatSwordIdleState::OnExecute(float elapsedTime)
{
    // 攻撃遷移
    if (_owner->GetPlayer()->IsAttack())
        _owner->GetStateMachine().ChangeState("Attack1");
    // 移動
    else if (_owner->GetPlayer()->IsMoving())
        _owner->GetStateMachine().ChangeState("GreatSwordWalk");
    // 回避移行
    else if (_owner->GetPlayer()->IsEvade())
        _owner->GetStateMachine().ChangeState("Evade");
    // ガード移行
    else if (_owner->GetPlayer()->IsGuard())
        _owner->GetStateMachine().ChangeState("Guard");
}
#pragma endregion

#pragma region 走り
namespace SprintSubState
{
    class SprintStartSubState : public StateBase<PlayerStateMachine>
    {
    public:
        SprintStartSubState(PlayerStateMachine* stateMachine) : StateBase(stateMachine)
        {
        }
        const char* GetName() const override { return "SprintStart"; }

        void OnEnter() override
        {
            _owner->GetAnimator()->SetIsUseRootMotion(true);
            _owner->GetAnimator()->PlayAnimation(u8"RunCombatStartF0", false, 0.2f);
        }
        void OnExecute(float elapsedTime) override
        {
            // 移動方向に向く
            _owner->RotationMovement(elapsedTime);
            // 攻撃移行
            if (_owner->GetPlayer()->IsAttack())
                _owner->GetStateMachine().ChangeSubState("SprintAttack");
            // 回避移行
            else if (_owner->GetPlayer()->IsEvade())
                _owner->GetStateMachine().ChangeState("Evade");
            // ガード移行
            else if (_owner->GetPlayer()->IsGuard())
                _owner->GetStateMachine().ChangeState("Guard");
            // アニメーションが終了していたら遷移
            else if (!_owner->GetAnimator()->IsPlayAnimation())
            {
                _owner->GetStateMachine().ChangeSubState("Sprinting");
            }
            // ダッシュ解除で移動に遷移
            else if (!_owner->GetPlayer()->IsDash())
            {
                _owner->GetStateMachine().ChangeState("Run");
            }
        }
        void OnExit() override
        {
        }
    };
    class SprintingSubState : public StateBase<PlayerStateMachine>
    {
    public:
        SprintingSubState(PlayerStateMachine* stateMachine) : StateBase(stateMachine)
        {
        }
        const char* GetName() const override { return "Sprinting"; }

        void OnEnter() override
        {
            _owner->GetAnimator()->SetIsUseRootMotion(true);
            _owner->GetAnimator()->PlayAnimation(u8"RunCombatLoopF0", true, 0.0f);
        }
        void OnExecute(float elapsedTime) override
        {
            // 移動方向に向く
            _owner->RotationMovement(elapsedTime);
            // 攻撃移行
            if (_owner->GetPlayer()->IsAttack())
                _owner->GetStateMachine().ChangeSubState("SprintAttack");
            // 回避移行
            else if (_owner->GetPlayer()->IsEvade())
                _owner->GetStateMachine().ChangeState("Evade");
            // ガード移行
            else if (_owner->GetPlayer()->IsGuard())
                _owner->GetStateMachine().ChangeState("Guard");
            // ダッシュ解除で移動に遷移
            else if (!_owner->GetPlayer()->IsDash())
                _owner->GetStateMachine().ChangeState("Run");
        }
        void OnExit() override
        {
        }
    };
    class SprintAttackSubState : public StateBase<PlayerStateMachine>
    {
    public:
        static constexpr float ATK = 1.0f;
    public:
        SprintAttackSubState(PlayerStateMachine* stateMachine) : StateBase(stateMachine)
        {
        }
        const char* GetName() const override { return "SprintAttack"; }

        void OnEnter() override
        {
            _owner->GetAnimator()->SetIsUseRootMotion(true);
            _owner->GetAnimator()->PlayAnimation(u8"RunAttack01", false, 0.2f);

            // 攻撃フラグを立てる
            _owner->GetPlayer()->SetBaseATK(ATK);

            // 先行入力遷移先をクリア
            _nextStateName = "";
        }
        void OnExecute(float elapsedTime) override
        {
            // 先行入力受付時
            if (_owner->GetPlayer()->CallInputBufferingEvent())
            {
                // 先行入力遷移先を設定
                // 攻撃
                if (_owner->GetPlayer()->IsAttack())
                    _nextStateName = "Attack1";
                // 回避移行
                else if (_owner->GetPlayer()->IsEvade())
                    _nextStateName = "Evade";
                // ガード移行
                else if (_owner->GetPlayer()->IsGuard())
                    _nextStateName = "Guard";
            }

            // アニメーションが終了していたら遷移
            if (!_owner->GetAnimator()->IsPlayAnimation())
                _owner->GetStateMachine().ChangeState("Idle");
            else if (_owner->GetPlayer()->CallCancelEvent())
            {
                // キャンセル待機中に入力があった場合は先行入力遷移
                if (!_nextStateName.empty())
                    _owner->GetStateMachine().ChangeState(_nextStateName);
            }
        }
        void OnExit() override
        {
        }
    private:
        std::string _nextStateName = "";
    };
}
PlayerGreatSwordRunState::PlayerGreatSwordRunState(PlayerStateMachine* stateMachine) :
    HierarchicalStateBase(stateMachine)
{
    // サブステート登録
    RegisterSubState(std::make_shared<SprintSubState::SprintStartSubState>(stateMachine));
    RegisterSubState(std::make_shared<SprintSubState::SprintingSubState>(stateMachine));
    RegisterSubState(std::make_shared<SprintSubState::SprintAttackSubState>(stateMachine));
}

void PlayerGreatSwordRunState::OnEnter()
{
    // フラグを立てる
    _owner->GetPlayer()->SetIsDash(true);
    // 初期サブステート設定
    ChangeSubState("SprintStart");
}
void PlayerGreatSwordRunState::OnExecute(float elapsedTime)
{
}
void PlayerGreatSwordRunState::OnExit()
{
    // フラグを下ろす
    _owner->GetPlayer()->SetIsDash(false);
}
#pragma endregion

#pragma region 回避
PlayerGreatSwordEvadeState::PlayerGreatSwordEvadeState(PlayerStateMachine* stateMachine) :
    Player8WayHSB(
        stateMachine, 
        { u8"RollCombatF0",
        u8"RollCombatFR45",
        u8"RollCombatR90",
        u8"RollCombatBR45",
        u8"RollCombatB180",
        u8"RollCombatBL45",
        u8"RollCombatL90",
        u8"RollCombatFL45", },
        0.2f, 
        true)
{
}

void PlayerGreatSwordEvadeState::OnEnter()
{
    // 入力方向から回避方向を決定
    Player8WayHSB::Direction directionType = Player8WayHSB::Direction::Front;
    Vector2 movement = _owner->GetPlayer()->GetMovement();

    // 入力方向が0なら前転
    if (movement.LengthSq() == 0.0f)
    {
        directionType = Player8WayHSB::Direction::Front;
    }
    else
    {
        // 入力方向とプレイヤーのY軸回転量から回避方向を決定
        float angle =
            DirectX::XMConvertToDegrees(
                atan2f(movement.x, movement.y)
                - _owner->GetPlayer()->GetActor()->GetTransform().GetRotation().y
            );
        // 角度を0~360度に正規化
        angle = fmodf(angle, 360.0f);
        if (angle < 0.0f)
            angle += 360.0f;
        // 360度を8方向に分割
        int index = (int)(angle / 45.0f + 0.5f);
        if (index >= 8)
            index = 0;
        directionType = static_cast<Player8WayHSB::Direction>(index);
    }
    ChangeSubState(directionType);
}
void PlayerGreatSwordEvadeState::OnExecute(float elapsedTime)
{
    // アニメーションが終了していたら遷移
    if (!_owner->GetAnimator()->IsPlayAnimation())
    {
        _owner->GetStateMachine().ChangeState("Idle");
    }
    else if (_owner->GetPlayer()->CallCancelEvent())
    {
        // 攻撃移行
        if (_owner->GetPlayer()->IsAttack())
            _owner->GetStateMachine().ChangeState("Attack1");
        // 移動移行
        else if (_owner->GetPlayer()->IsMoving())
            _owner->GetStateMachine().ChangeState("Run");
        // ガード移行
        else if (_owner->GetPlayer()->IsGuard())
            _owner->GetStateMachine().ChangeState("Guard");
    }
}
#pragma endregion

#pragma region 攻撃1
// 攻撃1サブステート
namespace Attack1SubState
{
    class ComboSubState : public StateBase<PlayerStateMachine>
    {
    public:
        static constexpr float ATK = 1.0f;
    public:
        ComboSubState(PlayerStateMachine* stateMachine,
            const std::string& name,
            const std::string& animationName,
            const std::string& nextSubStateName,
            float animationBlendTime,
            float ATK)
            : StateBase(stateMachine),
            _name(name),
            _animationName(animationName),
            _nextSubStateName(nextSubStateName),
            _animationBlendTime(animationBlendTime),
            _ATK(ATK)
        {
        }

        const char* GetName() const override { return _name.c_str(); }
        const char* GetNextStateName() const { return _nextSubStateName.c_str(); }

        void OnEnter() override
        {
            _owner->GetAnimator()->PlayAnimation(_animationName, false, _animationBlendTime);
            // 攻撃フラグを立てる
            _owner->GetPlayer()->SetBaseATK(_ATK);
        }
        void OnExecute(float elapsedTime) override
        {
        }
        void OnExit() override
        {
        }
    private:
        std::string _name;
        std::string _animationName;
        std::string _nextSubStateName;

        float _animationBlendTime = 0.2f;
        float _ATK = 1.0f;
    };
}
PlayerGreatSwordAttack1State::PlayerGreatSwordAttack1State(PlayerStateMachine* stateMachine) :
    HierarchicalStateBase(stateMachine)
{
    // サブステート登録
    RegisterSubState(std::make_shared<Attack1SubState::ComboSubState>(stateMachine,
        "ComboAttack1",
        "ComboAttack01_01",
        "ComboAttack2",
        0.3f,
        1.0f));
    RegisterSubState(std::make_shared<Attack1SubState::ComboSubState>(stateMachine,
        "ComboAttack2",
        "ComboAttack01_02",
        "ComboAttack3",
        0.3f,
        1.0f));
    RegisterSubState(std::make_shared<Attack1SubState::ComboSubState>(stateMachine,
        "ComboAttack3",
        "ComboAttack01_03",
        "ComboAttack4",
        0.3f,
        1.0f));
    RegisterSubState(std::make_shared<Attack1SubState::ComboSubState>(stateMachine,
        "ComboAttack4",
        "ComboAttack01_04",
        "",
        0.3f,
        1.0f));
}

void PlayerGreatSwordAttack1State::OnEnter()
{
    _owner->GetAnimator()->SetIsUseRootMotion(true);
    // 初期サブステート設定
    ChangeSubState("ComboAttack1");
    // 先行入力遷移先をクリア
    _nextStateName = "";
}

void PlayerGreatSwordAttack1State::OnExecute(float elapsedTime)
{
    // 先行入力受付時
    if (_owner->GetPlayer()->CallInputBufferingEvent())
    {
        // 先行入力遷移先を設定
        // 攻撃
        if (_owner->GetPlayer()->IsAttack())
            _nextStateName = "Attack";
        // 回避移行
        else if (_owner->GetPlayer()->IsEvade())
            _nextStateName = "Evade";
        // ガード移行
        else if (_owner->GetPlayer()->IsGuard())
            _nextStateName = "Guard";
    }

    // 攻撃キャンセル判定
    if (_owner->GetPlayer()->CallCancelAttackEvent())
    {
        if (_nextStateName == "Attack")
        {
            auto subState = dynamic_cast<Attack1SubState::ComboSubState*>(this->_subState);
            if (subState)
            {
                if (subState->GetNextStateName() != "")
                {
                    // キャンセル待機中に入力があった場合は先行入力遷移
                    _owner->GetStateMachine().ChangeSubState(subState->GetNextStateName());
                    _nextStateName = ""; // 遷移後はクリア
                    return;
                }
            }
        }
    }

    // キャンセル判定
    if (_owner->GetPlayer()->CallCancelEvent())
    {
        if (!_nextStateName.empty())
            _owner->GetStateMachine().ChangeState(_nextStateName);
    }
    else
    {
        // キャンセルがかかるまでの間は移動方向に向く
        _owner->RotationMovement(elapsedTime);
    }

    // アニメーションが終了していたら遷移
    if (!_owner->GetAnimator()->IsPlayAnimation())
    {
        // 攻撃からIdleに遷移
        _owner->GetStateMachine().ChangeState("Idle");
    }
}
#pragma endregion

#pragma region ガード
// ガードサブステート
namespace GuardSubState
{
    class GuardStartSubState : public StateBase<PlayerStateMachine>
    {
    public:
        GuardStartSubState(PlayerStateMachine* stateMachine) : StateBase(stateMachine)
        {
        }
        const char* GetName() const override { return "GuardStart"; }
        void OnEnter() override
        {
            _owner->GetAnimator()->PlayAnimation(u8"BlockStart", false, 2.0f);
        }
        void OnExecute(float elapsedTime) override
        {
            // アニメーションが終了していたら遷移
            if (!_owner->GetAnimator()->IsPlayAnimation())
                _owner->GetStateMachine().ChangeSubState("Guarding");
        }
        void OnExit() override
        {
        }
    };

    class GuardingSubState : public StateBase<PlayerStateMachine>
    {
    public:
        GuardingSubState(PlayerStateMachine* stateMachine) : StateBase(stateMachine)
        {
        }
        const char* GetName() const override { return "Guarding"; }
        void OnEnter() override
        {
            _owner->GetAnimator()->PlayAnimation(u8"BlockLoop", true, 0.2f);
        }
        void OnExecute(float elapsedTime) override
        {
        }
        void OnExit() override
        {
        }
    };
}

PlayerGreatSwordGuardState::PlayerGreatSwordGuardState(PlayerStateMachine* stateMachine) :
    HierarchicalStateBase(stateMachine)
{
}
void PlayerGreatSwordGuardState::OnEnter()
{
}
void PlayerGreatSwordGuardState::OnExecute(float elapsedTime)
{
}
void PlayerGreatSwordGuardState::OnExit()
{
}
#pragma endregion
