#pragma once

#include "../Define/NetworkDefine.h"

#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <unordered_map>

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

private:
	/// <summary>
	/// データ送信
	/// </summary>
	/// <param name="connection"></param>
	/// <param name="tag"></param>
	/// <param name="data"></param>
	/// <param name="dataLength"></param>
#ifdef USE_MRS
	void WriteRecord(MrsConnection connection, Network::DataTag tag, const void* data, uint32_t dataLength);
#else
	void WriteRecord(ENLConnection connection, Network::DataTag tag, const void* data, uint32_t dataLength);
#endif
	/// <summary>
	/// リーダーの選定
	/// </summary>
	void SelectingLeader();
	/// <summary>
	/// すべてのキャラクター取得
	/// </summary>
	/// <returns></returns>
	std::unordered_map<int, Network::Character> GetCharacters() { return charactersMap; }
	/// <summary>
    /// uniqueIDからキャラクター取得
	/// </summary>
	/// <param name="id"></param>
	/// <returns>失敗でnullptr</returns>
	Network::Character* GetCharacterFromID(int uniqueID);

    /// <summary>
    /// キャラクター作成
    /// </summary>
    /// <param name="leaderID"></param>
    /// <param name="type"></param>
    /// <param name="position"></param>
    /// <param name="angleY"></param>
    /// <param name="health"></param>
	Network::Character* CreateCharacter(
		int leaderID,
		Network::CharacterType type, 
		const DirectX::XMFLOAT3& position, 
		float angleY, 
		float health);

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
    // キャラクターの次のユニークID
    int characterNextUniqueID = 0;

	// リーダーのユニークID
	int playerLeaderID = -1;

#ifdef USE_MRS
	MrsServer mrsServer = nullptr;
#else
	ENLServer mrsServer = -1;
#endif // USE_MRS

	// クライアントのリスト
	std::vector<Network::Client> clients;
	// キャラクターのリスト
	std::unordered_map<int, Network::Character> charactersMap;

	bool loop = true;
};