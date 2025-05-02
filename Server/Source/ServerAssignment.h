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

#pragma region コールバック関数
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
	static void Disconnect(ENLConnection connection, void* connection_data);

	/// <summary>
	/// 接続されたときのコールバック関数
	/// </summary>
	/// <param name="server"></param>
	/// <param name="server_data">ServerAssignmentのポインタ</param>
	/// <param name="connection"></param>
	static void Accept(ENLServer server, void* server_data, ENLConnection connection);
#pragma endregion

	void AddID() { id++; }
	std::vector<Network::Client> GetClients() { return clients; }
	void EraseClient(ENLConnection connection);

private:
	int id = 0;
	ENLServer mrsServer = -1;
	std::vector<Network::Client> clients;
	bool loop = true;
};