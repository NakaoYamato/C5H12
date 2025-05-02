#pragma once

#include <ENL.h>
#include <ENLBuffer.h>
#include <DirectXMath.h>

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
}