#pragma once

#include "../../Library/Component/Component.h"

class Targetable : public Component
{
public:
	Targetable() {}
	~Targetable() override = default;
	// 名前取得
	const char* GetName() const override { return "Targetable"; }

#pragma region アクセサ
	// ヘイト値取得
	float GetHateValue() const { return _hateValue; }
	// ヘイト値設定
	void SetHateValue(float hateValue) { _hateValue = hateValue; }
	// ヘイト値を加算
	void AddHateValue(float hateValue) { _hateValue += hateValue; }
	// ターゲット可能か取得
	bool IsTargetable() const { return _isTargetable; }
	// ターゲット可能か設定
	void SetTargetable(bool isTargetable) { _isTargetable = isTargetable; }
#pragma endregion

private:
	// ヘイト値
	float _hateValue = 0.0f;
	// ターゲット可能か
	bool _isTargetable = true;
};