#pragma once

#include "../../Library/Network/ClientAssignment.h"

#include <unordered_map>
#include <mutex>

#include "../../Library/Actor/Actor.h"
#include "../../Library/Scene/Scene.h"
#include "../../Library/Audio/VoiceRecorder/VoiceRecorder.h"

#include "../../Source/Network/NetworkReceiver.h"

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
    /// キャラクターの情報
    /// </summary>
    struct CharacterData
    {
        int uniqueID = -1;
        std::weak_ptr<Actor> actor;
        std::weak_ptr<NetworkReceiver>		receiver;
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
    std::weak_ptr<Actor> CreatePlayer(int id, bool isControlled);
	/// <summary>
	/// 敵の生成
	/// </summary>
	/// <param name="uniqueId"></param>
	/// <param name="type"></param>
	/// <returns></returns>
	std::weak_ptr<Actor> CreateEnemy(
        int uniqueID,
		int controllerID,
        Network::CharacterType type,
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

    // 各キャラクター情報
    // key : ユニークID
    // value : ネットワーク受信クラスのポインタ
    std::unordered_map<int, CharacterData> _characters;

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
    std::vector<Network::CharacterSync>        _characterSyncs;
    std::vector<Network::CharacterMove>        _characterMoves;
    std::vector<Network::CharacterApplyDamage> _characterApplyDamages;

    std::vector<Network::EnemyCreate>       _enemyCreates;
#pragma endregion

    std::string _message;
    std::vector<std::string> _logs;

    // 情報の定期送信間隔(0.1秒に1回)
	float _sendInterval = 0.1f;
	float _sendTimer = 0.0f;

    // 敵生成のフラグ
	bool _sendEnemyCreate = false;
};
