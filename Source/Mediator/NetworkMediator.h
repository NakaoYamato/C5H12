#pragma once

#include "../../Library/Network/ClientAssignment.h"

#include <unordered_map>
#include <mutex>

#include "../../Library/Actor/Actor.h"
#include "../../Library/Scene/Scene.h"
#include "../../Source/Player/PlayerActor.h"
#include "../../Source/Enemy/EnemyController.h"

class NetworkMediator : public Actor
{
public:
    struct EnemyData
    {
        // 管理しているプレイヤーのID
        int controllerID = -1;
        std::weak_ptr<EnemyController> enemyController;
    };

public:
    ~NetworkMediator() override;

    // 開始時処理
    void OnCreate() override;
    // 開始時処理
    void OnStart() override;
    /// 更新前処理
    void OnPreUpdate(float elapsedTime) override;
    // 遅延更新処理
	void OnLateUpdate(float elapsedTime) override;
    // 固定間隔更新処理
    void OnFixedUpdate() override;
    // GUI描画
    void OnDrawGui() override;

    const std::vector<std::string>& GetLogs()
    {
        // スレッドセーフ
        std::lock_guard<std::mutex> lock(_mutex);
        return _logs;
    }
private:
    /// <summary>
    /// プレイヤー作成
    /// </summary>
    /// <param name="id"></param>
    /// <param name="isControlled"></param>
    std::weak_ptr<PlayerActor> CreatePlayer(int id, bool isControlled);

	/// <summary>
	/// 敵の生成
	/// </summary>
	/// <param name="uniqueId"></param>
	/// <param name="type"></param>
	/// <returns></returns>
	std::weak_ptr<EnemyController> CreateEnemy(
        int uniqueID,
		int controllerID,
        Network::EnemyType type,
        const Vector3& position,
        float angleY,
        float health);

    /// <summary>
    /// サーバーからの各種データ受け取りを行ったときのコールバック関数設定
    /// </summary>
    void SetClientCollback();

    /// <summary>
    /// 受け取ったデータを処理
    /// </summary>
    void ProcessNetworkData();

#pragma region デバッグ用
    /// <summary>
    /// ネットワークGUIの表示
    /// </summary>
    void DrawNetworkGui();

    /// <summary>
    /// ログの表示
    /// </summary>
    void DrawLogGui();

    /// <summary>
	/// メッセージの表示
    /// </summary>
    void DrawMessageGui();
#pragma endregion


private:
    // 各プレイヤー情報
    // key : ネットワークID
    // value : プレイヤーのポインタ
    std::unordered_map<int, std::weak_ptr<PlayerActor>> _players;
	// 各敵キャラクター情報
	// key : ユニークID
	// value : 敵のポインタ
	std::unordered_map<int, EnemyData> _enemies;
    // サーバー通信
    std::shared_ptr<ClientAssignment> _client;
    // ユーザーが操作するプレイヤー
    int myPlayerId = -1;
	// リーダーのユニークID
	int leaderPlayerId = -1;
    // スレッドセーフ用
    std::mutex _mutex;

#pragma region サーバーからの受信データ
	// プレイヤーメッセージデータ
    std::vector<Network::MessageData> _messageDatas;
    std::vector<Network::PlayerLogin> _playerLogins;
    std::vector<Network::PlayerLogout> _playerLogouts;
    std::vector<Network::PlayerSync> _playerSyncs;
    std::vector<Network::PlayerMove> _playerMoves;

    std::vector<Network::EnemyCreate> _enemyCreates;
	std::vector<Network::EnemySync> _enemySyncs;
	std::vector<Network::EnemyMove> _enemyMoves;
#pragma endregion

    // データ同期間隔(秒)
    float _syncTime = 1.0f;
    float _syncTimer = 0.0f;

    std::string _message;
    std::vector<std::string> _logs;

	bool _sendEnemyCreate = false; // 敵生成のフラグ
};