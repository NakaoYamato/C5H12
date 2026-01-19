#include "PlayerGreatSwordStates.h"

#include "../../Library/Component/Animator.h"
#include "../../Source/Player/PlayerController.h"
#include "../../Library/Algorithm/Converter.h"

#pragma region 待機
void PlayerGreatSwordIdleState::OnExecute(float elapsedTime)
{
    // 攻撃遷移
    if (_owner->GetPlayer()->IsAttack())
        _owner->ChangeState("CombatAttack1");
    else if (_owner->GetPlayer()->IsSpecialAttack())
        _owner->ChangeState("CombatAttack2");
    // 移動
    else if (_owner->GetPlayer()->IsMoving())
        _owner->ChangeState("CombatRun");
    // 回避移行
    else if (_owner->GetPlayer()->IsEvade())
        _owner->ChangeState("CombatEvade");
    // ガード移行
    else if (_owner->GetPlayer()->IsGuard())
        _owner->ChangeState("CombatGuard");
    // 納刀移行
	else if (_owner->GetPlayer()->IsUsingItem())
		_owner->ChangeState("ToNonCombat");

    // 操作UI表示
    _owner->GetOperateUIController()->AddDescription(
        "溜め斬り",
        { "Action1" });
    _owner->GetOperateUIController()->AddDescription(
        "薙ぎ払い",
        { "Action2" });
    _owner->GetOperateUIController()->AddDescription(
        "回避",
        { "Evade" });
    _owner->GetOperateUIController()->AddDescription(
        "ガード",
        { "Guard" });
}
#pragma endregion

#pragma region 走り
namespace RunSubState
{
	// 走り開始
    class RunStartSubState final : public PlayerSSB
    {
    public:
        RunStartSubState(PlayerStateController* owner) :
            PlayerSSB(owner,
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
            if (!_owner->GetAnimator()->IsPlaying())
                _owner->ChangeSubState("Running");
            // 移動していなければ終了に遷移
            else if (!_owner->GetPlayer()->IsMoving())
                _owner->ChangeSubState("RunStop");
            // 納刀移行
            else if (_owner->GetPlayer()->IsUsingItem() || _owner->GetPlayer()->IsDash())
                _owner->ChangeSubState("RunToNonCombat");
        }
    };
	// 走りループ
    class RunningSubState final : public PlayerSSB
    {
    public:
        RunningSubState(PlayerStateController* owner) :
            PlayerSSB(owner,
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
                _owner->ChangeSubState("RunStop");
            // 納刀移行
            else if (_owner->GetPlayer()->IsUsingItem() || _owner->GetPlayer()->IsDash())
                _owner->ChangeSubState("RunToNonCombat");
        }
    };
	// 走り終了
    class RunStopSubState final : public PlayerSSB
    {
    public:
        RunStopSubState(PlayerStateController* owner) :
            PlayerSSB(owner,
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
                _owner->ChangeSubState("Running");
            // アニメーションが終了していたら遷移
            if (!_owner->GetAnimator()->IsPlaying())
                _owner->ChangeState("CombatIdle");
        }
    };
    // 走り納刀
    class RunToNonCombatSubState final : public PlayerSSB
    {
	public:
		RunToNonCombatSubState(PlayerStateController* owner) :
			PlayerSSB(owner,
				"RunToNonCombat",
				u8"RunCombatToRun", 0.2f,
				false,
				true)
		{
		}
		void OnExecute(float elapsedTime) override
		{
            // 移動方向に向く
            _owner->RotationMovement(elapsedTime);
			// アニメーションが終了していたら遷移
            if (!_owner->GetAnimator()->IsPlaying())
            {
                _owner->ChangeToNonCombatState("Run");
                _owner->ChangeSubState("Running");
            }
		}
    };
}
PlayerGreatSwordRunState::PlayerGreatSwordRunState(PlayerStateController* owner) :
    HierarchicalStateBase(owner)
{
    // サブステート登録
    RegisterSubState(std::make_shared<RunSubState::RunStartSubState>(owner));
    RegisterSubState(std::make_shared<RunSubState::RunningSubState>(owner));
    RegisterSubState(std::make_shared<RunSubState::RunStopSubState>(owner));
    RegisterSubState(std::make_shared<RunSubState::RunToNonCombatSubState>(owner));
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
        _owner->ChangeState("CombatAttack1");
    else if (_owner->GetPlayer()->IsSpecialAttack())
        _owner->ChangeState("CombatAttack2");
    // 回避移行
    else if (_owner->GetPlayer()->IsEvade())
        _owner->ChangeState("CombatEvade");
    // ガード移行
    else if (_owner->GetPlayer()->IsGuard())
        _owner->ChangeState("CombatGuard");

    // 操作UI表示
    _owner->GetOperateUIController()->AddDescription(
        "溜め斬り",
        { "Action1" });
    _owner->GetOperateUIController()->AddDescription(
        "薙ぎ払い",
        { "Action2" });
    _owner->GetOperateUIController()->AddDescription(
        "回避",
        { "Evade" });
    _owner->GetOperateUIController()->AddDescription(
        "ガード",
        { "Guard" });
}
void PlayerGreatSwordRunState::OnExit()
{
    // フラグを下ろす
    _owner->GetPlayer()->SetIsDash(false);
}
#pragma endregion

