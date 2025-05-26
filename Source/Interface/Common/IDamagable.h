#pragma once

#include "../../Library/Math/Vector.h"

class IDamagable
{
public:
	virtual ~IDamagable() = default;
	/// <summary>
	/// ダメージを与える
	/// </summary>
	/// <param name="damage">ダメージ量</param>
	/// <param name="hitPosition">ダメージを受けた場所</param>
	virtual void AddDamage(float damage, Vector3 hitPosition) = 0;
	/// <summary>
	/// ヘルス取得
	/// </summary>
	virtual float GetHealth() const { return _health; }
	/// <summary>
	/// 最大ヘルス取得
	/// </summary>
	virtual float GetMaxHealth() const { return _maxHealth; }
	/// <summary>
	/// 死亡しているか
	/// </summary>
	virtual bool IsDead() const { return _health > 0; }
	/// <summary>
	/// HP初期化
	/// </summary>
	virtual void ResetHealth() {
		_health = _maxHealth;
	}
	/// <summary>
	/// 被弾位置
	/// </summary>
	/// <returns></returns>
	virtual Vector3 GetHitPosition() const { return _hitPosition; }
protected:
	float _health = 100.0f;
	float _maxHealth = 100.0f;
	Vector3 _hitPosition = Vector3::Zero;
};