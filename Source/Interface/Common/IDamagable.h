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
	virtual void AddDamage(float damage, Vector3 hitPosition)
	{
		if (_invisible) return; // 無敵状態ならダメージを受けない
		_health -= damage;
		_hitPosition = hitPosition;
		if (_health < 0.0f)
			_health = 0.0f;
		OnDamage(damage, hitPosition);
	}
	/// <summary>
	/// ダメージを受けたときの処理
	/// </summary>
	/// <param name="damage"></param>
	/// <param name="hitPosition"></param>
	virtual void OnDamage(float damage, Vector3 hitPosition) {};
	/// <summary>
	/// ヘルス取得
	/// </summary>
	virtual float GetHealth() const { return _health; }
	/// <summary>
	/// 最大ヘルス取得
	/// </summary>
	virtual float GetMaxHealth() const { return _maxHealth; }
	/// <summary>
	/// 無敵時間か
	/// </summary>
	/// <returns></returns>
	virtual bool IsInvisible() const { return _invisible; }
	/// <summary>
	/// 死亡しているか
	/// </summary>
	virtual bool IsDead() const { return _health <= 0; }
	/// <summary>
	/// HP初期化
	/// </summary>
	virtual void ResetHealth(float maxHealth) {
		_maxHealth = maxHealth;
		_health = _maxHealth;
	}
	/// <summary>
	/// 被弾位置
	/// </summary>
	/// <returns></returns>
	virtual Vector3 GetHitPosition() const { return _hitPosition; }
protected:
	bool _invisible = false; // 無敵状態かどうか
	float _health = 100.0f;
	float _maxHealth = 100.0f;
	Vector3 _hitPosition = Vector3::Zero;
};