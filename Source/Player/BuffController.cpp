#include "BuffController.h"

#include <imgui.h>

// 開始処理
void BuffController::Start()
{
	_damageable = GetActor()->GetComponent<Damageable>();
	_damageSender = GetActor()->GetComponent<DamageSender>();
}

// 更新処理
void BuffController::Update(float elapsedTime)
{
	_totalAttackBuff = 1.0f;
	_totalDefenseBuff = 1.0f;
	for (auto it = _activeBuffs.begin(); it != _activeBuffs.end(); )
	{
		auto& buff = *it;

		buff->_duration -= elapsedTime;
		if (buff->_duration <= 0.0f)
		{
			// 期限切れなら削除
			it = _activeBuffs.erase(it);
		}
		else
		{
			// 生存している場合のみバフ値を加算
			switch (buff->_type)
			{
			case BuffType::Attack:
				_totalAttackBuff += buff->_value;
				break;
			case BuffType::Defense:
				_totalDefenseBuff += buff->_value;
				break;
			}

			// 次の要素へ進む
			++it;
		}
	}

	// バフ適用
	if (auto damageSender = _damageSender.lock())
	{
		damageSender->SetSharpnessFactor(_totalAttackBuff);
	}
	if (auto damageable = _damageable.lock())
	{
		damageable->SetDefenseFactor(_totalDefenseBuff);
	}
}

// Gui描画
void BuffController::DrawGui()
{
	ImGui::Text(u8"現在の攻撃バフ倍率: %.2f", _totalAttackBuff);
	ImGui::Text(u8"現在の防御バフ倍率: %.2f", _totalDefenseBuff);
}

// バフ追加
void BuffController::AddBuff(BuffType _type, float duration, float _value)
{
	_activeBuffs.push_back(std::make_unique<BuffData>(_type, duration, _value));
}