#pragma region 回避
PlayerGreatSwordEvadeState::PlayerGreatSwordEvadeState(PlayerStateController* owner) :
    Player8WayHSB(
        owner, 
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
    if (!_owner->GetAnimator()->IsPlaying())
    {
        _owner->ChangeState("CombatIdle");
    }
    else if (_owner->GetPlayer()->CallCancelEvent())
    {
        // 攻撃移行
        if (_owner->GetPlayer()->IsAttack())
            _owner->ChangeState("CombatAttack1");
        else if (_owner->GetPlayer()->IsSpecialAttack())
            _owner->ChangeState("CombatAttack2");
        // 回避移行
        else if (_owner->GetPlayer()->IsEvade())
            _owner->ChangeState("CombatEvade");
        // 移動移行
        else if (_owner->GetPlayer()->IsMoving())
            _owner->ChangeState("CombatRun");
        // ガード移行
        else if (_owner->GetPlayer()->IsGuard())
            _owner->ChangeState("CombatGuard");
    }

    // 操作UI表示
    _owner->GetOperateUIController()->AddDescription(
        "溜め斬り",
        { "Action1" });
    _owner->GetOperateUIController()->AddDescription(
        "薙ぎ払い",
        { "Action2" });
    _owner->GetOperateUIController()->AddDescription(
        "回避",
        { "Evade" });
    _owner->GetOperateUIController()->AddDescription(
        "ガード",
        { "Guard" });
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
        ComboSubState(PlayerStateController* owner,
            const std::string& name,
            const std::string& animationName,
            const std::string& nextSubStateName,
            const std::string& branchSubStateName,
            float animationBlendTime,
            float vibrationL = 0.5f,
            float vibrationR = 0.0f,
            float vibrationTime = 0.1f) :
            PlayerSSB(owner,
                name,
                animationName,
                animationBlendTime,
                false,
                true),
			_nextSubStateName(nextSubStateName),
            _branchSubStateName(branchSubStateName),
			_vibrationL(vibrationL),
			_vibrationR(vibrationR),
			_vibrationTime(vibrationTime)
        {
        }

        void OnEnter() override
        {
            PlayerSSB::OnEnter();
            // 攻撃フラグを立てる
			float motionFactor = _owner->GetPlayer()->GetAttackMotionFactor();
            // チャージ量に合わせて攻撃力を設定
            int chargeLevel = std::clamp(_owner->GetPlayer()->GetChargeLevel(), 1, 3);
			_owner->GetDamageSender()->SetMotionFactor(motionFactor + 0.5f * (chargeLevel - 1));
            // 遷移先を設定
			_owner->SetNextSubStateName(_nextSubStateName);
			_owner->SetNextBranchSubStateName(_branchSubStateName);

			// 振動
			_owner->GetPlayer()->SetStageContactVibration(
				_vibrationL,
				_vibrationR,
				_vibrationTime);
        }
        void OnExecute(float elapsedTime) override
        {
            // アニメーションが終了していたら遷移
            if (!_owner->GetAnimator()->IsPlaying())
            {
                // 攻撃からIdleに遷移
                _owner->ChangeState("CombatIdle");
            }
        }
        void OnExit() override
        {
            PlayerSSB::OnExit();
        }
    protected:
        std::string _nextSubStateName;
        std::string _branchSubStateName;
		float _vibrationL = 0.5f;
		float _vibrationR = 0.0f;
		float _vibrationTime = 0.1f;
    };
    class ChargeSubState : public ComboSubState
    {
    public:
        ChargeSubState(PlayerStateController* owner,
            const std::string& name,
            const std::string& animationName,
            const std::string& nextSubStateName,
            const std::string& branchSubStateName,
            float animationBlendTime) :
            ComboSubState(owner,
                name,
                animationName,
                nextSubStateName,
                branchSubStateName,
                animationBlendTime)
        {
        }

        void OnEnter() override
        {
            ComboSubState::OnEnter();
            _chargingTimer = 0.0f;
            _chargeStage = 1;
        }
        void OnExecute(float elapsedTime) override
        {
            // 攻撃キーを押し続けている時
            if (_owner->GetPlayer()->IsHoldingAttackKey())
            {
                _chargingTimer += elapsedTime;

                if (_chargingTimer > _chargeStageTimer * _chargeStage)
                {
                    if (_chargeStage <= _chargeStageMax)
                    {
                        // リムライト処理
                        _owner->GetPlayer()->SetChargeLevel(_chargeStage);
                        _owner->GetPlayer()->StartChargeEffectRimLight();

                        // コントローラー振動
                        _owner->GetPlayer()->SetVibration(0.1f * _chargeStage, 0.1f * _chargeStage, 0.2f);

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

						if (_chargeStage > _chargeStageMax)
						{
                            _chargingTimer = _chargeStageTimer * _chargeStage;
						}
                        else
                        {
                            // チャージステージを上げる
                            _chargeStage++;
                        }
                    }
                    else
                    {
                        _chargingTimer = _chargeStageTimer * _chargeStage;
                    }
                }
                
                // 突進遷移
                if (_owner->GetPlayer()->IsSpecialAttack())
                {
                    // 攻撃2のほうに遷移
                    _owner->ChangeState("CombatAttack2");
                    _owner->ChangeSubState(_branchSubStateName);
                }
            }
            // 離したとき
            else
            {
                auto& subState = _nextSubStateName;
                // 次のサブステートへ遷移
                _owner->ChangeSubState(subState);
            }
            // エフェクトの位置を更新
            Vector3 position = _owner->GetPlayer()->GetActor()->GetTransform().GetWorldPosition() + _effectOffset;
            _owner->GetEffect()->GetEffectData(PlayerController::EffectType::Charge0)->SetPosition(position);
            _owner->GetEffect()->GetEffectData(PlayerController::EffectType::Charge1)->SetPosition(position);
            _owner->GetEffect()->GetEffectData(PlayerController::EffectType::Charge2)->SetPosition(position);

            // カメラ
            if (auto cameraActor = _owner->GetPlayer()->GetActor()->GetScene()->GetMainCameraActor())
            {
                cameraActor->AddOnUpdateCallback([&](float elapsedTime, MainCamera* camera)
                    {
                        float rate = (_chargingTimer - (_chargeStage - 1) * _chargeStageTimer) / _chargeStageTimer;
                        camera->AddEyeOffset(Vector3::Lerp(_cameraEyeStartOffset, _cameraEyeEndOffset, rate, EasingType::InSine));
                        return true;
                    });
            }

            // 操作UI表示
            _owner->GetOperateUIController()->AddDescription(
                "タックル",
                { "Action2" });
        }
        void OnExit() override
        {
            ComboSubState::OnExit();
            _owner->GetPlayer()->SetChargeLevel(_chargeStage);
        }
    private:
        float _chargingTimer = 0.0f;
        float _chargeStageTimer = 0.8f;
        int _chargeStage = 1;
        int _chargeStageMax = 3;

        Vector3 _effectOffset = Vector3(0.0f, 0.5f, 0.0f);

        Vector3 _cameraEyeStartOffset = Vector3(0.0f, 0.0f, 0.5f);
        Vector3 _cameraEyeEndOffset = Vector3(0.0f, -0.05f, 0.6f);
    };
    class EndComboSubState : public ComboSubState
    {
    public:
		EndComboSubState(PlayerStateController* owner) :
            ComboSubState(owner,
                "ChargeAttack03End",
                "ChargeAttack03End",
                "",
                "",
                1.0f,
                0.25f, 0.0f, 0.15f)
		{
		}
        void OnEnter() override
        {
			// ルートモーションの影響度を下げる
			_owner->GetAnimator()->SetRootMotionInfluence(0.5f);

			ComboSubState::OnEnter();
            _timer = 0.0f;
        }
		void OnExecute(float elapsedTime) override
		{
			_timer += elapsedTime;
            if (_timer > 1.0f)
            {
                // 振動
                _owner->GetPlayer()->SetStageContactVibration(
                    1.0f,
                    0.25f,
                    0.6f);
            }
			// アニメーションが終了していたら遷移
			if (!_owner->GetAnimator()->IsPlaying())
			{
				// 攻撃からIdleに遷移
				_owner->ChangeState("CombatIdle");
			}
		}
        void OnExit() override
        {
			ComboSubState::OnExit();
            // ルートモーションの影響度を戻す
            _owner->GetAnimator()->SetRootMotionInfluence(1.0f);
        }
    private:
        float _timer = 0.0f;
    };
	class ToRunSubState : public PlayerSSB
	{
	public:
		ToRunSubState(PlayerStateController* owner) :
			PlayerSSB(owner,
				"ToRun",
				"RunCombatStartF0",
				1.0f,
				false,
				true)
		{
		}
		void OnExecute(float elapsedTime) override
		{
            // 移動方向に向く
            _owner->RotationMovement(elapsedTime);
			// アニメーションが終了していたら遷移
			if (!_owner->GetAnimator()->IsPlaying())
			{
				_owner->ChangeState("CombatRun");
				_owner->ChangeSubState("Running");
			}

            // ブレンド処理中ならコンボ成立
            if (_owner->GetAnimator()->IsBlending())
            {
                // 攻撃
                if (_owner->GetPlayer()->IsAttack())
                {
                    std::string subStateName = _owner->GetNextSubStateName();
                    if (!subStateName.empty())
                    {
                        // 攻撃1のほうに遷移
                        _owner->ChangeState("CombatAttack1");
                        _owner->ChangeSubState(subStateName);
                    }
                }
                if (_owner->GetPlayer()->IsSpecialAttack())
                {
                    std::string subStateName = _owner->GetNextBranchSubStateName();
                    if (!subStateName.empty())
                    {
                        // 攻撃2のほうに遷移
                        _owner->ChangeState("CombatAttack2");
                        _owner->ChangeSubState(subStateName);
                    }
                }
            }
            else
            {
                // 動いていないなら待機へ遷移
                if (!_owner->GetPlayer()->IsMoving())
                {
                    _owner->ChangeState("CombatIdle");
                    return;
                }

                // 攻撃
                if (_owner->GetPlayer()->IsAttack())
                {
                    // 攻撃1のほうに遷移
                    _owner->ChangeState("CombatAttack1");
                }
                if (_owner->GetPlayer()->IsSpecialAttack())
                {
                    // 攻撃2のほうに遷移
                    _owner->ChangeState("CombatAttack2");
                }
            }

            // 回避移行
            if (_owner->GetPlayer()->IsEvade())
            {
                _owner->ChangeState("CombatEvade");
            }
            // ガード移行
            else if (_owner->GetPlayer()->IsGuard())
            {
                _owner->ChangeState("CombatGuard");
            }
		}
	};
}
PlayerGreatSwordAttack1State::PlayerGreatSwordAttack1State(PlayerStateController* owner) :
    HierarchicalStateBase(owner)
{
    // サブステート登録
    RegisterSubState(std::make_shared<Attack1SubState::ChargeSubState>(owner,
        "ChargeAttack01Start",
        "ChargeAttack01Loop",
        "ChargeAttack01End",
        "AttackTackle",
        1.0f));
    RegisterSubState(std::make_shared<Attack1SubState::ComboSubState>(owner,
        "ChargeAttack01End",
        "ChargeAttack01End",
        "ChargeAttack02Start",
        "SpinningAttack",
        1.0f,
        0.3f, 0.0f, 0.15f));
    RegisterSubState(std::make_shared<Attack1SubState::ChargeSubState>(owner,
        "ChargeAttack02Start",
        "ChargeAttack02Loop",
        "ChargeAttack02End",
        "AttackTackle",
        1.0f));
    RegisterSubState(std::make_shared<Attack1SubState::ComboSubState>(owner,
        "ChargeAttack02End",
        "ChargeAttack02End",
        "ChargeAttack03Start",
        "SpinningAttack",
        1.0f,
        0.5f, 0.0f, 0.2f));
    RegisterSubState(std::make_shared<Attack1SubState::ChargeSubState>(owner,
        "ChargeAttack03Start",
        "ChargeAttack03Loop",
        "ChargeAttack03End",
        "AttackTackle",
        1.0f));
    RegisterSubState(std::make_shared<Attack1SubState::EndComboSubState>(owner));
    RegisterSubState(std::make_shared<Attack1SubState::ToRunSubState>(owner));
}

