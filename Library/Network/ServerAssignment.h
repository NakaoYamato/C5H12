#pragma once
#include <ENL.h>
#include <ENLBuffer.h>
#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <DirectXMath.h>

#if 0
class ServerAssignment
{
public:
	ServerAssignment() {}
	~ServerAssignment() {}
	void Execute();
	void Exit();

	static void ReadRecord(ENLConnection connection, void* connection_data, uint16_t payload_type, const void* payload, uint32_t payload_len);
	static void Disconnect(ENLConnection connection, void* connection_data);
	static void Accept(ENLServer server, void* server_data, ENLConnection connection);

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
};
#endif