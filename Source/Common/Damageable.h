#pragma once

#include "../../Library/Component/Component.h"

#include <functional>

class Damageable : public Component
{
public:
	Damageable() {}
	~Damageable() override = default;
	// 名前取得
	const char* GetName() const override { return "Damagable"; }
	// 開始処理
	void Start() override;
	// 更新処理
	void Update(float elapsedTime) override;

	// HP初期化
	void ResetHealth(float maxHealth);
	// ダメージを与える
	bool AddDamage(float damage, Vector3 hitPosition, bool networkData = false);

#pragma region アクセサ
	// ヘルス取得
	float GetHealth() const { return _health; }
	// 最大ヘルス取得
	float GetMaxHealth() const { return _maxHealth; }
	// 無敵時間か
	bool IsInvisible() const { return _invisibleTimer > 0.0f; }
	// 死亡しているか
	bool IsDead() const { return _health <= 0; }
	// 被弾位置取得
	const Vector3& GetHitPosition() const { return _hitPosition; }
	// 前フレームに受けたダメージ量
	float GetLastDamage() const { return _lastDamage; }

	// 無敵状態にする
	void SetInvisible(float time) { _invisibleTimer = time; }
	// HPを設定
	void SetHelth(float health) { _health = health; }
	// 最大HPを設定
	void SetMaxHealth(float maxHealth) { _maxHealth = maxHealth; }
	// 被弾位置を設定
	void SetHitPosition(const Vector3& position) { _hitPosition = position; }
	// ダメージを受ける判定のコールバック関数を設定
	void SetTakeableDamageCallback(std::function<bool(float, Vector3)> callback) { _takeableDamageCallback = callback; }
	// ダメージを受けたときのコールバック関数を設定
	void SetOnDamageCallback(std::function<void(float, Vector3)> callback) { _onDamageCallback = callback; }
#pragma endregion

private:
	// 無敵状態タイマー
	float _invisibleTimer = 0.0f; 
	// 現在のHP
	float _health = 100.0f;
	// 最大HP
	float _maxHealth = 100.0f;
	// 被弾位置
	Vector3 _hitPosition = Vector3::Zero;
	// 前フレームに受けたダメージ量
	float _lastDamage = 0.0f;

	// ダメージを受ける判定のコールバック関数
	std::function<bool(float, Vector3)> _takeableDamageCallback;
	// ダメージを受けたときのコールバック関数
	std::function<void(float, Vector3)> _onDamageCallback;
};