void PlayerGreatSwordAttack1State::OnEnter()
{
    // 先行入力遷移先をクリア
    _nextStateName = "";
    _owner->SetNextSubStateName("");
    _owner->SetNextBranchSubStateName("");
    _owner->GetAnimator()->SetIsUseRootMotion(true);
    // 初期サブステート設定
    ChangeSubState("ChargeAttack01Start");
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

        // 操作UI表示
        _owner->GetOperateUIController()->AddDescription(
            "溜め斬り",
            { "Action1" });
        _owner->GetOperateUIController()->AddDescription(
            "薙ぎ払い",
            { "Action2" });
        _owner->GetOperateUIController()->AddDescription(
            "回避",
            { "Evade" });
        _owner->GetOperateUIController()->AddDescription(
            "ガード",
            { "Guard" });
    }

    // 攻撃キャンセル判定
    if (_owner->GetPlayer()->CallCancelAttackEvent())
    {
        if (_nextStateName == "Attack" && !_owner->GetNextSubStateName().empty())
        {
            // キャンセル待機中に入力があった場合は先行入力遷移
            _owner->ChangeSubState(_owner->GetNextSubStateName());
            // 遷移後はクリア
            _nextStateName = "";
            return;
        }
        else if (_nextStateName == "SpecialAttack" && !_owner->GetNextBranchSubStateName().empty())
        {
            // 攻撃2のほうに遷移
            std::string nextSubState = _owner->GetNextBranchSubStateName();
            _owner->ChangeState("CombatAttack2");
            _owner->ChangeSubState(nextSubState);
            // 遷移後はクリア
            _nextStateName = "";
            return;
        }
    }

    // キャンセル判定
    if (_owner->GetPlayer()->CallCancelEvent())
    {
        if (!_nextStateName.empty())
            _owner->ChangeState(_nextStateName);
        // 移動移行
        else if (_owner->GetPlayer()->IsMoving())
        {
            _owner->ChangeSubState("ToRun");
            _nextStateName = "";
        }
    }
}
#pragma endregion

