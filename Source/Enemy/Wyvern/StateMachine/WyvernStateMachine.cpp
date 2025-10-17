#include "WyvernStateMachine.h"

#include "../../Library/Component/Animator.h"
#include "../WyvernController.h"
#include "WyvernMainStates.h"

#include <imgui.h>

WyvernStateMachine::WyvernStateMachine(Actor* owner)
{
	// コンポーネント取得
	_enemy = owner->GetComponent<EnemyController>().get();
	_wyvern = owner->GetComponent<WyvernController>().get();
	_animator = owner->GetComponent<Animator>().get();
	_damageable = owner->GetComponent<Damageable>().get();
	_combatStatus = owner->GetComponent<CombatStatusController>().get();
	_roarController = owner->GetComponent<RoarController>().get();

	// ステートの登録
	_stateMachine.RegisterState(std::make_unique<WyvernIdleState>(this));
	_stateMachine.RegisterState(std::make_unique<WyvernThreatState>(this));

	_stateMachine.RegisterState(std::make_unique<WyvernRoarState>(this));
	_stateMachine.RegisterState(std::make_unique<WyvernToTargetState>(this));
	_stateMachine.RegisterState(std::make_unique<WyvernTurnState>(this));

	_stateMachine.RegisterState(std::make_unique<WyvernBiteAttackState>(this));
	_stateMachine.RegisterState(std::make_unique<WyvernClawAttackState>(this));
	_stateMachine.RegisterState(std::make_unique<WyvernTailAttackState>(this));
	_stateMachine.RegisterState(std::make_unique<WyvernChargeAttackState>(this));
	_stateMachine.RegisterState(std::make_unique<WyvernBreathAttackState>(this));
	_stateMachine.RegisterState(std::make_unique<WyvernFireBallAttackState>(this));

	_stateMachine.RegisterState(std::make_unique<WyvernBackJumpFireBallAttackState>(this));
	
	_stateMachine.RegisterState(std::make_unique<WyvernHoverState>(this));

	_stateMachine.RegisterState(std::make_unique<WyvernBackStepState>(this));
	_stateMachine.RegisterState(std::make_unique<WyvernPursuitState>(this));

	_stateMachine.RegisterState(std::make_unique<WyvernDamageState>(this));
	_stateMachine.RegisterState(std::make_unique<WyvernDeathState>(this));
}

// 開始処理
void WyvernStateMachine::Start()
{
	// ルートモーション設定
	GetAnimator()->SetRootNodeIndex("CG");
	GetAnimator()->SetRootMotionOption(Animator::RootMotionOption::RemovePositionX);
	GetAnimator()->SetRootOffset(Vector3(0.0f, 200.0f, 0.0f));

	// 初期ステートを設定
	_stateMachine.ChangeState("Idle");
}

