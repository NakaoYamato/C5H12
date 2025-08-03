#pragma once

#define NETWORK_MAX_MESSAGE_SIZE 512
#define NETWORK_MAX_STATE_NAME_SIZE 32

namespace Network
{
	/// <summary>
	/// キャラクターの種類
	/// </summary>
	enum class CharacterType : int
	{
		None = -1,

		Player = 0,

		Wyvern,
		Weak,
	};

#pragma region 送信データ
	struct MessageData
	{
		int playerUniqueID = 1;
		char message[NETWORK_MAX_MESSAGE_SIZE] = { "\0" };
	};
	struct PlayerLogin
	{
		int playerUniqueID = -1;
	};
	struct PlayerLogout
	{
		int playerUniqueID = -1;
	};

#pragma region キャラクター情報
	// キャラクターの同期情報
	struct CharacterSync
	{
		int senderID = -1;
		int uniqueID = -1;
		CharacterType type = CharacterType::None; // キャラクターの種類
		float angleY = {};
		DirectX::XMFLOAT3 position = {};
		float health = 100.0f;
	};
	// キャラクターの移動情報
	struct CharacterMove
	{
		int senderID = -1;
		int uniqueID = -1;
		CharacterType type = CharacterType::None; // キャラクターの種類
		float angleY = {};
		DirectX::XMFLOAT3 position = {};
		float padding0 = 0.0f;
		DirectX::XMFLOAT3 target = {};
		float padding1 = 0.0f;
		char mainState[NETWORK_MAX_STATE_NAME_SIZE] = { "\0" };
		char subState[NETWORK_MAX_STATE_NAME_SIZE] = { "\0" };
	};
	// キャラクターのダメージ情報
	struct CharacterApplyDamage
	{
		int senderID = -1;
		int uniqueID = -1;
		float damage = 0.0f;
		DirectX::XMFLOAT3 hitPosition = {};
	};
#pragma endregion

#pragma region プレイヤー情報
	struct PlayerSetLeader
	{
		int playerUniqueID = -1;
	};
#pragma endregion

#pragma region 敵情報
	struct EnemyCreate
	{
		CharacterType type = CharacterType::None; // キャラクターの種類
		int uniqueID = -1;
		int leaderID = -1;
		DirectX::XMFLOAT3 position = {};
		float angleY = {};
		float health = 100.0f;
	};
#pragma endregion

#pragma endregion
}