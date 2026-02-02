#pragma once

#include "../../Library/Component/Component.h"
#include "../../Source/Common/Targetable.h"
#include "../../Source/Stage/EntryZone.h"
#include "../../Source/Stage/StageController.h"

#include <vector>
#include <memory>
#include <functional>

// 前方宣言
class PlayerController;

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

#pragma region ステージ
	/// <summary>
	/// ステージコントローラー登録
	/// </summary>
	/// <param name="stageController"></param>
	void RegisterStageController(std::weak_ptr<StageController> stageController);

	/// <summary>
	/// ステージコントローラー削除
	/// </summary>
	/// <param name="stageController"></param>
	void RemoveStageController(std::weak_ptr<StageController> stageController);

	/// <summary>
	/// 指定の位置が含まれる開始地点を検索
	/// </summary>
	/// <param name="faction"></param>
	/// <param name="stageIndex"></param>
	/// <returns></returns>
	EntryZone* SearchEntryZoneFromStage(Targetable::Faction faction, int stageIndex) const;
#pragma endregion

#pragma region 倒した敵
	// 倒した敵の名前を登録
	void RegisterDefeatedEnemyName(const std::string& name)
	{
		_defeatedEnemyNames.push_back(name);
	}
	// 倒した敵の名前リスト取得
	const std::vector<std::string>& GetDefeatedEnemyNames() const
	{
		return _defeatedEnemyNames;
	}
	// 倒した敵の名前リストクリア
	void ClearDefeatedEnemyNames()
	{
		_defeatedEnemyNames.clear();
	}
#pragma endregion

#pragma region プレイヤー
    // プレイヤーコントローラー登録
	void RegisterPlayerController(std::weak_ptr<PlayerController> playerController);
    // プレイヤーコントローラー削除
    void RemovePlayerController(std::weak_ptr<PlayerController> playerController);
    // プレイヤーコントローラーリスト取得
	const std::vector<std::weak_ptr<PlayerController>>& GetPlayerControllers()
	{
		return _playerControllers;
    }
#pragma endregion


private:
	// ターゲット可能なオブジェクトのリスト
	std::vector<std::weak_ptr<Targetable>> _targetables;
	// 開始地点
	std::vector<std::weak_ptr<EntryZone>> _entryZones;
	// ステージコントローラーリスト
	std::vector<std::weak_ptr<StageController>> _stageControllers;
	// 倒した敵の名前リスト
	std::vector<std::string> _defeatedEnemyNames;
	// プレイヤー情報
    std::vector<std::weak_ptr<PlayerController>> _playerControllers;

	Vector2 _textPosition = Vector2(100.0f, 100.0f);
};