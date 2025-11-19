#pragma once

#include "../../Library/Actor/Actor.h"
#include "ItemData.h"

// 効果処理基底クラス
class ItemFunctionBase
{
public:
	virtual ~ItemFunctionBase() {}
	// 名前取得
	virtual std::string GetName() const = 0;
	// 効果発動
	virtual void Execute(ItemData* item, Actor* user) = 0;
	// パラメータマップ取得
	virtual ItemData::ParameterMap GetParameterKeys() = 0;
};


class HealingPotionFunc : public ItemFunctionBase
{
public:
	HealingPotionFunc() = default;
	~HealingPotionFunc() override = default;
	// 名前取得
	std::string GetName() const override { return "HealingPotionFunc"; }
	// 効果発動
	void Execute(ItemData* item, Actor* user) override;
	// パラメータマップ取得
	ItemData::ParameterMap GetParameterKeys() override;
};