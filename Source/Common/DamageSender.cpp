#include "DamageSender.h"

#include "../../Source/Common/Damageable.h"
#include "../../Library/Scene/Scene.h"

#include <imgui.h>

// 開始処理
void DamageSender::Start()
{
	_modelCollider = GetActor()->GetCollider<ModelCollider>();
	_effectController = GetActor()->GetComponent<EffectController>();
	_targetable = GetActor()->GetComponent<Targetable>();
}

// 更新処理
void DamageSender::Update(float elapsedTime)
{
	if (auto modelCollider = _modelCollider.lock())
	{
		// 攻撃イベントが呼び出されていなければリセット
		if (!modelCollider->IsCollAttackEvent())
			ResetAttackState();
	}

	// ステージエフェクトタイマー更新
	if (_stageEffectTimer > 0.0f)
	{
		_stageEffectTimer = std::max<float>(0.0f, _stageEffectTimer - elapsedTime);
	}

	// ダメージテキストの表示
	auto& textRenderer = GetActor()->GetScene()->GetTextRenderer();
	for (auto it = _damageTexts.begin(); it != _damageTexts.end(); )
	{
		auto& text = *it;

		// タイマー更新
		text.timer += elapsedTime;
		if (text.timer >= _textRemoveTime)
		{
			// 期限切れなら削除
			it = _damageTexts.erase(it);
			continue;
		}

		// 位置調整
		Vector3 textPosition = text.position;
		textPosition.y += (text.timer * _textRiseSpeed);
		// 色設定
		Vector4 textColor = text.isCritical ? _textCriticalColor : _textNormalColor;
		// 透明度設定
		textColor.w = 1.0f - (text.timer / _textRemoveTime);

		// テキスト描画
		textRenderer.Draw3D(
			FontType::MSGothic,
			std::to_string(static_cast<int>(text.damageAmount)).c_str(),
			textPosition,
			textColor,
			0.0f,
			Vector2::Zero,
			_textScale);

		// 次の要素へ進む
		++it;
	}
}

// GUI描画
void DamageSender::DrawGui()
{
	ImGui::DragFloat(u8"基本攻撃力", &_baseATK, 0.1f, 0.0f, 100.0f);
	ImGui::DragFloat(u8"モーション値", &_motionFactor, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat(u8"攻撃倍率", &_sharpnessFactor, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat(u8"会心倍率", &_criticalFactor, 0.01f, 0.0f, 10.0f);
	ImGui::DragInt(u8"ヒットエフェクトインデックス", &_hitEffectIndex, 1, -1, 100);
	ImGui::DragFloat(u8"ヘイト倍率", &_heteFactor, 0.01f, 0.0f, 10.0f);
	ImGui::Separator();

	ImGui::Checkbox(u8"ステージエフェクト再生", &_isPlayStageEffect);
	ImGui::Checkbox(u8"ステージカメラシェイク", &_isShakeOnStageContact);
	ImGui::DragFloat(u8"ステージエフェクト生成間隔", &_stageEffectInterval, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat(u8"ステージエフェクトモーション影響度", &_stageEffectMotionFactor, 0.01f, 0.0f, 10.0f);
	ImGui::DragFloat(u8"ステージシェイクモーション影響度", &_stageShakeMotionFactor, 0.01f, 0.0f, 10.0f);
	ImGui::Separator();

	ImGui::Checkbox(u8"ダメージテキスト表示", &_drawText);
	ImGui::ColorEdit4(u8"ダメージテキスト通常色", &_textNormalColor.x);
	ImGui::ColorEdit4(u8"ダメージテキスト会心色", &_textCriticalColor.x);
	ImGui::DragFloat(u8"ダメージテキスト上昇速度", &_textRiseSpeed, 0.1f, 0.0f, 100.0f);
	ImGui::DragFloat(u8"ダメージテキスト表示時間", &_textRemoveTime, 0.1f, 0.1f, 10.0f);
	ImGui::DragFloat2(u8"ダメージテキストスケール", &_textScale.x, 0.1f, 0.1f, 10.0f);
}

// 接触時処理
void DamageSender::OnContact(CollisionData& collisionData)
{
	// エフェクト判定
	// ステージとの接触処理
	if (_isPlayStageEffect && collisionData.myLayer == CollisionLayer::Effect)
	{
		if (collisionData.otherLayer == CollisionLayer::Stage)
		{
			if (auto stageEffectEmitter = collisionData.other->GetComponent<StageEffectEmitter>())
			{
				ContactStage(stageEffectEmitter.get(), collisionData);
				return;
			}
		}
	}

	// 攻撃判定
	if (collisionData.myLayer == CollisionLayer::Attack)
	{
		// 攻撃先情報を取得
		// もし親がいるならその親の名前で取得
		std::string targetName = collisionData.other->GetParent() ?
			collisionData.other->GetParent()->GetName() :
			collisionData.other->GetName();

		// 以前に攻撃していた相手には攻撃しない
		if (_attackedTargets.find(targetName) != _attackedTargets.end())
			return;

		// ダメージを与える
		auto damageable = collisionData.other->GetComponent<Damageable>();
		if (damageable != nullptr)
		{
			// 攻撃力計算
			CalculateATK(1.0f/*TODO : 肉質実装*/);

			if (damageable->AddDamage(_ATK, collisionData.hitPosition))
			{
				damageable->SetLastDamageActor(GetActor());

				// ダメージを与えたらヒットエフェクト再生
				if (_effectController.lock() && _hitEffectIndex != -1)
					_effectController.lock()->Play(_hitEffectIndex, collisionData.hitPosition);
				// 自身のヘイト値を増やす
				_targetable.lock()->AddHateValue(_ATK);

				// 攻撃先情報を保存
				_attackedTargets[targetName] = collisionData.other;

				if (_drawText)
				{
					// ダメージテキスト追加
					DamageText damageText;
					damageText.position = collisionData.hitPosition;
					damageText.damageAmount = _ATK;
					damageText.isCritical = (_criticalFactor > 1.0f);
					_damageTexts.push_back(damageText);
				}
			}
		}
	}
}

// 攻撃力の計算
void DamageSender::CalculateATK(float hitzoneFactor)
{
	_ATK = _baseATK * _motionFactor * _sharpnessFactor * hitzoneFactor * _criticalFactor;
}

// ステージとの接触処理
void DamageSender::ContactStage(StageEffectEmitter* target, CollisionData& collisionData)
{
	if (_stageEffectTimer > 0.0f)
		return;

	float effectScale = std::clamp<float>(std::powf(_motionFactor * _stageEffectMotionFactor, 2.0f), 0.0f, 1.0f);
	float shakePower = std::clamp<float>(std::powf(_motionFactor * _stageShakeMotionFactor, 2.0f), 0.0f, 1.0f);

	// エフェクト再生
	target->PlayEffect(collisionData.hitPosition, Vector3::Zero, effectScale, shakePower);

	// タイマー設定
	_stageEffectTimer = _stageEffectInterval;

	// コールバック
	if (_onStageContactCallback)
	{
		_onStageContactCallback(target, collisionData);
	}
}
