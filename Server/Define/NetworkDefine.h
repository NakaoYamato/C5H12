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
#include "EnemyDefine.h"

#define NETWORK_PORT_ADDRESS 7000
#define NETWORK_MAX_CONNECTION 4
#define NETWORK_MAX_MESSAGE_SIZE 512
#define NETWORK_MAX_STATE_NAME_SIZE 32

namespace Network
{
	/// <summary>
	/// データ識別用
	/// </summary>
	enum class DataTag : uint16_t
	{
		Message = 1,

		PlayerLogin,
		PlayerLogout,
		PlayerSetLeader,
		PlayerSync,
		PlayerMove,
		PlayerApplyDamage,

		EnemyCreate,
		EnemySync,
		EnemyMove,
		EnemyApplayDamage,
	};

	struct Player
	{
		int uniqueID = -1;
		bool isLeader = false;
		DirectX::XMFLOAT3 position = {};
        DirectX::XMFLOAT2 movement = {};
        float angleY = 0.0f;
		float health = 100.0f;
		PlayerMainStates state = PlayerMainStates::None;
        PlayerSubStates subState = PlayerSubStates::None;
	};

	struct Enemy
	{
		EnemyType type = EnemyType::None;
		int uniqueID = -1;
		int leaderID = -1; // 読み取り用
		DirectX::XMFLOAT3 position = {};
		float angleY = 0.0f;
		DirectX::XMFLOAT3 target = {};
		float health = 100.0f;
		char mainState[NETWORK_MAX_STATE_NAME_SIZE] = { "\0" };
		char subState[NETWORK_MAX_STATE_NAME_SIZE] = { "\0" };
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
		int playerUniqueID = 1;
		char message[NETWORK_MAX_MESSAGE_SIZE] = { "\0" };
    };

#pragma region プレイヤー情報
	struct PlayerLogin
	{
		int playerUniqueID = -1;
	};
	struct PlayerLogout
	{
		int playerUniqueID = -1;
	};
	struct PlayerSetLeader
	{
		int playerUniqueID = -1;
	};
	struct PlayerSync
	{
		int playerUniqueID = -1;
		DirectX::XMFLOAT3 position = {};
		float angleY = 0.0f;
	};
	struct PlayerMove
	{
		int playerUniqueID = -1;
		DirectX::XMFLOAT3 position = {};
		DirectX::XMFLOAT2 movement = {};
		float angleY = 0.0f;
		PlayerMainStates state = PlayerMainStates::None;
		PlayerSubStates subState = PlayerSubStates::None;
	};
	struct PlayerApplyDamage
	{
		int playerUniqueID = -1;
		float damage = 0.0f;
		DirectX::XMFLOAT3 hitPosition = {};
	};
#pragma endregion

#pragma region 敵情報
	struct EnemyCreate
	{
		EnemyType type = EnemyType::None;
		int uniqueID = -1;
		int leaderID = -1;
		DirectX::XMFLOAT3 position = {};
		float angleY = {};
		float health = 100.0f;
	};

	struct EnemySync
	{
		EnemyType type = EnemyType::None;
		int uniqueID = -1;
		int leaderID = -1;
		DirectX::XMFLOAT3 position = {};
		float angleY = {};
		float health = 100.0f;
	};

	struct EnemyMove
	{
		int uniqueID = -1;
		DirectX::XMFLOAT3 position = {};
		float angleY = {};
		DirectX::XMFLOAT3 target = {};
		char mainState[NETWORK_MAX_STATE_NAME_SIZE] = { "\0" };
		char subState[NETWORK_MAX_STATE_NAME_SIZE] = { "\0" };
	};

	struct EnemyApplayDamage
	{
		int playerUniqueID = -1;
		int uniqueID = -1;
		float damage = 0.0f;
		DirectX::XMFLOAT3 hitPosition = {};
	};
#pragma endregion


#pragma endregion
}