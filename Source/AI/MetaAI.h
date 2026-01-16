#pragma once

#include "../../Library/Component/Component.h"
#include "../../Source/Common/Targetable.h"
#include "../../Source/Stage/EntryZone.h"

#include <vector>
#include <memory>
#include <functional>

class MetaAI : public Component
{
public:
	MetaAI() {}
	~MetaAI() override = default;
	// 名前取得
	const char* GetName() const override { return "MetaAI"; }
	// 開始処理
	void Start() override;
	// 更新処理
	void Update(float elapsedTime) override;
	// GUI描画
	void DrawGui() override;

#pragma region ターゲット
	/// <summary>
	/// ターゲット登録
	/// </summary>
	/// <param name="targetable"></param>
	void RegisterTargetable(std::weak_ptr<Targetable> targetable);

	/// <summary>
	/// ターゲット削除
	/// </summary>
	/// <param name="targetable"></param>
	void RemoveTargetable(std::weak_ptr<Targetable> targetable);

	/// <summary>
	/// ターゲット検索
	/// </summary>
	/// <param name="faction">検索する対象の陣営</param>
	/// <param name="searchPosition">検索する場所</param>
	/// <param name="searchRadius">検索する範囲</param>
	/// <param name="searchFanction">検索関数　引数は検索対象</param>
	/// <returns></returns>
	Targetable* SearchTarget(Targetable::Faction faction,
		const Vector3& searchPosition,
		float searchRadius,
		const std::function<bool(Targetable*)>& searchFanction = nullptr) const;

	/// <summary>
	/// 特定の位置からランダムな位置を取得
	/// </summary>
	/// <param name="center"></param>
	/// <param name="range"></param>
	/// <returns></returns>
	Vector3 GetRandomPositionInRange(const Vector3& center, float range) const;
#pragma endregion

#pragma region 開始地点
	/// <summary>
	/// 開始地点登録
	/// </summary>
	/// <param name="entryZone"></param>
	void RegisterEntryZone(std::weak_ptr<EntryZone> entryZone);

	/// <summary>
	/// 開始地点削除
	/// </summary>
	/// <param name="entryZone"></param>
	void RemoveEntryZone(std::weak_ptr<EntryZone> entryZone);

	/// <summary>
	/// 指定の位置から最も近い開始地点を検索
	/// </summary>
	/// <param name="faction"></param>
	/// <param name="position"></param>
	/// <returns></returns>
	EntryZone* SearchNearestEntryZone(Targetable::Faction faction, const Vector3& position) const;
#pragma endregion


private:
	// ターゲット可能なオブジェクトのリスト
	std::vector<std::weak_ptr<Targetable>> _targetables;

	// 開始地点
	std::vector<std::weak_ptr<EntryZone>> _entryZones;

	// ゲームクリアフラグ
	bool _gameClear = false;
	bool _gameOver = false;
	float _inputTimer = 0.0f;
	float _inputHoldTime = 2.0f;

	Vector2 _textPosition = Vector2(100.0f, 100.0f);
};