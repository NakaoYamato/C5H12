#pragma once

#include "../../Library/Network/ClientAssignment.h"

#include <unordered_map>
#include <mutex>

#include "../../Library/Actor/Actor.h"
#include "../../Library/Scene/Scene.h"
#include "../../Library/Audio/VoiceRecorder/VoiceRecorder.h"
#include "../../Library/Component/StateController.h"

#include "../../Source/Player/PlayerActor.h"
#include "../../Source/Enemy/EnemyActor.h"
#include "../../Source/Enemy/EnemyController.h"

/// <summary>
/// 通信を管理するメディエーター
/// </summary>
class NetworkMediator : public Actor
{
public:
    enum class State
    {
        Awaiting,
        Connecting,
        NonServer,

		StateMax,
    };

	/// <summary>
	/// プレイヤー情報
	/// </summary>
	struct PlayerData
	{
		std::weak_ptr<PlayerActor>          actor;
        std::weak_ptr<PlayerStateMachine>	state;
        std::weak_ptr<Damageable>		    damageable;
	};

    /// <summary>
    /// エネミー情報
    /// </summary>
    struct EnemyData
    {
        // 管理しているプレイヤーのID
        int                             controllerID = -1;
        std::weak_ptr<EnemyActor>       actor;
        std::weak_ptr<EnemyController>  controller;
        std::weak_ptr<StateMachine>	    state;
        std::weak_ptr<Damageable>		damageable;
    };

public:
    ~NetworkMediator() override;

    // 生成時処理
    void OnCreate() override;
    // 更新前処理
    void OnPreUpdate(float elapsedTime) override;
    // 遅延更新処理
	void OnLateUpdate(float elapsedTime) override;
    // GUI描画
    void OnDrawGui() override;

    // サーバー開始
    void ExecuteServer(const std::string& ipAddress);

    // ログ取得
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
	std::weak_ptr<EnemyActor> CreateEnemy(
        int uniqueID,
		int controllerID,
        Network::EnemyType type,
        const Vector3& position,
        float angleY,
        float health);
    /// <summary>
	/// 自身のプレイヤーのダメージ送信
    /// </summary>
    void SendMyPlayerDamage();
    /// <summary>
	/// 敵のダメージ送信
    /// </summary>
    void SendEnemyDamage();
    /// <summary>
    /// リーダーの再設定
    /// </summary>
    void ResetLeader();
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
    // ネットワークの状態
	State _state = State::Awaiting;
	// 接続先のIPアドレス
    std::string _ipAddress = "127.0.0.1";
    // 接続開始フラグ
	bool _isConnecting = false;
    // サーバー通信
    ClientAssignment _client;

    // 各プレイヤー情報
    // key : ネットワークID
    // value : プレイヤーのポインタ
    std::unordered_map<int, PlayerData> _players;
	// 各敵キャラクター情報
	// key : ユニークID
	// value : 敵のポインタ
	std::unordered_map<int, EnemyData> _enemies;
    // ユーザーが操作するプレイヤー
    int myPlayerId = -1;
	// リーダーのユニークID
	int leaderPlayerId = -1;
	// 次のリーダーのユニークID
	int nextLeaderPlayerId = -1;
    // スレッドセーフ用
    std::mutex _mutex;

    // 音声録音用
    VoiceRecorder _voiceRecorder;

#pragma region サーバーからの受信データ
	// プレイヤーメッセージデータ
    std::vector<Network::MessageData>       _messageDatas;
    std::vector<Network::PlayerLogin>       _playerLogins;
    std::vector<Network::PlayerLogout>      _playerLogouts;
    std::vector<Network::PlayerSync>        _playerSyncs;
    std::vector<Network::PlayerMove>        _playerMoves;
    std::vector<Network::PlayerApplyDamage> _playerApplyDamages;

    std::vector<Network::EnemyCreate>       _enemyCreates;
	std::vector<Network::EnemySync>         _enemySyncs;
	std::vector<Network::EnemyMove>         _enemyMoves;
	std::vector<Network::EnemyApplayDamage> _enemyApplayDamages;
#pragma endregion

    std::string _message;
    std::vector<std::string> _logs;

    // 情報の定期送信間隔(0.1秒に1回)
	float _sendInterval = 0.1f;
	float _sendTimer = 0.0f;

    // 敵生成のフラグ
	bool _sendEnemyCreate = false;
};