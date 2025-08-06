#include "PlayerController.h"

#include "../../Library/Scene/Scene.h"
#include "../../Library/DebugSupporter/DebugSupporter.h"

#include "../../Source/Enemy/EnemyController.h" 
#include "../../Source/Common/Damageable.h"

#include <imgui.h>

void PlayerController::Start()
{
	_charactorController = GetActor()->GetComponent<CharactorController>();
	_animator = GetActor()->GetComponent<Animator>();
	_hitEffectController = GetActor()->GetComponent<EffekseerEffectController>();
	_damageable = GetActor()->GetComponent<Damageable>();
	_targetable = GetActor()->GetComponent<Targetable>();

	auto stateController = GetActor()->GetComponent<StateController>();
	_stateMachine = std::dynamic_pointer_cast<PlayerStateMachine>(stateController->GetStateMachine());

	_damageable.lock()->SetTakeableDamageCallback(
		[&](float damage, Vector3 hitPosition) -> bool
		{
			Vector3 vec = hitPosition - GetActor()->GetTransform().GetPosition();
			Vector3 front = GetActor()->GetTransform().GetAxisZ();
			// プレイヤーがガード状態ならダメージを受けない
			if (_stateMachine.lock()->GetStateName() == "Guard" &&
				vec.Dot(front) > 0.0f)
			{
				// ガード成功
				_stateMachine.lock()->ChangeState("GuardHit", nullptr);
				return false;
			}
			return true;
		}
	);
	_damageable.lock()->SetOnDamageCallback(
		[&](float damage, Vector3 hitPosition)
		{
			if (damage >= 2.0f)
			{
				// 大きくのけぞる
				_stateMachine.lock()->ChangeState("HitKnockDown", nullptr);
			}
			else
			{
				// 軽くのけぞる
				_stateMachine.lock()->ChangeState("Hit", nullptr);
			}
		}
	);

	_charactorController.lock()->SetMaxSpeedXZ(5.0f);
}

// 更新処理
void PlayerController::Update(float elapsedTime)
{
	auto animator = _animator.lock();

	// 攻撃力の更新
	_ATK = _BaseATK * _ATKFactor;

	_callInputBufferingEvent = false;
	_callCancelAttackEvent = false;
	_callCancelEvent = false;
	_oldInvisibleEvent = _callInvisivleEvent;
	_callInvisivleEvent = false;

	// アニメーションイベント取得
	if (animator->IsPlayAnimation())
	{
		auto& animationEvent = animator->GetAnimationEvent();
		int massageListSize = (int)animationEvent.GetMessageList().size();
		auto events = animator->GetCurrentEvents();
		for (auto& event : events)
		{
			// メッセージインデックスが範囲外ならcontinue
			if (event.messageIndex < 0 || event.messageIndex >= massageListSize)
				continue;

            // 先行入力判定
			if (animationEvent.GetMessageList().at(event.messageIndex) == "InputBuffering")
			{
				_callInputBufferingEvent = true;
			}
			// 攻撃キャンセル判定
			if (animationEvent.GetMessageList().at(event.messageIndex) == "CancelAttack")
			{
				_callCancelAttackEvent = true;
			}
			// キャンセル判定
			if (animationEvent.GetMessageList().at(event.messageIndex) == "Cancel")
			{
				_callCancelEvent = true;
			}
			// 無敵判定
			if (animationEvent.GetMessageList().at(event.messageIndex) == "Invisible")
			{
				_callInvisivleEvent = true;
			}
		}
	}
}

// 3D描画後の描画処理
void PlayerController::DelayedRender(const RenderContext& rc)
{
	// 頭上に名前表示
	GetActor()->GetScene()->GetTextRenderer().Draw3D(
		FontType::MSGothic,
		GetActor()->GetName(),
		GetActor()->GetTransform().GetPosition() + Vector3(0.0f, 2.0f, 0.0f),
		Vector4::White
	);
}

// GUI描画
void PlayerController::DrawGui()
{
	ImGui::DragFloat(u8"攻撃力",		&_ATK, 0.1f, 0.0f, 100.0f);
	ImGui::DragFloat(u8"基本攻撃力", &_BaseATK, 0.1f, 0.0f, 100.0f);
	ImGui::DragFloat(u8"攻撃倍率",	&_ATKFactor, 0.01f, 0.0f, 10.0f);

	ImGui::Separator();
}

// 接触時処理
void PlayerController::OnContactEnter(CollisionData& collisionData)
{
	// 攻撃判定
	if (collisionData.myLayer == CollisionLayer::Attack)
	{
		// ダメージを与える
		auto damageable = collisionData.other->GetComponent<Damageable>();
		if (damageable != nullptr)
		{
			if (damageable->AddDamage(_ATK, collisionData.hitPosition))
			{
				// ダメージを与えたらヒットエフェクト再生
				_hitEffectController.lock()->Play(collisionData.hitPosition, 1.0f);
				// 自身のヘイト値を増やす
				_targetable.lock()->AddHateValue(_ATK);
			}
		}
	}
}
