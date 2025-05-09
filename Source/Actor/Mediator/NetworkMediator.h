#pragma once

#include "../../Library/Network/ClientAssignment.h"

#include <unordered_map>
#include <mutex>

#include "../../Library/Actor/Actor.h"
#include "../Player/PlayerActor.h"
#include "../../Library/Scene/Scene.h"

class NetworkMediator : public Actor
{
public:
    ~NetworkMediator() override;

    // 開始時処理
    void OnCreate() override;

    // 開始時処理
    void Start() override;

    // 更新処理
    void Update(float elapsedTime) override;

    // 固定間隔更新処理
    void FixedUpdate() override;

    // GUI描画
    void DrawGui() override;

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
    // サーバー通信
    std::shared_ptr<ClientAssignment> _client;
    // ユーザーが操作するプレイヤー
    int myPlayerId = -1;

    // スレッドセーフ用
    std::mutex _mutex;

#pragma region サーバーからの受信データ
	// プレイヤーメッセージデータ
    std::vector<Network::MessageData> _messageDatas;
    std::vector<Network::PlayerLogin> _playerLogins;
    std::vector<Network::PlayerLogout> _playerLogouts;
    std::vector<Network::PlayerSync> _playerSyncs;
    std::vector<Network::AllPlayerSync> _allPlayerSyncs;
    std::vector<Network::PlayerMove> _playerMoves;
#pragma endregion

    // データ同期間隔(秒)
    float _syncTime = 1.0f;
    float _syncTimer = 0.0f;

    std::string _message;
    std::vector<std::string> _logs;
};