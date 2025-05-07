#pragma once

#define USE_MRS

#ifdef USE_MRS
#include <mrs.hpp>

#define NETWORK_CONNECTION_TYPE MRS_CONNECTION_TYPE_UDP
#define NETWORK_RECORD_OPTION MrsRecordOption::MRS_RECORD_OPTION_NONE

#else
#include <ENL.h>
#include <ENLBuffer.h>

#define NETWORK_CONNECTION_TYPE ENLConnectionType::CONNECTION_TYPE_TCP
#endif // USE_MRS

#include <DirectXMath.h>
#include <string>

#define NETWORK_PORT_ADDRESS 7000

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
		int id = -1;
		DirectX::XMFLOAT3 position = {};
		DirectX::XMFLOAT3 angle = {};
		DirectX::XMFLOAT3 scale = {};
	};

	/// <summary>
	/// クライアント構造体
	/// </summary>
	struct Client 
	{
#ifdef USE_MRS
		MrsConnection connection = nullptr;
#else
		ENLConnection connection = -1;
#endif // USE_MRS
		Player player = {};
	};

#pragma region 送信データ
    struct MessageData
    {
		int id = 1;
		std::string message = {};
    };

	struct PlayerLogin
	{
		int id = -1;
	};

	struct PlayerLogout
	{
		int id = -1;
	};

	struct PlayerSync
	{
		int id = -1;
		DirectX::XMFLOAT3 position = {};
		DirectX::XMFLOAT3 angle = {};
	};

	struct PlayerMove
	{
		int id = -1;
		DirectX::XMFLOAT3 velocity = {};
	};

#pragma endregion
}