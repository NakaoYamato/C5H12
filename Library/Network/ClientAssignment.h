#pragma once

#include <NetworkDefine.h>

#include <string>
#include <vector>
#include <functional>

/// <summary>
/// クライアント
/// </summary>
class ClientAssignment
{
public:
	ClientAssignment() {}
	~ClientAssignment() {}

	/// <summary>
	/// 開始処理
	/// </summary>
	void Execute(const char* ipAddress = "127.0.0.1");

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 終了処理
	/// </summary>
	void Exit();

	// GUI表示
	void DrawGui();

#pragma region サーバーからの各種データ受け取りを行ったときのコールバック関数設定
    /// <summary>
    /// MessageDataを受け取った時のコールバック関数を設定する
    /// </summary>
    /// <param name="callback"></param>
    void SetPlayerMessageDataCallback(std::function<void(const Network::MessageData&)> callback)
    {
        _playerMessageDataCallback = callback;
    }
	/// <summary>
	/// PlayerSyncを受け取ったときのコールバック関数を設定する
	/// </summary>
	/// <param name="callback"></param>
	void SetPlayerSyncCallback(std::function<void(const Network::PlayerSync&)> callback)
	{
		_playerSyncCallback = callback;
	}
    /// <summary>
    /// PlayerLoginを受け取ったときのコールバック関数を設定する
    /// </summary>
    /// <param name="callback"></param>
    void SetPlayerLoginCallback(std::function<void(const Network::PlayerLogin&)> callback)
    {
        _playerLoginCallback = callback;
    }
	/// <summary>
	/// PlayerLogoutを受け取ったときのコールバック関数を設定する
	/// </summary>
	/// <param name="callback"></param>
    void SetPlayerLogoutCallback(std::function<void(const Network::PlayerLogout&)> callback)
    {
        _playerLogoutCallback = callback;
    }
	/// <summary>
	/// PlayerMoveを受け取ったときのコールバック関数を設定する
	/// </summary>
	/// <param name="callback"></param>
    void SetPlayerMoveCallback(std::function<void(const Network::PlayerMove&)> callback)
    {
        _playerMoveCallback = callback;
    }
#pragma endregion

private:
#pragma region サーバー通信のコールバック関数
	/// <summary>
	/// サーバーと接続時に呼ばれる関数
	/// </summary>
	/// <param name="connection"></param>
	/// <param name="connectionData">自身(ClientAssignment)のポインタ</param>
	static void Connect(ENLConnection connection, void* connectionData);

	/// <summary>
	/// サーバーと切断されたときに呼ばれる関数
	/// </summary>
	/// <param name="connection"></param>
	/// <param name="connectionData">自身(ClientAssignment)のポインタ</param>
	static void Disconnect(ENLConnection connection, void* connectionData);

	/// <summary>
	/// サーバーからデータが送られたときに呼ばれる関数
	/// </summary>
	/// <param name="connection"></param>
	/// <param name="connectionData">自身(ClientAssignment)のポインタ</param>
	/// <param name="payloadType">送られたデータタイプ</param>
	/// <param name="payload">データのポインタ</param>
	/// <param name="payloadLen">データの長さ</param>
	static void ReadRecord(ENLConnection connection, void* connectionData, uint16_t payloadType, const void* payload, uint32_t payloadLen);
#pragma endregion

private:
	ENLConnection connection = 0;

#pragma region サーバーからの各種データ受け取りを行ったときのコールバック関数
	std::function<void(const Network::MessageData&)> _playerMessageDataCallback;
	std::function<void(const Network::PlayerLogin&)> _playerLoginCallback;
	std::function<void(const Network::PlayerLogout&)> _playerLogoutCallback;
	std::function<void(const Network::PlayerSync&)> _playerSyncCallback;
	std::function<void(const Network::PlayerMove&)> _playerMoveCallback;
#pragma endregion

	// デバッグ用
	bool _drawGui = false;
	std::vector<std::string> _logs;
};