#pragma region 攻撃2
namespace Attack2SubState
{
    class TackleSubState : public Attack1SubState::ComboSubState
    {
    public:
        TackleSubState(PlayerStateController* owner,
            const std::string& name,
            const std::string& animationName,
            const std::string& nextSubStateName,
            float animationBlendTime) :
            Attack1SubState::ComboSubState(owner,
                name,
                animationName,
                nextSubStateName,
                "",
                animationBlendTime)
        {
        }
        void OnEnter() override
        {
            // 直前のサブステートの名前から次のサブステートを決定
            auto& str = _owner->GetStateMachine()->GetPreviousSubStateName();
            if (str == "ChargeAttack01Start")
            {
                // 前方突進攻撃
                _branchSubStateName = "ChargeAttack02Start";
            }
            if (str == "ChargeAttack02Start")
            {
                // 前方突進攻撃
                _branchSubStateName = "ChargeAttack03Start";
            }
            if (str == "ChargeAttack03Start")
            {
                // 前方突進攻撃
                _branchSubStateName = "ChargeAttack03Start";
            }
            ComboSubState::OnEnter();
        }
        void OnExecute(float elapsedTime) override
        {
            // 攻撃キーを押し続けている時
            if (_owner->GetPlayer()->CallCancelAttackEvent() && _owner->GetPlayer()->IsHoldingAttackKey())
            {
                _owner->GetPlayer()->SetIsAttack(true);
            }

            // 操作UI表示
            _owner->GetOperateUIController()->AddDescription(
                "溜め斬り",
                { "Action1" });
            _owner->GetOperateUIController()->AddDescription(
                "薙ぎ払い",
                { "Action2" });
        }
    };
}
PlayerGreatSwordAttack2State::PlayerGreatSwordAttack2State(PlayerStateController* owner) :
    HierarchicalStateBase(owner)
{
    // サブステート登録
    RegisterSubState(std::make_shared<Attack1SubState::ComboSubState>(owner,
        "SpinningAttack",
        "SpinningAttack",
        "",
        "ChargeAttack01Start",
        0.3f,
        0.5f, 0.0f, 0.2f));
    RegisterSubState(std::make_shared<Attack2SubState::TackleSubState>(owner,
        "AttackTackle",
        "AttackTackle",
        "",
        0.3f));
}

