#include "ClientAssignment.h"

/// 開始処理
void ClientAssignment::Execute()
{
	connection = 0;
	// ENLの初期化

}

/// 更新処理
void ClientAssignment::Update()
{
}

/// 終了処理
void ClientAssignment::Exit()
{
}

#pragma region コールバック関数
/// レコードが読み込み可能になった際に呼ばれるコールバック関数
void ClientAssignment::ReadRecord(ENLConnection connection, void* connectionData, uint16_t payloadType, const void* payload, uint32_t payloadLen)
{
}

/// ユーザが切断したときに呼ばれるコールバック関数
void ClientAssignment::Disconnect(ENLConnection connection, void* connection_data)
{
}

/// 接続されたときのコールバック関数
void ClientAssignment::Accept(ENLServer server, void* server_data, ENLConnection connection)
{
}
#pragma endregion

// GUI表示
void ClientAssignment::DrawGui()
{
}
