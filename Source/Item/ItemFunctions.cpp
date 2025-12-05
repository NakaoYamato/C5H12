#include "ItemFunctions.h"

#include "../../Source/Common/Damageable.h"
#include "../../Source/Player/BuffController.h"

#pragma region 回復薬
// 開始処理
void HealingPotionFunc::Start(ItemData* item, Actor* user)
{
	ItemFunctionBase::Start(item, user);

	// パラメータ取得
	auto it = _item->parameters.find(u8"回復量");
	if (it != _item->parameters.end())
	{
		_healAmount = std::get<float>(it->second);
	}
	it = _item->parameters.find(u8"必要時間");
	if (it != _item->parameters.end())
	{
		_requiredTime = std::get<float>(it->second);
	}

	_timer = 0.0f;
}

void HealingPotionFunc::Execute(float elapsedTime)
{
	auto damageable = _user->GetComponent<Damageable>();
	if (!damageable)
		return;

	// 回復処理
	float amount = _healAmount * (elapsedTime / _requiredTime);
	damageable->Heal(amount);

	// 経過時間処理
	_timer += elapsedTime;
	if (_timer >= _requiredTime)
	{
		_state = State::End;
		return;
	}
}

ItemData::ParameterMap HealingPotionFunc::GetParameterKeys()
{
	ItemData::ParameterMap p;
	p[u8"回復量"] = 0.0f;
	p[u8"必要時間"] = 0.0f;
	return p;
}
#pragma endregion

#pragma region 強化薬
// 開始処理
void StrengthPotionFunc::Start(ItemData* item, Actor* user)
{
	ItemFunctionBase::Start(item, user);
	_timer = 0.0f;
	// パラメータ取得
	auto it = _item->parameters.find(u8"効果量");
	if (it != _item->parameters.end())
	{
		_strengthAmount = std::get<float>(it->second);
	}
	it = _item->parameters.find(u8"必要時間");
	if (it != _item->parameters.end())
	{
		_requiredTime = std::get<float>(it->second);
	}
	it = _item->parameters.find(u8"効果");
	if (it != _item->parameters.end())
	{
		_strengthType = static_cast<StrengthType>(std::get<int>(it->second));
	}
	it = _item->parameters.find(u8"効果時間");
	if (it != _item->parameters.end())
	{
		_effectTime = std::get<float>(it->second);
	}
}
// 効果発動
void StrengthPotionFunc::Execute(float elapsedTime)
{
	_timer += elapsedTime;
	if (_timer >= _requiredTime)
	{
		// 効果付与
		auto buffController = _user->GetComponent<BuffController>();
		if (buffController)
		{
			switch (_strengthType)
			{
			case StrengthType::AttackPower:
				buffController->AddBuff(BuffController::BuffType::Attack, _effectTime, _strengthAmount);
				break;
			case StrengthType::DefensePower:
				buffController->AddBuff(BuffController::BuffType::Defense, _effectTime, _strengthAmount);
				break;
			}
		}

		_state = State::End;
	}
}
// パラメータマップ取得
ItemData::ParameterMap StrengthPotionFunc::GetParameterKeys()
{
	ItemData::ParameterMap p;
	p[u8"効果"] = 0;
	p[u8"効果量"] = 0.0f;
	p[u8"必要時間"] = 0.0f;
	p[u8"効果時間"] = 0.0f;
	return p;
}
#pragma endregion

#pragma region 秘薬
void ElixirPotionFunc::Start(ItemData* item, Actor* user)
{
	ItemFunctionBase::Start(item, user);
	_timer = 0.0f;
	// パラメータ取得
	auto it = _item->parameters.find(u8"必要時間");
	if (it != _item->parameters.end())
	{
		_requiredTime = std::get<float>(it->second);
	}
	it = _item->parameters.find(u8"効果");
	if (it != _item->parameters.end())
	{
		_type = static_cast<Type>(std::get<int>(it->second));
	}
}

void ElixirPotionFunc::Execute(float elapsedTime)
{
	_timer += elapsedTime;
	if (_timer >= _requiredTime)
	{
		auto damageable = _user->GetComponent<Damageable>();
		if (!damageable)
			return;

		switch (_type)
		{
		case ElixirPotionFunc::HPOnly:
			damageable->ResetHealth(damageable->GetMaxHealth());
			break;
		case ElixirPotionFunc::All:
			damageable->ResetHealth(damageable->GetMaxHealth());
			// TODO : スタミナ回復
			break;
		}

		_state = State::End;
	}
}

ItemData::ParameterMap ElixirPotionFunc::GetParameterKeys()
{
	ItemData::ParameterMap p;
	p[u8"効果"] = 0;
	p[u8"必要時間"] = 0.0f;
	return p;
}
#pragma endregion