void WyvernStateMachine::Execute(float elapsedTime)
{
	bool airFlag = false;
	float blurRate = 0.0f;
    _callCancelEvent = false;
    _callFireBreath = false;
	_callFireBall = false;
    _callLookAtTarget = false;

    // アニメーションイベント取得
    if (GetAnimator()->IsPlayAnimation())
    {
        auto& animationEvent = GetAnimator()->GetAnimationEvent();
        int massageListSize = (int)animationEvent.GetMessageList().size();
        auto events = GetAnimator()->GetCurrentEvents();
        for (auto& event : events)
        {
            // メッセージインデックスが範囲外ならcontinue
            if (event.messageIndex < 0 || event.messageIndex >= massageListSize)
                continue;

            // キャンセル判定
            if (animationEvent.GetMessageList().at(event.messageIndex) == "Cancel")
            {
                _callCancelEvent = true;
            }
            // ブレス攻撃判定
			else if (animationEvent.GetMessageList().at(event.messageIndex) == "FireBreath")
			{
				_callFireBreath = true;
                // ブレス攻撃のグローバル位置を設定
				_breathGlobalPosition = event.position;
			}
			// 火球攻撃判定
			else if (animationEvent.GetMessageList().at(event.messageIndex) == "FireBall")
			{
				_callFireBall = true;
				// 火球攻撃のグローバル位置を設定
				_fireBallGlobalPosition = event.position;
			}
            // ルックアット判定
			else if (animationEvent.GetMessageList().at(event.messageIndex) == "LookAtTarget")
			{
                _callLookAtTarget = true;
			}
			// 咆哮判定
			else if (animationEvent.GetMessageList().at(event.messageIndex) == "Roar")
			{
				blurRate = 1.0f - (GetAnimator()->GetAnimationTimer() - event.startSeconds) / (event.endSeconds - event.startSeconds);
			}
			// 飛行判定
			else if (animationEvent.GetMessageList().at(event.messageIndex) == "DuringFlight")
			{
				airFlag = true;
			}
        }
    }

    // 被弾処理
	if (_enemy->IsPerformDamageReaction())
	{
        // 被弾遷移
		_stateMachine.ChangeState("Damage");
		_enemy->SetPerformDamageReaction(false);
	}

    // 死亡処理
    if (GetDamageable()->IsDead() && _stateMachine.GetStateName() != "Death")
    {
        // 死亡遷移
		_stateMachine.ChangeState("Death");
    }

	// ターゲットを向く処理
    if (_callLookAtTarget)
    {
  //      // 頭のノード取得
  //      auto model = GetWyvern()->GetActor()->GetModel().lock();
  //      int nodeIndex = model->GetNodeIndex("Head");
		//auto& headNode = model->GetPoseNodes()[nodeIndex];
  //      // 頭の親ノード取得
  //      auto headParentNode = model->GetPoseNodes()[nodeIndex].parent;

  //      DirectX::XMMATRIX headParentInvTransform = DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&headParentNode->worldTransform));

		//// ターゲット方向に向く
  //      Quaternion nextQ = Quaternion::LookAt(
  //          Vector3::Zero,
  //          Vector3::Up,
  //          Vector3::TransformCoord(GetWyvern()->GetTargetPosition() + _headRotationOffset, headParentInvTransform));

  //      // 回転制限
		//Vector3 euler = Vector3::NormalizeEuler(Quaternion::ToRollPitchYaw(nextQ));
		//Vector3 parentEuler = Vector3::NormalizeEuler(Quaternion::ToRollPitchYaw(headParentNode->rotation));
		//// X軸方向の回転制限
		//float limitX = DirectX::XMConvertToRadians(_headRotationLimitX);
		//// X軸回転量が制限を超えている場合は制限値に設定し親に反映
  //      if (euler.x > limitX || euler.x < -limitX)
  //      {
		//	if (euler.x > 0.0f)
  //              parentEuler.x += euler.x - limitX;
		//	else
  //              parentEuler.x += euler.x - limitX;
  //          euler.x = std::clamp(euler.x, -limitX, limitX);
  //      }
		//euler.y = 0.0f; // Y軸方向の回転は制限しない
		//// 親ノードの回転値を更新
		//headParentNode->rotation = Quaternion::FromRollPitchYaw(parentEuler);
  //      nextQ = Quaternion::FromRollPitchYaw(euler);

		//headNode.rotation = nextQ;

  //      // 行列更新
		//model->UpdateNodeTransform(headParentNode);
    }

	// 咆哮のブラー処理
	if (blurRate > 0.0f)
	{
		// ワイバーンの頭の位置を取得
		auto model = GetEnemy()->GetActor()->GetModel().lock();
		int nodeIndex = model->GetNodeIndex("Head");
		Vector3 headWorldPosition =
		{
			model->GetPoseNodes()[nodeIndex].worldTransform._41,
			model->GetPoseNodes()[nodeIndex].worldTransform._42,
			model->GetPoseNodes()[nodeIndex].worldTransform._43
		};
		_roarController->SetWorldPosition(headWorldPosition);
	}
	_roarController->SetRate(blurRate);

	// 空中処理
	GetWyvern()->SetIsDuringFlight(airFlag);

	// ステートマシンの実行
    _stateMachine.Update(elapsedTime);
}

void WyvernStateMachine::DrawGui()
{
    if (ImGui::TreeNode(u8"ステートマシン"))
    {
		ImGui::DragFloat(u8"頭の回転制限角度", &_headRotationLimitX, 1.0f, 0.0f, 90.0f, "%.1f°");

        ImGui::Separator();
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

void WyvernStateMachine::ChangeState(const char* mainStateName, const char* subStateName)
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

const char* WyvernStateMachine::GetStateName()
{
    if (!_stateMachine.GetState())
		return "";

    return _stateMachine.GetState()->GetName();
}

const char* WyvernStateMachine::GetSubStateName()
{
    if (!_stateMachine.GetState())
        return "";

    return _stateMachine.GetState()->GetSubStateName();
}
