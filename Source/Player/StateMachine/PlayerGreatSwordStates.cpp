#include "PlayerGreatSwordStates.h"

#include "../../Library/Component/Animator.h"
#include "../PlayerController.h"
#include "../../Library/Algorithm/Converter.h"

#pragma region 待機
void PlayerGreatSwordIdleState::OnExecute(float elapsedTime)
{
    // 攻撃遷移
    if (_owner->GetPlayer()->IsAttack())
        _owner->GetStateMachine().ChangeState("CombatAttack1");
    else if (_owner->GetPlayer()->IsSpecialAttack())
        _owner->GetStateMachine().ChangeState("CombatAttack2");
    // 移動
    else if (_owner->GetPlayer()->IsMoving())
        _owner->GetStateMachine().ChangeState("CombatRun");
    // 回避移行
    else if (_owner->GetPlayer()->IsEvade())
        _owner->GetStateMachine().ChangeState("CombatEvade");
    // ガード移行
    else if (_owner->GetPlayer()->IsGuard())
        _owner->GetStateMachine().ChangeState("CombatGuard");
    // 納刀移行
	else if (_owner->GetPlayer()->IsUsingItem())
		_owner->GetStateMachine().ChangeState("ToNonCombat");
}
#pragma endregion

#pragma region 走り
namespace RunSubState
{
	// 走り開始
    class RunStartSubState final : public PlayerSSB
    {
    public:
        RunStartSubState(PlayerStateMachine* stateMachine) :
            PlayerSSB(stateMachine,
                "RunStart",
                u8"RunCombatStartF0", 0.2f,
                false,
                true)
        {
        }
        void OnExecute(float elapsedTime) override
        {
            // 移動方向に向く
            _owner->RotationMovement(elapsedTime);
            // アニメーションが終了していたら遷移
            if (!_owner->GetAnimator()->IsPlayAnimation())
                _owner->GetStateMachine().ChangeSubState("Running");
            // 移動していなければ終了に遷移
            else if (!_owner->GetPlayer()->IsMoving())
                _owner->GetStateMachine().ChangeSubState("RunStop");
            // 納刀移行
            else if (_owner->GetPlayer()->IsUsingItem() || _owner->GetPlayer()->IsDash())
                _owner->GetStateMachine().ChangeSubState("RunToNonCombat");
        }
    };
	// 走りループ
    class RunningSubState final : public PlayerSSB
    {
    public:
        RunningSubState(PlayerStateMachine* stateMachine) :
            PlayerSSB(stateMachine,
                "Running",
                u8"RunCombatLoopF0", 0.2f,
                true,
                true)
        {
        }
        void OnExecute(float elapsedTime) override
        {
            // 移動方向に向く
            _owner->RotationMovement(elapsedTime);
            // 移動していなければ終了に遷移
            if (!_owner->GetPlayer()->IsMoving())
                _owner->GetStateMachine().ChangeSubState("RunStop");
            // 納刀移行
            else if (_owner->GetPlayer()->IsUsingItem() || _owner->GetPlayer()->IsDash())
                _owner->GetStateMachine().ChangeSubState("RunToNonCombat");
        }
    };
	// 走り終了
    class RunStopSubState final : public PlayerSSB
    {
    public:
        RunStopSubState(PlayerStateMachine* stateMachine) :
            PlayerSSB(stateMachine,
                "RunStop",
                u8"RunCombatStopF0", 0.2f,
                false,
                true)
        {
        }
        void OnExecute(float elapsedTime) override
        {
            // 移動方向に向く
            _owner->RotationMovement(elapsedTime);
            // 移動があれば遷移
            if (_owner->GetPlayer()->IsMoving())
                _owner->GetStateMachine().ChangeSubState("Running");
            // アニメーションが終了していたら遷移
            if (!_owner->GetAnimator()->IsPlayAnimation())
                _owner->GetStateMachine().ChangeState("CombatIdle");
        }
    };
    // 走り納刀
    class RunToNonCombatSubState final : public PlayerSSB
    {
	public:
		RunToNonCombatSubState(PlayerStateMachine* stateMachine) :
			PlayerSSB(stateMachine,
				"RunToNonCombat",
				u8"RunCombatToRun", 0.2f,
				false,
				true)
		{
		}
		void OnExecute(float elapsedTime) override
		{
			// アニメーションが終了していたら遷移
            if (!_owner->GetAnimator()->IsPlayAnimation())
            {
                _owner->GetStateMachine().ChangeState("Run");
                _owner->GetStateMachine().ChangeSubState("Running");
            }
		}
    };
}
PlayerGreatSwordRunState::PlayerGreatSwordRunState(PlayerStateMachine* stateMachine) :
    HierarchicalStateBase(stateMachine)
{
    // サブステート登録
    RegisterSubState(std::make_shared<RunSubState::RunStartSubState>(stateMachine));
    RegisterSubState(std::make_shared<RunSubState::RunningSubState>(stateMachine));
    RegisterSubState(std::make_shared<RunSubState::RunStopSubState>(stateMachine));
    RegisterSubState(std::make_shared<RunSubState::RunToNonCombatSubState>(stateMachine));
}

