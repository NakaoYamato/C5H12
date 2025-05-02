#pragma once

#include <ENL.h>
#include <ENLBuffer.h>
#include <DirectXMath.h>

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
		Move,
		Attack,
		Login,
		Logout,
		Sync
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
	struct PlayerTransform
	{
		int id;
		DirectX::XMFLOAT3 position = {};
		DirectX::XMFLOAT3 angle = {};
		DirectX::XMFLOAT3 scale = {};
	};

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
		DirectX::XMFLOAT3 angle = {};
		DirectX::XMFLOAT3 scale = {};
	};
#pragma endregion
}