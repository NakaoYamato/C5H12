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

#define NETWORK_PORT_ADDRESS 7000
#define NETWORK_MAX_CONNECTION 4

#include "NetworkStructDefine.h"

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

        CharacterSync,
        CharacterMove,
        CharacterApplyDamage,

		EnemyCreate,
	};

	struct Character
	{
		int leaderID = -1;
		int uniqueID = -1;
		CharacterType type = CharacterType::None; // キャラクターの種類
		float angleY = {};
		DirectX::XMFLOAT3 position = {};
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
        int characterID = -1; // キャラクターのユニークID
        bool isLeader = false; // リーダーかどうか
        bool isSync = false;
	};
}