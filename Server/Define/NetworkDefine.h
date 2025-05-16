#pragma once

#define USE_MRS

#ifdef USE_MRS
#include <mrs.hpp>

#define NETWORK_CONNECTION_TYPE MRS_CONNECTION_TYPE_UDP
#define NETWORK_RECORD_OPTION MrsRecordOption::MRS_RECORD_OPTION_NONE

#else
#include <ENL.h>
#include <ENLBuffer.h>

#define NETWORK_CONNECTION_TYPE ENLConnectionType::CONNECTION_TYPE_UDP
#endif // USE_MRS

#include <DirectXMath.h>
#include <string>

#include "PlayerDefine.h"

#define NETWORK_PORT_ADDRESS 7000
#define NETWORK_MAX_CONNECTION 4
#define NETWORK_MAX_MESSAGE_SIZE 512

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
        AllSync,

		Move,
		Attack,
	};

	struct Player
	{
		int id = -1;
		DirectX::XMFLOAT3 position = {};
		DirectX::XMFLOAT3 angle = {};
		PlayerState state = PlayerState::None;
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
        bool isSync = false;
	};

#pragma region 送信データ
    struct MessageData
    {
		int id = 1;
		char message[NETWORK_MAX_MESSAGE_SIZE] = { "\0" };
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

	/// <summary>
	/// Server: 受け取るときはplayers[0]にデータが入っている
	///			送信するときはidは各プレイヤーのID
	///			存在しないところはID == -1
    /// Client:	受け取るときはidは各プレイヤーのID
	///			存在しないところはID == -1
    ///			送信するときはplayers[0]にデータを入れる
	/// </summary>
	struct AllPlayerSync
	{
		Player players[NETWORK_MAX_CONNECTION] = {};
	};

	struct PlayerMove
	{
		int id = -1;
		DirectX::XMFLOAT3 position = {};
		DirectX::XMFLOAT3 velocity = {};
		PlayerState state = PlayerState::None;
	};

#pragma endregion
}