void PlayerGreatSwordRunState::OnEnter()
{
    // フラグを立てる
    _owner->GetPlayer()->SetIsDash(true);
    // 初期サブステート設定
    ChangeSubState("RunStart");
}
void PlayerGreatSwordRunState::OnExecute(float elapsedTime)
{
    // 攻撃遷移
    if (_owner->GetPlayer()->IsAttack())
        _owner->GetStateMachine().ChangeState("CombatAttack1");
    else if (_owner->GetPlayer()->IsSpecialAttack())
        _owner->GetStateMachine().ChangeState("CombatAttack2");
    // 回避移行
    else if (_owner->GetPlayer()->IsEvade())
        _owner->GetStateMachine().ChangeState("CombatEvade");
    // ガード移行
    else if (_owner->GetPlayer()->IsGuard())
        _owner->GetStateMachine().ChangeState("CombatGuard");
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
                - _owner->GetPlayer()->GetActor()->GetTransform().GetAngle().y
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

    // スタミナ自動回復を停止
    _owner->GetStaminaController()->SetIsStaminaRecover(false);
    // スタミナ消費
    _owner->GetStaminaController()->ConsumeStamina(
        _owner->GetPlayer()->GetEvadeStaminaConsume(),
        true);
}
void PlayerGreatSwordEvadeState::OnExecute(float elapsedTime)
{
    // アニメーションが終了していたら遷移
    if (!_owner->GetAnimator()->IsPlayAnimation())
    {
        _owner->GetStateMachine().ChangeState("CombatIdle");
    }
    else if (_owner->GetPlayer()->CallCancelEvent())
    {
        // 攻撃移行
        if (_owner->GetPlayer()->IsAttack())
            _owner->GetStateMachine().ChangeState("CombatAttack1");
        else if (_owner->GetPlayer()->IsSpecialAttack())
            _owner->GetStateMachine().ChangeState("CombatAttack2");
        // 移動移行
        else if (_owner->GetPlayer()->IsMoving())
            _owner->GetStateMachine().ChangeState("CombatRun");
        // ガード移行
        else if (_owner->GetPlayer()->IsGuard())
            _owner->GetStateMachine().ChangeState("CombatGuard");
    }
}
void PlayerGreatSwordEvadeState::OnExit()
{
    // スタミナ自動回復を再開
    _owner->GetStaminaController()->SetIsStaminaRecover(true);
}
#pragma endregion

#pragma region 攻撃1
// 攻撃1サブステート
namespace Attack1SubState
{
    class ComboSubState : public PlayerSSB
    {
    public:
        static constexpr float ATK = 1.0f;

