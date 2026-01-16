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
	// 更新処理
	void Update(float elapsedTime) override;
	// Gui描画処理
	void DrawGui() override;
	// オブジェクトとの接触時の処理
	void OnContact(CollisionData& collisionData) override;

#pragma region アクセサ
	// 陣営取得
	Faction GetFaction() const { return _faction; }
	// 陣営設定
	void SetFaction(Faction faction) { _faction = faction; }
	// 半径取得
	float GetRadius() const { return _radius; }
	// 半径設定
	void SetRadius(float radius) { _radius = radius; }
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
	// セーフゾーンの中にいるか
	bool IsInSafetyZone() const { return _isInSafetyZone; }
#pragma endregion

private:
	// 陣営
	Faction _faction = Faction::Neutral;
	// エリア番号
	int _areaNumber = -1;
	// 半径
	float _radius = 1.0f;
	// ヘイト値
	float _hateValue = 0.0f;
	// ターゲット可能か
	bool _isTargetable = true;

	// セーフゾーンにいるかどうか
	bool _isInSafetyZone = false;
    // セーフティタイマー
    float _sefetyTimer = 0.0f;
};