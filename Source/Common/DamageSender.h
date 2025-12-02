#pragma once

#include "../../Library/Component/Component.h"
#include "../../Library/Component/Collider/ModelCollider.h"
#include "../../Library/Component/EffectController.h"
#include "../../Source/Common/Targetable.h"
#include "../../Library/Component/StageEffectEmitter.h"

#include <functional>

class DamageSender : public Component
{
public:
	struct DamageText
	{
		Vector3 position;
		float timer = 0.0f;
		float damageAmount = 0.0f;
		bool isCritical = false;
	};

public:
	DamageSender() {}
	~DamageSender() override {}
	// 名前取得
	const char* GetName() const override { return "DamageSender"; }
	// 開始処理
	void Start() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// GUI描画
	void DrawGui() override;
	// 接触時処理
	void OnContact(CollisionData& collisionData) override;

	// 攻撃情報のリセット
	void ResetAttackState()
	{
		_attackedTargets.clear();
	}
#pragma region アクセサ
	// 基本攻撃力のセット
	void SetBaseATK(float baseATK) { _baseATK = baseATK; }
	// モーション値のセット
	void SetMotionFactor(float motionFactor) { _motionFactor = motionFactor; }
	// 攻撃倍率のセット
	void SetSharpnessFactor(float sharpnessFactor) { _sharpnessFactor = sharpnessFactor; }
	// 会心倍率のセット
	void SetCriticalFactor(float criticalFactor) { _criticalFactor = criticalFactor; }
	// ヒットエフェクトのセット
	void SetHitEffectIndex(int index) { _hitEffectIndex = index; }
	// ステージエフェクトを再生するかどうかのセット
	void SetPlayStageEffect(bool f) { _isPlayStageEffect = f; }
	// ステージ接触時にカメラシェイクを行うかどうかのセット
	void SetShakeOnStageContact(bool f) { _isShakeOnStageContact = f; }
	// ステージエフェクト生成間隔のセット
	void SetStageEffectInterval(float interval) { _stageEffectInterval = interval; }
	// モーション値によるステージエフェクトスケールの影響度のセット
	void SetStageEffectMotionFactor(float factor) { _stageEffectMotionFactor = factor; }
	// モーション値によるステージエフェクトカメラシェイク影響度のセット
	void SetStageShakeMotionFactor(float factor) { _stageShakeMotionFactor = factor; }
	// ヘイト倍率のセット
	void SetHateFactor(float hateFactor) { _heteFactor = hateFactor; }
	// テキスト描画するかどうか
	void SetDrawText(bool f) { _drawText = f; }
	// ダメージを与えた時のコールバック関数のセット
	void SetOnSendDamageCallback(std::function<void(DamageSender*, CollisionData&)> callback)
	{
		_onSendDamageCallback = callback;
	}
	// ステージとの接触時のコールバック関数のセット
	void SetOnStageContactCallback(std::function<void(StageEffectEmitter*, CollisionData&)> callback)
	{
		_onStageContactCallback = callback;
	}


	// 基本攻撃力の取得
	float GetBaseATK() const { return _baseATK; }
	// モーション値の取得
	float GetMotionFactor() const { return _motionFactor; }
	// 攻撃倍率の取得
	float GetSharpnessFactor() const { return _sharpnessFactor; }
	// 会心倍率の取得
	float GetCriticalFactor() const { return _criticalFactor; }

	// 最終攻撃力の取得
	float GetATK() const { return _ATK; }
#pragma endregion

private:
	// 攻撃力の計算
	// hitzoneFactor : 肉質倍率
	void CalculateATK(float hitzoneFactor);

	// ステージとの接触処理
	void ContactStage(StageEffectEmitter* target, CollisionData& collisionData);

private:
#pragma region 攻撃力決定用パラメータ
	float _baseATK			= 1.0f;	// 基本攻撃力
	float _motionFactor		= 1.0f;	// モーション値
	float _sharpnessFactor	= 1.0f;	// 攻撃倍率
	float _criticalFactor	= 1.5f;	// 会心倍率

	float _ATK = 1.0f;			// 最終攻撃力
#pragma endregion

#pragma region ヒットエフェクト
	std::weak_ptr<EffectController> _effectController;
	int _hitEffectIndex = -1;

	// ステージエフェクトを再生するかどうか
	bool _isPlayStageEffect = true;
	// ステージとの接触でカメラシェイクを行うかどうか
	bool _isShakeOnStageContact = true;
	// ステージエフェクトの生成間隔
	float _stageEffectInterval = 0.03f;
	float _stageEffectTimer = 0.0f;
	// モーション値によるステージエフェクトスケールの影響度
	float _stageEffectMotionFactor = 1.0f;
	// モーション値によるステージエフェクトカメラシェイク影響度
	float _stageShakeMotionFactor = 1.0f;

	// ダメージを与えた時のコールバック関数
	std::function<void(DamageSender*, CollisionData&)> _onSendDamageCallback;
	// ステージとの接触時のコールバック関数
	std::function<void(StageEffectEmitter*, CollisionData&)> _onStageContactCallback;
#pragma endregion

#pragma region ヘイト値関係
	std::weak_ptr<Targetable> _targetable;
	float _heteFactor = 1.0f;	// ヘイト倍率
#pragma endregion

	std::weak_ptr<ModelCollider> _modelCollider;
	// 攻撃先情報
	std::unordered_map<std::string, Actor*> _attackedTargets;
	
	bool _drawText = false;
	std::vector<DamageText> _damageTexts;
	Vector4 _textNormalColor = Vector4::Orange;
	Vector4 _textCriticalColor = Vector4::Red;
	Vector2 _textScale = Vector2(1.5f, 1.5f);
	float _textRemoveTime = 1.0f;
	float _textRiseSpeed = 0.25f;
};