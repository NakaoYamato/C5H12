#pragma once

#include "../../Library/Actor/Actor.h"
#include "ItemData.h"

// 効果処理基底クラス
class ItemFunctionBase
{
public:
	enum class State
	{
		Start,
		Execute,
		End,
	};

public:
	virtual ~ItemFunctionBase() {}
	// 名前取得
	virtual std::string GetName() const = 0;
	// 開始処理
	virtual void Start(ItemData* item, Actor* user)
	{
		if (!item || !user)
		{
			_state = State::End;
			return;
		}
		_state = State::Execute;
		_item = item;
		_user = user;
	}
	// 効果発動
	virtual void Execute(float elapsedTime) = 0;
	// 終了処理
	virtual void End()
	{
		_state = State::End;
		_item = nullptr;
		_user = nullptr;
	}
	// パラメータマップ取得
	virtual ItemData::ParameterMap GetParameterKeys() = 0;
	// 状態取得
	State GetState() const { return _state; }
protected:
	State _state = State::Start;
	ItemData* _item = nullptr;
	Actor* _user = nullptr;
};

#pragma region 回復薬
class HealingPotionFunc : public ItemFunctionBase
{
public:
	HealingPotionFunc() = default;
	~HealingPotionFunc() override = default;
	// 名前取得
	std::string GetName() const override { return "HealingPotionFunc"; }
	// 開始処理
	void Start(ItemData* item, Actor* user) override;
	// 効果発動
	void Execute(float elapsedTime) override;
	// パラメータマップ取得
	ItemData::ParameterMap GetParameterKeys() override;

private:
	float _timer = 0.0f;
	float _healAmount = 0.0f;
	float _requiredTime = 0.0f;
};
#pragma endregion

#pragma region 強化薬
class StrengthPotionFunc : public ItemFunctionBase
{
public:
	enum StrengthType : int
	{
		AttackPower = 0,
		DefensePower,
	};

public:
	StrengthPotionFunc() = default;
	~StrengthPotionFunc() override = default;
	// 名前取得
	std::string GetName() const override { return "StrengthPotionFunc"; }
	// 開始処理
	void Start(ItemData* item, Actor* user) override;
	// 効果発動
	void Execute(float elapsedTime) override;
	// パラメータマップ取得
	ItemData::ParameterMap GetParameterKeys() override;
private:
	StrengthType _strengthType = StrengthType::AttackPower;
	float _timer = 0.0f;
	float _strengthAmount = 0.0f;
	float _requiredTime = 0.0f;
	float _effectTime = 0.0f;
};
#pragma endregion

#pragma region 秘薬
class ElixirPotionFunc : public ItemFunctionBase
{
public:
	enum Type : int
	{
		HPOnly = 0,
		All,
	};

public:
	ElixirPotionFunc() = default;
	~ElixirPotionFunc() override = default;
	// 名前取得
	std::string GetName() const override { return "ElixirPotionFunc"; }
	// 開始処理
	void Start(ItemData* item, Actor* user) override;
	// 効果発動
	void Execute(float elapsedTime) override;
	// パラメータマップ取得
	ItemData::ParameterMap GetParameterKeys() override;
private:
	Type _type = Type::HPOnly;
	float _timer = 0.0f;
	float _requiredTime = 0.0f;
};
#pragma endregion
