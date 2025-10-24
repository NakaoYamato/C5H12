#include "BodyPartController.h"

#include <imgui.h>

// 開始処理
void BodyPartController::Start()
{
	_damageableChild = GetActor()->GetComponent<DamageableChild>();
}

// 更新処理
void BodyPartController::Update(float elapsedTime)
{
	auto damageable = _damageableChild.lock();
	if (damageable == nullptr) return;

	// DamageableChildの総ダメージ量で処理
	float totalDamage = damageable->GetTotalDamage();
	// ダメージ量を取得
	float damage = totalDamage - _prevTotalDamage;
	// 部位耐久値更新
	_currentDurability -= damage;
	_staggerDamage += damage;

	// 怯み判定
	if (_staggerDamage >= _staggerInterval)
	{
		// 怯み処理
		_isStagger = true;
		_staggerCount++;
		_staggerDamage = 0.0f;

		if (_staggerToDownCount != -1 && _staggerCount % _staggerToDownCount == 0)
		{
			// ダウン処理
			_isDown = true;
		}
	}

	// 部位破壊判定
	if (!_isDestroyed && _currentDurability <= 0.0f)
	{
		OnDestroyBodyPart();
	}

	// 過去の総ダメージ量更新
	_prevTotalDamage = totalDamage;
}

// Gui描画
void BodyPartController::DrawGui()
{
	ImGui::Text(u8"部位名: %s", _bodyPartName.c_str());
	ImGui::DragFloat(u8"最大部位耐久値", &_maxDurability, 1.0f, 0.0f, 1000.0f, "%.1f");
	ImGui::DragFloat(u8"部位耐久値", &_currentDurability, 1.0f, 0.0f, _maxDurability, "%.1f");
	if (ImGui::Checkbox(u8"部位破壊済みか", &_isDestroyed))
		if (_isDestroyed)OnDestroyBodyPart();

	ImGui::DragFloat(u8"怯み間隔", &_staggerInterval, 1.0f, 0.0f, 1000.0f, "%.1f");
	ImGui::Text(u8"怯み回数: %d", _staggerCount);
	ImGui::DragFloat(u8"怯み判定用ダメージ量", &_staggerDamage, 1.0f, 0.0f, 1000.0f, "%.1f");
	ImGui::Checkbox(u8"怯み中か", &_isStagger);
	ImGui::DragInt(u8"ダウンまでの怯み回数", &_staggerToDownCount, 1, 1, 100);
	ImGui::Checkbox(u8"ダウン中か", &_isDown);
}

// 部位破壊時の処理
void BodyPartController::OnDestroyBodyPart()
{
	_isDestroyed = true;
	_currentDurability = 0.0f;
	// 部位破壊時コールバック呼び出し
	if (_onDestroyCallback)
	{
		_onDestroyCallback();
	}
}
