#pragma once

#include "../Define/NetworkDefine.h"

#include <iostream>
#include <thread>
#include <string>
#include <vector>

/// <summary>
/// サーバー
/// </summary>
class ServerAssignment
{
public:
	ServerAssignment() {}
	~ServerAssignment() {}

	/// <summary>
	/// 開始処理
	/// </summary>
	void Execute();

	/// <summary>
	/// 終了処理
	/// </summary>
	void Exit();

	/// <summary>
	/// すべてのクライアント取得
	/// </summary>
	/// <returns></returns>
	std::vector<Network::Client> GetClients() { return clients; }

	/// <summary>
    /// idからクライアントを取得
	/// </summary>
	/// <param name="id"></param>
	/// <returns>失敗でnullptr</returns>
	Network::Client* GetClientFromID(int id)
	{
		for (Network::Client& client : clients)
		{
			if (client.player.id == id)
			{
				return &client;
			}
		}
		return nullptr;
	}

#ifdef USE_MRS
	/// <summary>
	/// connectionからクライアントを取得
	/// </summary>
	/// <param name="connection"></param>
	/// <returns>失敗でnullptr</returns>
	Network::Client* GetClientFromConnection(MrsConnection connection)
	{
		for (Network::Client& client : clients)
		{
			if (client.connection == connection)
			{
				return &client;
			}
		}
		return nullptr;
	}
#else
	/// <summary>
	/// connectionからクライアントを取得
	/// </summary>
	/// <param name="connection"></param>
	/// <returns>失敗でnullptr</returns>
	Network::Client* GetClientFromConnection(ENLConnection connection)
	{
		for (Network::Client& client : clients)
		{
			if (client.connection == connection)
			{
				return &client;
			}
		}
		return nullptr;
	}
#endif // USE_MRS

#ifdef USE_MRS
	/// <summary>
	/// クライアント削除
	/// </summary>
	/// <param name="connection"></param>
	/// <returns>削除したクライアントのID</returns>
	int EraseClient(MrsConnection connection);
#else
	/// <summary>
	/// クライアント削除
	/// </summary>
	/// <param name="connection"></param>
	/// <returns>削除したクライアントのID</returns>
	int EraseClient(ENLConnection connection);
#endif // USE_MRS

private:
#pragma region コールバック関数
#ifdef USE_MRS
	/// <summary>
	/// レコードが読み込み可能になった際に呼ばれるコールバック関数
	/// </summary>
	/// <param name="connection"></param>
	/// <param name="connection_data">ServerAssignmentのポインタ</param>
	/// <param name="payload_type"></param>
	/// <param name="payload"></param>
	/// <param name="payload_len"></param>
	static void ReadRecord(MrsConnection connection, void* connectionData, uint32 seqnum, uint16 options, uint16 payloadType, const void* payload, uint32 payloadLen);

	/// <summary>
	/// ユーザが切断したときに呼ばれるコールバック関数
	/// </summary>
	/// <param name="connection"></param>
	/// <param name="connection_data">ServerAssignmentのポインタ</param>
	static void Disconnect(MrsConnection connection, void* connectionData);

	/// <summary>
	/// 接続されたときのコールバック関数
	/// </summary>
	/// <param name="server"></param>
	/// <param name="server_data">ServerAssignmentのポインタ</param>
	/// <param name="connection"></param>
	static void Accept(MrsServer server, void* serverData, MrsConnection connection);
#else
	/// <summary>
	/// レコードが読み込み可能になった際に呼ばれるコールバック関数
	/// </summary>
	/// <param name="connection"></param>
	/// <param name="connection_data">ServerAssignmentのポインタ</param>
	/// <param name="payload_type"></param>
	/// <param name="payload"></param>
	/// <param name="payload_len"></param>
	static void ReadRecord(ENLConnection connection, void* connectionData, uint16_t payloadType, const void* payload, uint32_t payloadLen);

	/// <summary>
	/// ユーザが切断したときに呼ばれるコールバック関数
	/// </summary>
	/// <param name="connection"></param>
	/// <param name="connection_data">ServerAssignmentのポインタ</param>
	static void Disconnect(ENLConnection connection, void* connectionData);

	/// <summary>
	/// 接続されたときのコールバック関数
	/// </summary>
	/// <param name="server"></param>
	/// <param name="server_data">ServerAssignmentのポインタ</param>
	/// <param name="connection"></param>
	static void Accept(ENLServer server, void* serverData, ENLConnection connection);
#endif // USE_MRS
#pragma endregion

private:
	// プレイヤーの次のユニークID
	int playerNextUniqueID = 0;
	// 敵の次のユニークID
	int enemyNextUniqueID = 0;

#ifdef USE_MRS
	MrsServer mrsServer = nullptr;
#else
	ENLServer mrsServer = -1;
#endif // USE_MRS

	std::vector<Network::Client> clients;
	bool loop = true;
};