    public:
        ComboSubState(PlayerStateMachine* stateMachine,
            const std::string& name,
            const std::string& animationName,
            const std::string& nextSubStateName,
            const std::string& branchSubStateName,
            float animationBlendTime,
            float motionFactor) :
            PlayerSSB(stateMachine,
                name,
                animationName,
                animationBlendTime,
                false,
                true),
			_nextSubStateName(nextSubStateName),
            _branchSubStateName(branchSubStateName),
            _motionFactor(motionFactor)
        {
        }

        const std::string& GetNextStateName() const { return _nextSubStateName; }
        const std::string& GetBranchStateName() const { return _branchSubStateName; }

        void OnEnter() override
        {
            PlayerSSB::OnEnter();
            // 攻撃フラグを立てる
			_owner->GetDamageSender()->SetMotionFactor(_motionFactor);
        }
    private:
        std::string _nextSubStateName;
        std::string _branchSubStateName;
		float _motionFactor = 1.0f;
    };
    class ChargingAttackSubState final : public ComboSubState
    {
    public:
        ChargingAttackSubState(PlayerStateMachine* stateMachine,
            const std::string& name,
            const std::string& animationName,
            const std::string& nextSubStateName,
            const std::string& branchSubStateName,
            float animationBlendTime,
            float ATK) :
            ComboSubState(stateMachine,
                name,
                animationName,
                nextSubStateName,
                branchSubStateName,
                animationBlendTime,
                ATK)
        {
        }

        void OnEnter() override
        {
            ComboSubState::OnEnter();
            _chargingTimer = 0.0f;
            _isCharging = true;
            _chargeStage = 1;
        }
        void OnExecute(float elapsedTime) override
        {
            // チャージ中の処理
            if (_owner->GetPlayer()->CallChargingEvent() && _isCharging)
            {
                if (_owner->GetPlayer()->IsHoldingAttackKey())
                {
                    _chargingTimer += elapsedTime;
                    // アニメーションを止める
                    _owner->GetAnimator()->SetIsPaused(true);

                    if (_chargeStage <= _chargeStageMax && _chargingTimer > _chargeStageTimer * _chargeStage)
                    {
                        // リムライト処理
                        _owner->GetPlayer()->SetChargeLevel(_chargeStage);
						_owner->GetPlayer()->StartChargeEffectRimLight();

                        // 各チャージエフェクト停止
						_owner->GetEffect()->Stop(PlayerController::EffectType::Charge0);
						_owner->GetEffect()->Stop(PlayerController::EffectType::Charge1);
						_owner->GetEffect()->Stop(PlayerController::EffectType::Charge2);
						// エフェクト再生
						PlayerController::EffectType effectType = PlayerController::EffectType::Charge0;
                        switch (_chargeStage)
                        {
						case 1: effectType = PlayerController::EffectType::Charge0; break;
						case 2: effectType = PlayerController::EffectType::Charge1; break;
						case 3: effectType = PlayerController::EffectType::Charge2; break;
                        }
                        _owner->GetEffect()->Play(effectType,
                            _owner->GetPlayer()->GetActor()->GetTransform().GetWorldPosition() + _effectOffset);

                        // チャージステージを上げる
                        _chargeStage++;
                    }
                }
                else
                {
                    _isCharging = false;
                    // アニメーションを再開
                    _owner->GetAnimator()->SetIsPaused(false);
                }
            }
            // エフェクトの位置を更新
            Vector3 position = _owner->GetPlayer()->GetActor()->GetTransform().GetWorldPosition() + _effectOffset;
            _owner->GetEffect()->GetEffectData(PlayerController::EffectType::Charge0)->SetPosition(position);
            _owner->GetEffect()->GetEffectData(PlayerController::EffectType::Charge1)->SetPosition(position);
            _owner->GetEffect()->GetEffectData(PlayerController::EffectType::Charge2)->SetPosition(position);
        }
        void OnExit() override
        {
            ComboSubState::OnExit();
            // アニメーションを再開
            _owner->GetAnimator()->SetIsPaused(false);
        }
    private:
        float _chargingTimer = 0.0f;
        float _chargeStageTimer = 1.0f;
        int _chargeStage = 1;
        int _chargeStageMax = 3;
        bool _isCharging = true;

