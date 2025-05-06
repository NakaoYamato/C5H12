#pragma once

#include <ENL.h>
#include <ENLBuffer.h>
#include <DirectXMath.h>
#include <string>

#define ENL_CONNECTION_TYPE ENLConnectionType::CONNECTION_TYPE_TCP
#define ENL_PORT_ADDRESS 7000

namespace Network
{
	/// <summary>
	/// データ識別用
	/// </summary>
	enum class DataTag : uint16_t
	{
		Message = 1,

		Login,
		Logout,
		Sync,

		Move,
		Attack,
	};

	struct Player
	{
		int id;
		DirectX::XMFLOAT3 position = {};
		DirectX::XMFLOAT3 angle = {};
		DirectX::XMFLOAT3 scale = {};
	};

	/// <summary>
	/// クライアント構造体
	/// </summary>
	struct Client 
	{
		ENLConnection enlConnection = -1;
		Player player = {};
	};

#pragma region 送信データ
    struct MessageData
    {
		int id;
        std::string message;
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
		int id = -1;
		DirectX::XMFLOAT3 position = {};
		DirectX::XMFLOAT3 angle = {};
	};

	struct PlayerMove
	{
		int id;
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 clickPosition;
	};

#pragma endregion
}