void PlayerGreatSwordAttack2State::OnEnter()
{
    // 先行入力遷移先をクリア
    _nextStateName = "";
    _owner->SetNextSubStateName("");
    _owner->SetNextBranchSubStateName("");
    _owner->GetAnimator()->SetIsUseRootMotion(true);
    // 初期サブステート設定
    ChangeSubState("SpinningAttack");
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
            if (_nextStateName == "Attack" && !_owner->GetNextBranchSubStateName().empty())
            {
                // 攻撃1のほうに遷移
				std::string nextSubState = _owner->GetNextBranchSubStateName();
                _owner->ChangeState("CombatAttack1");
                _owner->ChangeSubState(nextSubState);
                // 遷移後はクリア
                _nextStateName = "";
                return;
            }
            else if (_nextStateName == "SpecialAttack" && !_owner->GetNextSubStateName().empty())
            {
                // キャンセル待機中に入力があった場合は先行入力遷移
                _owner->ChangeSubState(_owner->GetNextSubStateName());
                // 遷移後はクリア
                _nextStateName = "";
                return;
            }
        }
    }

    // キャンセル判定
    if (_owner->GetPlayer()->CallCancelEvent())
    {
        if (!_nextStateName.empty())
            _owner->ChangeState(_nextStateName);
        // 移動移行
        else if (_owner->GetPlayer()->IsMoving())
        {
            _owner->ChangeState("CombatAttack1");
            _owner->ChangeSubState("ToRun");
            _nextStateName = "";
        }
    }
    else
    {
        // キャンセルがかかるまでの間は移動方向に向く
        _owner->RotationMovement(elapsedTime);
    }

    // アニメーションが終了していたら遷移
    if (!_owner->GetAnimator()->IsPlaying())
    {
        // 攻撃からIdleに遷移
        _owner->ChangeState("CombatIdle");
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
        GuardStartSubState(PlayerStateController* owner) :
            PlayerSSB(owner,
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
            if (!_owner->GetAnimator()->IsPlaying())
                _owner->ChangeSubState("Guarding");
            // ガード解除
            else if (!_owner->GetPlayer()->IsGuard())
                _owner->ChangeSubState("GuardEnd");
        }
    };
    // ガード中
    class GuardingSubState final : public PlayerSSB
    {
    public:
        GuardingSubState(PlayerStateController* owner) :
            PlayerSSB(owner,
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
                _owner->ChangeSubState("GuardEnd");
        }
    };
    // ガード終了
    class GuardEndSubState final : public PlayerSSB
    {
    public:
        GuardEndSubState(PlayerStateController* owner) :
            PlayerSSB(owner,
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
            if (!_owner->GetAnimator()->IsPlaying())
                _owner->ChangeState("CombatIdle");
        }
    };
    // ガード時被弾
    class GuardHitSubState final : public PlayerSSB
    {
    public:
        GuardHitSubState(PlayerStateController* owner) :
            PlayerSSB(owner,
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
            if (!_owner->GetAnimator()->IsPlaying())
                _owner->ChangeSubState("Guarding");
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

PlayerGreatSwordGuardState::PlayerGreatSwordGuardState(PlayerStateController* owner) :
    HierarchicalStateBase(owner)
{
    // サブステート登録
    RegisterSubState(std::make_shared<GuardSubState::GuardStartSubState>(owner));
    RegisterSubState(std::make_shared<GuardSubState::GuardingSubState>(owner));
    RegisterSubState(std::make_shared<GuardSubState::GuardHitSubState>(owner));
    RegisterSubState(std::make_shared<GuardSubState::GuardEndSubState>(owner));
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
        _owner->ChangeState("CombatIdle");

    // 移動方向に向く
    _owner->RotationMovement(elapsedTime);

    // 攻撃移行
    if (_owner->GetPlayer()->IsAttack())
        _owner->ChangeState("CombatAttack1");
    else if (_owner->GetPlayer()->IsSpecialAttack())
        _owner->ChangeState("CombatAttack2");
    // 回避移行
    else if (_owner->GetPlayer()->IsEvade())
        _owner->ChangeState("CombatEvade");

    // 操作UI表示
    _owner->GetOperateUIController()->AddDescription(
        "溜め斬り",
        { "Action1" });
    _owner->GetOperateUIController()->AddDescription(
        "薙ぎ払い",
        { "Action2" });
    _owner->GetOperateUIController()->AddDescription(
        "回避",
        { "Evade" });
    _owner->GetOperateUIController()->AddDescription(
        "ガード解除",
        { "Guard" });
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
PlayerGreatSwordHitState::PlayerGreatSwordHitState(PlayerStateController* owner) :
    Player8WayHSB(
        owner,
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
    if (!_owner->GetAnimator()->IsPlaying())
        _owner->ChangeState("CombatIdle");

    // 操作UI表示
    _owner->GetOperateUIController()->AddDescription(
        "溜め斬り",
        { "Action1" });
    _owner->GetOperateUIController()->AddDescription(
        "薙ぎ払い",
        { "Action2" });
    _owner->GetOperateUIController()->AddDescription(
        "回避",
        { "Evade" });
    _owner->GetOperateUIController()->AddDescription(
        "ガード",
        { "Guard" });
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

PlayerGreatSwordHitKnockDownState::PlayerGreatSwordHitKnockDownState(PlayerStateController* owner) :
    Player8WayHSB(
        owner,
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
    if (!_owner->GetAnimator()->IsPlaying())
        _owner->ChangeState("CombatIdle");

    // 操作UI表示
    _owner->GetOperateUIController()->AddDescription(
        "溜め斬り",
        { "Action1" });
    _owner->GetOperateUIController()->AddDescription(
        "薙ぎ払い",
        { "Action2" });
    _owner->GetOperateUIController()->AddDescription(
        "回避",
        { "Evade" });
    _owner->GetOperateUIController()->AddDescription(
        "ガード",
        { "Guard" });
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
    if (!_owner->GetAnimator()->IsPlaying())
        _owner->ChangeToNonCombatState("Idle");
}
#pragma endregion

#pragma region ダウン
namespace PlayerGreatSwordDownSubState
{
    class DownStartSubState : public PlayerSSB
    {
    public:
        DownStartSubState(PlayerStateController* owner) :
            PlayerSSB(owner,
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
            if (!_owner->GetAnimator()->IsPlaying())
            {
                if (_owner->GetPlayer()->IsMoving())
                {
                    _owner->ChangeSubState("DownEnd");
                    return;
                }
            }
        }
    };
    class DownEndSubState : public PlayerSSB
    {
    public:
        DownEndSubState(PlayerStateController* owner) :
            PlayerSSB(owner,
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
            if (!_owner->GetAnimator()->IsPlaying())
                _owner->ChangeState("Idle");
            // キャンセルイベントが呼ばれたら
            if (_owner->GetPlayer()->CallCancelEvent())
            {
                // 移動移行
                if (_owner->GetPlayer()->IsMoving())
                    _owner->ChangeState("CombatRun");
                // ガード移行
                else if (_owner->GetPlayer()->IsGuard())
                    _owner->ChangeState("CombatGuard");
            }
        }
    };
}
PlayerGreatSwordDownState::PlayerGreatSwordDownState(PlayerStateController* owner) :
    HierarchicalStateBase(owner)
{
    RegisterSubState(std::make_shared<PlayerGreatSwordDownSubState::DownStartSubState>(owner));
    RegisterSubState(std::make_shared<PlayerGreatSwordDownSubState::DownEndSubState>(owner));
}

void PlayerGreatSwordDownState::OnEnter()
{
    // ダメージを受けたアクターの方向を向く
    Vector3 hitPosition = _owner->GetPlayer()->GetDamageable()->GetHitPosition();
    if (auto damageSender = _owner->GetPlayer()->GetDamageable()->GetLastDamageActor())
        hitPosition = damageSender->GetTransform().GetPosition();
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

    // 操作UI表示
    _owner->GetOperateUIController()->AddDescription(
        "溜め斬り",
        { "Action1" });
    _owner->GetOperateUIController()->AddDescription(
        "薙ぎ払い",
        { "Action2" });
    _owner->GetOperateUIController()->AddDescription(
        "回避",
        { "Evade" });
    _owner->GetOperateUIController()->AddDescription(
        "ガード",
        { "Guard" });
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