		Vector3 _effectOffset = Vector3(0.0f, 0.5f, 0.0f);
    };
}
PlayerGreatSwordAttack1State::PlayerGreatSwordAttack1State(PlayerStateMachine* stateMachine) :
    HierarchicalStateBase(stateMachine)
{
    // サブステート登録
    RegisterSubState(std::make_shared<Attack1SubState::ChargingAttackSubState>(stateMachine,
        "ComboAttack1",
        "ComboAttack02_02",
        "ComboAttack2",
        "ComboAttack1",
        1.0f,
        1.0f));
    RegisterSubState(std::make_shared<Attack1SubState::ComboSubState>(stateMachine,
        "ComboAttack2",
        "ComboAttack03_01",
        "ComboAttack3",
        "ComboAttack1",
        0.5f,
        1.0f));
    RegisterSubState(std::make_shared<Attack1SubState::ChargingAttackSubState>(stateMachine,
        "ComboAttack3",
        "ComboAttack01_02",
        "ComboAttack4",
        "ComboAttack1",
        1.0f,
        1.0f));
    RegisterSubState(std::make_shared<Attack1SubState::ChargingAttackSubState>(stateMachine,
        "ComboAttack4",
        "ComboAttack04_02",
        "",
        "ComboAttack1",
        1.0f,
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
    // ブレンドアニメーション中は向きを変えれるようにする
    if (_owner->GetAnimator()->IsBlending())
    {
        _owner->RotationMovement(_rotationSpeed * elapsedTime);
    }

    // 先行入力受付時
    if (_owner->GetPlayer()->CallInputBufferingEvent())
    {
        // 先行入力遷移先を設定
        // 攻撃
        if (_owner->GetPlayer()->IsAttack())
            _nextStateName = "Attack";
        else if (_owner->GetPlayer()->IsSpecialAttack())
            _nextStateName = "SpecialAttack";
        // 回避移行
        else if (_owner->GetPlayer()->IsEvade())
            _nextStateName = "CombatEvade";
        // ガード移行
        else if (_owner->GetPlayer()->IsGuard())
            _nextStateName = "CombatGuard";
    }

    // 攻撃キャンセル判定
    if (_owner->GetPlayer()->CallCancelAttackEvent())
    {
        auto subState = dynamic_cast<Attack1SubState::ComboSubState*>(this->_subState);
        if (subState)
        {
            if (_nextStateName == "Attack" && !subState->GetNextStateName().empty())
            {
                // キャンセル待機中に入力があった場合は先行入力遷移
                _owner->GetStateMachine().ChangeSubState(subState->GetNextStateName());
                _nextStateName = ""; // 遷移後はクリア
                return;
            }
            else if (_nextStateName == "SpecialAttack" && !subState->GetBranchStateName().empty())
            {
                // 攻撃2のほうに遷移
                _owner->GetStateMachine().ChangeState("CombatAttack2");
                _owner->GetStateMachine().ChangeSubState(subState->GetBranchStateName());
                _nextStateName = ""; // 遷移後はクリア
                return;
            }
        }
    }

    // キャンセル判定
    if (_owner->GetPlayer()->CallCancelEvent())
    {
        if (!_nextStateName.empty())
            _owner->GetStateMachine().ChangeState(_nextStateName);
    }

    // アニメーションが終了していたら遷移
    if (!_owner->GetAnimator()->IsPlayAnimation())
    {
        // 攻撃からIdleに遷移
        _owner->GetStateMachine().ChangeState("CombatIdle");
    }
}
#pragma endregion

#pragma region 攻撃2
PlayerGreatSwordAttack2State::PlayerGreatSwordAttack2State(PlayerStateMachine* stateMachine) :
    HierarchicalStateBase(stateMachine)
{
    // サブステート登録
    RegisterSubState(std::make_shared<Attack1SubState::ComboSubState>(stateMachine,
        "ComboAttack1",
        "ComboAttack04_01",
        "",
        "ComboAttack2",
        0.3f,
        1.0f));
}

void PlayerGreatSwordAttack2State::OnEnter()
{
    _owner->GetAnimator()->SetIsUseRootMotion(true);
    // 初期サブステート設定
    ChangeSubState("ComboAttack1");
    // 先行入力遷移先をクリア
    _nextStateName = "";
}

void PlayerGreatSwordAttack2State::OnExecute(float elapsedTime)
{
    // 先行入力受付時
    if (_owner->GetPlayer()->CallInputBufferingEvent())
    {
        // 先行入力遷移先を設定
        // 攻撃
        if (_owner->GetPlayer()->IsAttack())
            _nextStateName = "Attack";
        else if (_owner->GetPlayer()->IsSpecialAttack())
            _nextStateName = "SpecialAttack";
        // 回避移行
        else if (_owner->GetPlayer()->IsEvade())
            _nextStateName = "CombatEvade";
        // ガード移行
        else if (_owner->GetPlayer()->IsGuard())
            _nextStateName = "CombatGuard";
    }

    // 攻撃キャンセル判定
    if (_owner->GetPlayer()->CallCancelAttackEvent())
    {
        auto subState = dynamic_cast<Attack1SubState::ComboSubState*>(this->_subState);
        if (subState)
        {
            if (_nextStateName == "Attack" && !subState->GetBranchStateName().empty())
            {
                // 攻撃1のほうに遷移
                _owner->GetStateMachine().ChangeState("CombatAttack1");
                _owner->GetStateMachine().ChangeSubState(subState->GetBranchStateName());
                _nextStateName = ""; // 遷移後はクリア
                return;
            }
            else if (_nextStateName == "SpecialAttack" && !subState->GetNextStateName().empty())
            {
                // キャンセル待機中に入力があった場合は先行入力遷移
                _owner->GetStateMachine().ChangeSubState(subState->GetNextStateName());
                _nextStateName = ""; // 遷移後はクリア
                return;
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
        _owner->GetStateMachine().ChangeState("CombatIdle");
    }
}
#pragma endregion

#pragma region ガード
// ガードサブステート
namespace GuardSubState
{
    // ガード開始
    class GuardStartSubState final : public PlayerSSB
    {
    public:
        GuardStartSubState(PlayerStateMachine* stateMachine) :
            PlayerSSB(stateMachine,
                "GuardStart",
                u8"BlockStart",
                2.0f,
                false,
                true)
        {
        }
        void OnExecute(float elapsedTime) override
        {
            // アニメーションが終了していたら遷移
            if (!_owner->GetAnimator()->IsPlayAnimation())
                _owner->GetStateMachine().ChangeSubState("Guarding");
            // ガード解除
            else if (!_owner->GetPlayer()->IsGuard())
                _owner->GetStateMachine().ChangeSubState("GuardEnd");
        }
    };
    // ガード中
    class GuardingSubState final : public PlayerSSB
    {
    public:
        GuardingSubState(PlayerStateMachine* stateMachine) :
            PlayerSSB(stateMachine,
                "Guarding",
                u8"BlockLoop",
                0.2f,
                true,
                true)
        {
        }
        void OnExecute(float elapsedTime) override
        {
            // ガード解除
            if (!_owner->GetPlayer()->IsGuard())
                _owner->GetStateMachine().ChangeSubState("GuardEnd");
        }
    };
    // ガード終了
    class GuardEndSubState final : public PlayerSSB
    {
    public:
        GuardEndSubState(PlayerStateMachine* stateMachine) :
            PlayerSSB(stateMachine,
                "GuardEnd",
                u8"BlockEnd",
                0.2f,
                false,
                true)
        {
        }
        void OnExecute(float elapsedTime) override
        {
            // アニメーションが終了していたら遷移
            if (!_owner->GetAnimator()->IsPlayAnimation())
                _owner->GetStateMachine().ChangeState("CombatIdle");
        }
    };
    // ガード時被弾
    class GuardHitSubState final : public PlayerSSB
    {
    public:
        GuardHitSubState(PlayerStateMachine* stateMachine) :
            PlayerSSB(stateMachine,
                "GuardHit",
                u8"BlockHit",
                0.2f,
                false,
                true)
        {
        }
        void OnEnter() override
        {
			PlayerSSB::OnEnter();
            // 被弾モーション中は押し出されないようにする
            auto charactorController = _owner->GetPlayer()->GetCharactorController();
            if (charactorController != nullptr)
            {
                charactorController->SetIsPushable(false);
            }
            // スタミナ消費
            _owner->GetStaminaController()->ConsumeStamina(
                _owner->GetPlayer()->GetGuardStaminaConsume(),
                true);
        }
        void OnExecute(float elapsedTime) override
        {
            // アニメーションが終了していたら遷移
            if (!_owner->GetAnimator()->IsPlayAnimation())
                _owner->GetStateMachine().ChangeSubState("Guarding");
        }
        void OnExit() override
        {
            // 押し出されれるようにする
            auto charactorController = _owner->GetPlayer()->GetCharactorController();
            if (charactorController != nullptr)
            {
                charactorController->SetIsPushable(true);
            }
        }
    };
}

PlayerGreatSwordGuardState::PlayerGreatSwordGuardState(PlayerStateMachine* stateMachine) :
    HierarchicalStateBase(stateMachine)
{
    // サブステート登録
    RegisterSubState(std::make_shared<GuardSubState::GuardStartSubState>(stateMachine));
    RegisterSubState(std::make_shared<GuardSubState::GuardingSubState>(stateMachine));
    RegisterSubState(std::make_shared<GuardSubState::GuardHitSubState>(stateMachine));
    RegisterSubState(std::make_shared<GuardSubState::GuardEndSubState>(stateMachine));
}
void PlayerGreatSwordGuardState::OnEnter()
{
    // フラグを立てる
    _owner->GetPlayer()->SetIsGuard(true);
    _owner->GetAnimator()->SetIsUseRootMotion(true);
    // 初期サブステート設定
    ChangeSubState("GuardStart");
    // スタミナ自動回復を停止
    _owner->GetStaminaController()->SetIsStaminaRecover(false);
}
void PlayerGreatSwordGuardState::OnExecute(float elapsedTime)
{
    // スタミナが尽きたら遷移
    if (_owner->GetStaminaController()->GetStamina() <= 0.0f)
        _owner->GetStateMachine().ChangeState("CombatIdle");

    // 移動方向に向く
    _owner->RotationMovement(elapsedTime);

    // 攻撃移行
    if (_owner->GetPlayer()->IsAttack())
        _owner->GetStateMachine().ChangeState("CombatAttack1");
    else if (_owner->GetPlayer()->IsSpecialAttack())
        _owner->GetStateMachine().ChangeState("CombatAttack2");
    // 回避移行
    else if (_owner->GetPlayer()->IsEvade())
        _owner->GetStateMachine().ChangeState("CombatEvade");
}
void PlayerGreatSwordGuardState::OnExit()
{
    // フラグを下ろす
    _owner->GetPlayer()->SetIsGuard(false);
    // スタミナ自動回復を再開
    _owner->GetStaminaController()->SetIsStaminaRecover(true);
}
#pragma endregion

#pragma region 被弾
PlayerGreatSwordHitState::PlayerGreatSwordHitState(PlayerStateMachine* stateMachine) :
    Player8WayHSB(
        stateMachine,
        { u8"HitCombatF",
        u8"HitCombatF",
        u8"HitCombatR",
        u8"HitCombatB",
        u8"HitCombatB",
        u8"HitCombatB",
        u8"HitCombatL",
        u8"HitCombatF", },
        0.2f,
        true)
{
}
void PlayerGreatSwordHitState::OnEnter()
{
	// プレイヤーの向きと、被弾方向から被弾アニメーションを決定
	auto& position = _owner->GetPlayer()->GetActor()->GetTransform().GetPosition();
    auto& hitPosition = _owner->GetPlayer()->GetDamageable()->GetHitPosition();
	auto hitDirection = (hitPosition - position).Normalize();

    float angle =
        DirectX::XMConvertToDegrees(
            atan2f(hitDirection.x, hitDirection.z)
            - _owner->GetPlayer()->GetActor()->GetTransform().GetAngle().y
        );
    // 角度を0~360度に正規化
    angle = fmodf(angle, 360.0f);
    if (angle < 0.0f)
        angle += 360.0f;
    // 360度を8方向に分割
    int index = (int)(angle / 45.0f + 0.5f);
    if (index >= 8)
        index = 0;
    Player8WayHSB::Direction directionType = static_cast<Player8WayHSB::Direction>(index);
    ChangeSubState(directionType);

    // 被弾モーション中は押し出されないようにする
    auto charactorController = _owner->GetPlayer()->GetCharactorController();
    if (charactorController != nullptr)
    {
        charactorController->SetIsPushable(false);
    }
}

void PlayerGreatSwordHitState::OnExecute(float elapsedTime)
{
    // アニメーションが終了していたら遷移
    if (!_owner->GetAnimator()->IsPlayAnimation())
        _owner->GetStateMachine().ChangeState("CombatIdle");
}

void PlayerGreatSwordHitState::OnExit()
{
    // 押し出されれるようにする
    auto charactorController = _owner->GetPlayer()->GetCharactorController();
    if (charactorController != nullptr)
    {
        charactorController->SetIsPushable(true);
    }
}

PlayerGreatSwordHitKnockDownState::PlayerGreatSwordHitKnockDownState(PlayerStateMachine* stateMachine) :
    Player8WayHSB(
        stateMachine,
        { u8"HitCombatLargeF",
        u8"HitCombatLargeF",
        u8"HitCombatLargeR",
        u8"HitCombatLargeB",
        u8"HitCombatLargeB",
        u8"HitCombatLargeB",
        u8"HitCombatLargeL",
        u8"HitCombatLargeF", },
        0.2f,
        true)
{
}

void PlayerGreatSwordHitKnockDownState::OnEnter()
{
    // プレイヤーの向きと、被弾方向から被弾アニメーションを決定
    auto& position = _owner->GetPlayer()->GetActor()->GetTransform().GetPosition();
    auto& hitPosition = _owner->GetPlayer()->GetDamageable()->GetHitPosition();
    auto hitDirection = (hitPosition - position).Normalize();

    float angle =
        DirectX::XMConvertToDegrees(
            atan2f(hitDirection.x, hitDirection.z)
            - _owner->GetPlayer()->GetActor()->GetTransform().GetAngle().y
        );
    // 角度を0~360度に正規化
    angle = fmodf(angle, 360.0f);
    if (angle < 0.0f)
        angle += 360.0f;
    // 360度を8方向に分割
    int index = (int)(angle / 45.0f + 0.5f);
    if (index >= 8)
        index = 0;
    Player8WayHSB::Direction directionType = static_cast<Player8WayHSB::Direction>(index);
    ChangeSubState(directionType);

    // 被弾モーション中は押し出されないようにする
    auto charactorController = _owner->GetPlayer()->GetCharactorController();
    if (charactorController != nullptr)
    {
        charactorController->SetIsPushable(false);
    }
}

void PlayerGreatSwordHitKnockDownState::OnExecute(float elapsedTime)
{
    // アニメーションが終了していたら遷移
    if (!_owner->GetAnimator()->IsPlayAnimation())
        _owner->GetStateMachine().ChangeState("CombatIdle");
}

void PlayerGreatSwordHitKnockDownState::OnExit()
{
    // 押し出されれるようにする
    auto charactorController = _owner->GetPlayer()->GetCharactorController();
    if (charactorController != nullptr)
    {
        charactorController->SetIsPushable(true);
    }
}

#pragma endregion

#pragma region 納刀
void PlayerGreatSwordToNonCombatState::OnExecute(float elapsedTime)
{
    // アニメーションが終了していたら遷移
    if (!_owner->GetAnimator()->IsPlayAnimation())
        _owner->GetStateMachine().ChangeState("Idle");
}
#pragma endregion

#pragma region ダウン
namespace PlayerGreatSwordDownSubState
{
    class DownStartSubState : public PlayerSSB
    {
    public:
        DownStartSubState(PlayerStateMachine* stateMachine) :
            PlayerSSB(stateMachine,
                "DownStart",
                u8"HitCombatLargeDeath",
                0.2f,
                false,
                true)
        {
        }
        ~DownStartSubState() override {}
        void OnExecute(float elapsedTime) override
        {
            // アニメーションが終了していて、移動入力があれば遷移
            if (!_owner->GetAnimator()->IsPlayAnimation())
            {
                if (_owner->GetPlayer()->IsMoving())
                {
                    _owner->GetStateMachine().ChangeSubState("DownEnd");
                    return;
                }
            }
        }
    };
    class DownEndSubState : public PlayerSSB
    {
    public:
        DownEndSubState(PlayerStateMachine* stateMachine) :
            PlayerSSB(stateMachine,
                "DownEnd",
                u8"GetUpCombat",
                0.2f,
                false,
                true)
        {
        }
        ~DownEndSubState() override {}
        void OnExecute(float elapsedTime) override
        {
            // アニメーションが終了していたら遷移
            if (!_owner->GetAnimator()->IsPlayAnimation())
                _owner->GetStateMachine().ChangeState("Idle");
            // キャンセルイベントが呼ばれたら
            if (_owner->GetPlayer()->CallCancelEvent())
            {
                // 移動移行
                if (_owner->GetPlayer()->IsMoving())
                    _owner->GetStateMachine().ChangeState("CombatRun");
                // ガード移行
                else if (_owner->GetPlayer()->IsGuard())
                    _owner->GetStateMachine().ChangeState("CombatGuard");
            }
        }
    };
}
PlayerGreatSwordDownState::PlayerGreatSwordDownState(PlayerStateMachine* stateMachine) :
    HierarchicalStateBase(stateMachine)
{
    RegisterSubState(std::make_shared<PlayerGreatSwordDownSubState::DownStartSubState>(stateMachine));
    RegisterSubState(std::make_shared<PlayerGreatSwordDownSubState::DownEndSubState>(stateMachine));
}

void PlayerGreatSwordDownState::OnEnter()
{
    // ダメージを受けた方向を向く
    auto& hitPosition = _owner->GetPlayer()->GetDamageable()->GetHitPosition();
	_owner->GetPlayer()->GetActor()->GetTransform().LookAt(hitPosition);
    // XZ回転量をリセット
    _owner->GetPlayer()->GetActor()->GetTransform().SetAngleX(0.0f);
    _owner->GetPlayer()->GetActor()->GetTransform().SetAngleZ(0.0f);

    ChangeSubState("DownStart");
    // モーション中は押し出されないようにする
    auto charactorController = _owner->GetPlayer()->GetCharactorController();
    if (charactorController != nullptr)
    {
        charactorController->SetIsPushable(false);
    }
}

void PlayerGreatSwordDownState::OnExit()
{
    // 押し出されれるようにする
    auto charactorController = _owner->GetPlayer()->GetCharactorController();
    if (charactorController != nullptr)
    {
        charactorController->SetIsPushable(true);
    }
}
#pragma endregion
