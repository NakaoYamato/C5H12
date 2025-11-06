#pragma once

#include "../../Library/Component/Component.h"
#include "../../Library/Component/CharactorController.h"
#include "../../Library/Component/Animator.h"
#include "../../Library/Component/EffectController.h"
#include "../../Library/Component/ModelRenderer.h"

#include "../../Source/Common/Damageable.h"
#include "../../Source/Common/Targetable.h"
#include "StateMachine/PlayerStateMachine.h"

class PlayerController : public Component
{
public:
	enum EffectType : UINT
	{
		HitEffect = 0,
		Charge0,
		Charge1,
		Charge2,

		MaxEffect
	};

public:
	PlayerController() {}
	~PlayerController() override {}

	// 名前取得
	const char* GetName() const override { return "PlayerController"; }

	// 開始処理
	void Start() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// 3D描画後の描画処理
	void DelayedRender(const RenderContext& rc) override;
	// GUI描画
	void DrawGui() override;

	// 接触処理
	void OnContact(CollisionData& collisionData) override;

#pragma region アクセサ
	// キャラクターコントローラー取得
	std::shared_ptr<CharactorController> GetCharactorController()
	{
		return _charactorController.lock();
	}
	std::shared_ptr<Damageable> GetDamageable()
	{
		return _damageable.lock();
	}
#pragma endregion

#pragma region アクセサ
	// 先行入力イベントを取得
	bool CallInputBufferingEvent() const { return _callInputBufferingEvent; }
	// 攻撃キャンセルイベントを取得
	bool CallCancelAttackEvent() const { return _callCancelAttackEvent; }
	// キャンセルイベントを取得
	bool CallCancelEvent() const { return _callCancelEvent; }
	// 無敵時間
	bool CallInvisivleEvent() const { return _callInvisivleEvent; }
	// 溜めイベント取得
	bool CallChargingEvent() const { return _callChargingEvent; }
	bool OldInvisibleEvent() const { return _oldInvisibleEvent; }
	const Vector2& GetMovement() const { return _movement; }
	bool IsMoving()	const { return _isMoving; }
	bool IsDash()	const { return _isDash; }
	bool IsEvade()	const { return _isEvade; }
	bool IsInCombat() const { return _isInCombat; }
	bool IsAttack() const { return _isAttack; }
	bool IsHoldingAttackKey() const { return _isHoldingAttackKey; }
	bool IsSpecialAttack() const { return _isSpecialAttack; }
	bool IsGuard()	const { return _isGuard; }
	bool IsDead()	const { return _isDead; }
	bool IsUsingItem() const { return _isUsingItem; }
	bool IsSelect()	const { return _isSelect; }
	int GetChargeLevel() const { return _chargeLevel; }

	void SetMovement(const Vector2& movement) { _movement = movement; }
	void SetIsMoving(bool isMoving) { _isMoving = isMoving; }
	void SetIsDash(bool isDush) { _isDash = isDush; }
	void SetIsEvade(bool isEvade) { _isEvade = isEvade; }
	void SetIsInCombat(bool isInCombat) { _isInCombat = isInCombat; }
	void SetIsAttack(bool isAttack) { _isAttack = isAttack; }
	void SetIsHoldingAttackKey(bool isHoldingAttackKey) { _isHoldingAttackKey = isHoldingAttackKey; }
	void SetIsSpecialAttack(bool isSpecialAttack) { _isSpecialAttack = isSpecialAttack; }
	void SetIsGuard(bool isGuard) { _isGuard = isGuard; }
	void SetIsDead(bool isDead) { _isDead = isDead; }
	void SetIsUsingItem(bool isUsingItem) { _isUsingItem = isUsingItem; }
	void SetIsSelect(bool isSelect) { _isSelect = isSelect; }
	void SetChargeLevel(int chargeLevel) { _chargeLevel = chargeLevel; }

    float GetDashRotationFactor() const { return _dashRotationFactor; }

    float GetDashStaminaConsume() const { return _dashStaminaConsume; }
    float GetEvadeStaminaConsume() const { return _evadeStaminaConsume; }
    float GetGuardStaminaConsume() const { return _guardStaminaConsume; }

	// 溜め演出時のリムライト表示
	void StartChargeEffectRimLight()
	{
		_chargeEffectRimLightTimer = _chargeEffectRimLightTimeMax;
	}
#pragma endregion

private:
	std::weak_ptr<PlayerStateMachine> _stateMachine;
	std::weak_ptr<CharactorController> _charactorController;
	std::weak_ptr<Animator> _animator;
	std::weak_ptr<EffectController> _effectController;
	std::weak_ptr<ModelRenderer> _modelRenderer;

	std::weak_ptr<Damageable> _damageable;
	std::weak_ptr<Targetable> _targetable;

#pragma region 各種フラグ
	bool _callInputBufferingEvent = false;
	bool _callCancelAttackEvent = false;
	bool _callCancelEvent = false;
	bool _callInvisivleEvent = false;
	bool _callChargingEvent = false;
	bool _oldInvisibleEvent = false;
	// 入力方向をワールド空間に変換したもの
	Vector2 _movement = { 0.0f, 0.0f };
	bool _isMoving = false;
	bool _isDash = false;
	bool _isEvade = false;
	bool _isInCombat = false;
	bool _isAttack = false;
	bool _isHoldingAttackKey = false;
	bool _isSpecialAttack = false;
	bool _isGuard = false;
	bool _isDead = false;
	bool _isUsingItem = false;
	bool _isSelect = false;

	// 溜め段階
	int _chargeLevel = 0;
#pragma endregion

	// ダッシュ時の回転補正値
	float _dashRotationFactor = 0.7f;

	// ダッシュ時消費スタミナ(s)
    float _dashStaminaConsume = 5.0f;
	// 回避時消費スタミナ(一回当たり)
    float _evadeStaminaConsume = 10.0f;
	// ガード時消費スタミナ(一回当たり)
    float _guardStaminaConsume = 10.0f;

	// 溜め演出時のリムライト表示時間
	float _chargeEffectRimLightTimer = 0.0f;
	float _chargeEffectRimLightTimeMax = 0.5f;
	// 溜め段階のリムライトカラー
	Vector4 _chargeEffectRimLightColor1 = Vector4::Yellow;
	Vector4 _chargeEffectRimLightColor2 = Vector4(1.0f, 50.0f / 255.0f, 0.0f, 1.0f);
	Vector4 _chargeEffectRimLightColor3 = Vector4::Red;
};