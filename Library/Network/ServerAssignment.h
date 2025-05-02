#pragma once
#include <ENL.h>
#include <ENLBuffer.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <vector>
#include <DirectXMath.h>

#if 1
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
	/// 更新処理
	/// </summary>
	void Update();

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

	// GUI表示
	void DrawGui();

	enum class NetworkTag : uint16_t
	{
		Message = 1,
		Move,
		Attack,
		Login,
		Logout,
		Sync
	};

	struct Client {
		ENLConnection enlConnection = -1;
		//Player* player = nullptr;
	};

	// 送信データ用構造体
	struct PlayerInput
	{
		int id;
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 clickPosition;
	};

	struct PlayerLogin
	{
		int id;
	};

	struct PlayerLogout
	{
		int id;
	};
	struct PlayerSync
	{
		int id;
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 targetPosition;
		//Player::State state;
	};

	void AddID() { id++; }
	std::vector<Client> GetClients() { return clients; }
	void EraseClient(ENLConnection connection);

private:
	int id = 0;
	ENLServer mrsServer = -1;
	std::vector<Client> clients;
	bool loop = true;

	// 更新処理用スレッド
	std::shared_ptr<std::thread> _updateThread = nullptr;
	std::mutex updateThreadMutex;

	// デバッグ用
	bool _drawGui = false;
	std::vector<std::string> _logs;
};
#endif