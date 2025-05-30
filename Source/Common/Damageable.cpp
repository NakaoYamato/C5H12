#include "Damageable.h"
// 開始処理
void Damageable::Start()
{
	// HP初期化
	ResetHealth(_maxHealth);
}
// 更新処理
void Damageable::Update(float elapsedTime)
{
	if (_invisibleTimer > 0.0f)
		_invisibleTimer -= elapsedTime;
}
// HP初期化
void Damageable::ResetHealth(float maxHealth)
{
	_maxHealth = maxHealth;
	_health = _maxHealth;
}
// ダメージを与える
bool Damageable::AddDamage(float damage, Vector3 hitPosition)
{
	// 無敵状態ならダメージを受けない
	if (_invisibleTimer > 0.0f) return false;
	// ダメージを受ける判定のコールバック関数がfalseを返した場合はダメージを受けない
	if (_takeableDamageCallback)
		if (_takeableDamageCallback(damage, hitPosition) == false) return false;
	_health -= damage;
	_hitPosition = hitPosition;
	if (_health < 0.0f)
		_health = 0.0f;
	// ダメージを受けたときのコールバック関数呼び出し
	if (_onDamageCallback)
		_onDamageCallback(damage, hitPosition);
	return true;
}
