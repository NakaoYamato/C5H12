#include "ClientAssignment.h"

#include <imgui.h>

/// 開始処理
void ClientAssignment::Execute(const char* ipAddress)
{
	uint32_t timeout = 5000;
	connection = 0;

	// ENLの初期化
	if (!ENLInitialize())
	{
		_logs.push_back(u8"\tENLの初期化失敗");
	}
	else
	{
		_logs.push_back(u8"ENLの初期化成功");
	}
	// Connection
	connection = ENLClientConnect(ENL_CONNECTION_TYPE, ipAddress, ENL_PORT_ADDRESS, timeout);

	// コールバック関数はstatic関数なので
	// コールバック関数内から非静的メンバ変数にアクセスできない。
	// そのため、connectionにポインタを紐づけておく
	SetClientData(connection, this);
	// Setup CallBack
	// コネクト
	SetConnectCallback(connection, Connect);
}

/// 更新処理
void ClientAssignment::Update()
{
	ENLUpdate();
}

/// 終了処理
void ClientAssignment::Exit()
{
	// server終了処理
	if (connection != 0)
	{
		ENLClose(connection);
		connection = -1;
	}
	ENLFinalize();
}

#pragma region コールバック関数
/// サーバーと接続時に呼ばれる関数
void ClientAssignment::Connect(ENLConnection connection, void* connectionData)
{
	// connectionに紐づいているポインタからメンバ変数を参照する
	ClientAssignment* self = static_cast<ClientAssignment*>(connectionData);
	self->_logs.push_back(u8"サーバーと接続");

	// レコードが読み込み可能になった際に呼ばれるCallBack関数の設定
	SetReadCallback(connection, ReadRecord);
	// 切断されたときのCallBack関数
	SetDisconnectCallback(connection, Disconnect);
}

/// サーバーと切断されたときに呼ばれる関数
void ClientAssignment::Disconnect(ENLConnection connection, void* connectionData)
{
	// connectionに紐づいているポインタからメンバ変数を参照する
	ClientAssignment* self = static_cast<ClientAssignment*>(connectionData);
	self->_logs.push_back(u8"サーバーと切断");
}

/// サーバーからデータが送られたときに呼ばれる関数
void ClientAssignment::ReadRecord(ENLConnection connection, void* connectionData, uint16_t payloadType, const void* payload, uint32_t payloadLen)
{
	// connectionに紐づいているポインタからメンバ変数を参照する
	ClientAssignment* self = static_cast<ClientAssignment*>(connectionData);
	self->_logs.push_back(u8"サーバーからデータ受信");

	// 受信データ取得
    ENLBuffer buffer;
    buffer.Write(payload, payloadLen);

	// コネクションに紐づいているメンバ変数からデータ更新
	switch (static_cast<Network::DataTag>(payloadType))
	{
    case Network::DataTag::Message:
    {
		Network::MessageData message;
		if (!buffer.Read(&message, payloadLen))
		{
			self->_logs.push_back(u8"\t読み取り失敗:Message");
			return;
		}
		if (self->_playerMessageDataCallback)
			self->_playerMessageDataCallback(message);
    }
    break;
    case Network::DataTag::Login:
	{
        Network::PlayerLogin playerLogin;
		if (!buffer.Read(&playerLogin, payloadLen))
		{
			self->_logs.push_back(u8"\t読み取り失敗:Login");
			return;
		}
		if (self->_playerLoginCallback)
			self->_playerLoginCallback(playerLogin);
	}
	break;
	case Network::DataTag::Logout:
	{
		Network::PlayerLogout playerLogout;
		if (!buffer.Read(&playerLogout, payloadLen))
		{
			self->_logs.push_back(u8"\t読み取り失敗:Logout");
			return;
		}
		if (self->_playerLogoutCallback)
			self->_playerLogoutCallback(playerLogout);
	}
	break;
	case Network::DataTag::Sync:
	{
		Network::PlayerSync playerSync;
		if (!buffer.Read(&playerSync, payloadLen))
		{
			self->_logs.push_back(u8"\t読み取り失敗:Sync");
			return;
		}
		if (self->_playerSyncCallback)
			self->_playerSyncCallback(playerSync);
	}
    break;
	case Network::DataTag::Move:
	{
		Network::PlayerMove playerMove;
		if (!buffer.Read(&playerMove, payloadLen))
		{
			self->_logs.push_back(u8"\t読み取り失敗:Move");
			return;
		}
		if (self->_playerMoveCallback)
			self->_playerMoveCallback(playerMove);
	}
	break;
	}
}
#pragma endregion

// GUI表示
void ClientAssignment::DrawGui()
{
	ImGui::Text(u8"ログ");
	ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(250, 470), ImGuiWindowFlags_NoTitleBar);
	for (std::string message : _logs) {
		ImGui::Text(u8"%s", message.c_str());
	}
	ImGui::EndChild();
	ImGui::Spacing();
}

/// データをサーバーに送信する
void ClientAssignment::WriteRecord(Network::DataTag tag, const void* data, uint32_t length)
{
	ENLBuffer send;
	send.Write(data, length);
	ENLWriteRecord(
		connection,								// 送信先指定
		static_cast<uint16_t>(tag),					// データコマンド
		send.GetData(),								// 送信データ
		send.GetDataLen()							// 送信サイズ
	);
}
