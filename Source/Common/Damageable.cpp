#include "Damageable.h"

#include <imgui.h>

// 開始処理
void Damageable::Start()
{
	// HP初期化
	ResetHealth(_maxHealth);
}
// 更新処理
void Damageable::LateUpdate(float elapsedTime)
{
	if (_invisibleTimer > 0.0f)
		_invisibleTimer -= elapsedTime;
	// 前フレームに受けたダメージ量をリセット
	_lastDamage = 0.0f;
	// 前フレームの体力を保存
	_prevHealth = _health;
}
// Gui描画
void Damageable::DrawGui()
{
	ImGui::Text(u8"体力 : %f / %f", _health, _maxHealth);
	ImGui::Text(u8"無敵状態 : %s", _invisibleTimer > 0.0f ? u8"True" : u8"False");
	ImGui::Text(u8"前フレームに受けたダメージ量 : %f", _lastDamage);
	ImGui::Text(u8"総ダメージ量 : %f", _totalDamage);
	ImGui::Text(u8"防御力 : %f", _defense);
	ImGui::Text(u8"防御倍率 : %f", _defenseFactor);
	ImGui::Separator();

	if (ImGui::Button(u8"10ダメージを与える"))
	{
		AddDamage(10.0f, Vector3::Zero);
	}
	ImGui::SameLine();
	if (ImGui::Button(u8"HPの半分ダメージを与える"))
	{
		AddDamage(_health / 2.0f, Vector3::Zero);
	}
	if (ImGui::Button(u8"死亡"))
	{
		AddDamage(FLT_MAX, Vector3::Zero);
	}
	if (ImGui::Button(u8"体力全回復"))
	{
		Heal(_maxHealth);
	}
	ImGui::Separator();

	if (ImGui::TreeNode(u8"ダメージを受ける判定のコールバック関数"))
	{
		auto names = _takeableDamageCallback.GetCallBackNames();
		for (auto& name : names)
		{
			ImGui::Text("%s", name.c_str());
		}
		ImGui::TreePop();
	}
	ImGui::Separator();
	if (ImGui::TreeNode(u8"ダメージを受けたときのコールバック関数"))
	{
		auto names = _onDamageCallback.GetCallBackNames();
		for (auto& name : names)
		{
			ImGui::Text("%s", name.c_str());
		}
		ImGui::TreePop();
	}
	ImGui::Separator();
	if (ImGui::TreeNode(u8"死亡時のコールバック関数"))
	{
		auto names = _onDeathCallback.GetCallBackNames();
		for (auto& name : names)
		{
			ImGui::Text("%s", name.c_str());
		}
		ImGui::TreePop();
	}
	ImGui::Separator();
}
// HP初期化
void Damageable::ResetHealth(float maxHealth)
{
	_maxHealth = maxHealth;
	_health = _maxHealth;
	_prevHealth = _health;
}
// ダメージを与える
bool Damageable::AddDamage(float damage, Vector3 hitPosition, bool networkData)
{
	if (!networkData)
	{
		// 無敵状態ならダメージを受けない
		if (_invisibleTimer > 0.0f) return false;
		// 今フレームでダメージを受けている場合はダメージを受けない
		if (_lastDamage > 0.0f) return false;
		// ダメージを受ける判定のコールバック関数がfalseを返した場合はダメージを受けない
		auto names = _takeableDamageCallback.GetCallBackNames();
		for (auto& name : names)
		{
			if (!_takeableDamageCallback.Call(name, damage, hitPosition))
			{
				return false;
			}
		}
	}

	// ダメージ計算
	float def = 80.0f / (80.0f + _defense * _defenseFactor);
	damage = damage * def;

	// まだ生存していて、ダメージによって死亡する場合はonDeathをtrueにする
	bool onDeath = _health > 0.0f && (_health - damage) <= 0.0f;

	_health -= damage;
	_hitPosition = hitPosition;
	_totalDamage += damage;

	if (_health < 0.0f)
	{
		_health = 0.0f;
	}
	// ダメージを受けたときのコールバック関数呼び出し
	_onDamageCallback.Call(damage, hitPosition);
	// 受けたダメージ量を記録
	_lastDamage += damage;

	if (onDeath)
		// 死亡時のコールバック関数呼び出し
		_onDeathCallback.Call();
	return true;
}

// 回復する
void Damageable::Heal(float amount)
{
	_health += amount;
	if (_health > _maxHealth)
		_health = _maxHealth;
}
