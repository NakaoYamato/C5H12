#pragma once

#include "../../Library/Component/Component.h"

class Targetable : public Component , public std::enable_shared_from_this<Targetable>
{
public:
	// 陣営
	enum class Faction
	{
		Player,
		Enemy,
		Neutral,
	};

public:
	Targetable() {}
	~Targetable() override = default;
	// 名前取得
	const char* GetName() const override { return "Targetable"; }

	// 開始処理
	void Start() override;
	// Gui描画処理
	void DrawGui() override;

#pragma region アクセサ
	// 陣営取得
	Faction GetFaction() const { return _faction; }
	// 陣営設定
	void SetFaction(Faction faction) { _faction = faction; }
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
	// 陣営
	Faction _faction = Faction::Neutral;
	// ヘイト値
	float _hateValue = 0.0f;
	// ターゲット可能か
	bool _isTargetable = true;
};