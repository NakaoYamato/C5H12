#pragma once

#include "../../Library/Component/Component.h"
#include "../../Library/Component/Collider/ModelCollider.h"
#include "../../Library/Component/EffectController.h"
#include "../../Source/Common/Targetable.h"

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
	// ヘイト倍率のセット
	void SetHateFactor(float hateFactor) { _heteFactor = hateFactor; }
	// テキスト描画するかどうか
	void SetDrawText(bool f) { _drawText = f; }

	// 最終攻撃力の取得
	float GetATK() const { return _ATK; }
#pragma endregion

private:
	// 攻撃力の計算
	// hitzoneFactor : 肉質倍率
	void CalculateATK(float hitzoneFactor);

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