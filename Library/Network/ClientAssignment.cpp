#include "ClientAssignment.h"

#include <imgui.h>

/// 開始処理
void ClientAssignment::Execute(const char* ipAddress)
{
	uint32_t timeout = 5000;
#ifdef USE_MRS
	// MRSライブラリ初期化
	if (!mrs_initialize())
	{
		_logs.push_back(u8"\tENLの初期化失敗");
	}
	_logs.push_back(u8"ENLの初期化成功");

	// Connection
	connection = mrs_connect(MRS_CONNECTION_TYPE_UDP, ipAddress, NETWORK_PORT_ADDRESS, timeout);

	// コールバック関数はstatic関数なので
	// コールバック関数内から非静的メンバ変数にアクセスできない。
	// そのため、connectionにポインタを紐づけておく
	mrs_connection_set_data(connection, this);

	// Setup CallBack
	// 接続に成功したときに実行される関数
	mrs_set_connect_callback(connection, Connect);
#else
	connection = 0;

	// ENLの初期化
	if (!ENLInitialize())
	{
		_logs.push_back(u8"\tENLの初期化失敗");
	}
	_logs.push_back(u8"ENLの初期化成功");
	// Connection
	connection = ENLClientConnect(NETWORK_CONNECTION_TYPE, ipAddress, NETWORK_PORT_ADDRESS, timeout);

	// コールバック関数はstatic関数なので
	// コールバック関数内から非静的メンバ変数にアクセスできない。
	// そのため、connectionにポインタを紐づけておく
	SetClientData(connection, this);
	// Setup CallBack
	// コネクト
	SetConnectCallback(connection, Connect);
#endif // USE_MRS
}

/// 更新処理
void ClientAssignment::Update()
{
#ifdef USE_MRS
	mrs_update();
#else
	ENLUpdate();
#endif // USE_MRS
}

/// 終了処理
void ClientAssignment::Exit()
{
#ifdef USE_MRS
	// server終了処理
	if (connection != nullptr)
	{
		// mrs_closeを実行したタイミングでは解放はされない。
		// 次にmrs_updateが実行されたタイミングで解放は行われる
		mrs_close(connection);
		connection = nullptr;
		mrs_update();
	}

	// ライブラリ解放
	mrs_finalize();
#else
	// server終了処理
	if (connection != 0)
	{
		ENLClose(connection);
		connection = -1;
	}
	ENLFinalize();
#endif // USE_MRS
}

#pragma region コールバック関数
/// サーバーと接続時に呼ばれる関数
#ifdef USE_MRS
void ClientAssignment::Connect(MrsConnection connection, void* connectionData)
#else
void ClientAssignment::Connect(ENLConnection connection, void* connectionData)
#endif // USE_MRS
{
	// connectionに紐づいているポインタからメンバ変数を参照する
	ClientAssignment* self = static_cast<ClientAssignment*>(connectionData);
	self->_logs.push_back(u8"サーバーと接続");

#ifdef USE_MRS
	// レコードが読み込み可能になった際に呼ばれるCallBack関数の設定
	mrs_set_read_record_callback(connection, ReadRecord);
	// 切断されたときのCallBack関数
	mrs_set_disconnect_callback(connection, Disconnect);
#else
	// レコードが読み込み可能になった際に呼ばれるCallBack関数の設定
	SetReadCallback(connection, ReadRecord);
	// 切断されたときのCallBack関数
	SetDisconnectCallback(connection, Disconnect);
#endif // USE_MRS
}

/// サーバーと切断されたときに呼ばれる関数
#ifdef USE_MRS
void ClientAssignment::Disconnect(MrsConnection connection, void* connectionData)
#else
void ClientAssignment::Disconnect(ENLConnection connection, void* connectionData)
#endif // USE_MRS
{
	// connectionに紐づいているポインタからメンバ変数を参照する
	ClientAssignment* self = static_cast<ClientAssignment*>(connectionData);
	self->_logs.push_back(u8"サーバーと切断");
}

/// サーバーからデータが送られたときに呼ばれる関数
#ifdef USE_MRS
void ClientAssignment::ReadRecord(MrsConnection connection, void* connectionData, uint32 seqnum, uint16 options, uint16 payloadType, const void* payload, uint32 payloadLen)
#else
void ClientAssignment::ReadRecord(ENLConnection connection, void* connectionData, uint16_t payloadType, const void* payload, uint32_t payloadLen)
#endif // USE_MRS
{
	// connectionに紐づいているポインタからメンバ変数を参照する
	ClientAssignment* self = static_cast<ClientAssignment*>(connectionData);
	self->_logs.push_back(u8"サーバーからデータ受信");

	// 受信データ取得
#ifdef USE_MRS
	mrs::Buffer buffer;
	buffer.Write(payload, payloadLen);
#else
    ENLBuffer buffer;
    buffer.Write(payload, payloadLen);
#endif // USE_MRS

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
    case Network::DataTag::PlayerLogin:
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
	case Network::DataTag::PlayerLogout:
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
	case Network::DataTag::PlayerSync:
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
	case Network::DataTag::PlayerMove:
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
#ifdef USE_MRS
	mrs::Buffer send;
    send.Write(data, length);
	mrs_write_record(
		connection,								// 送信先コネクション
		NETWORK_RECORD_OPTION,	// 通信オプション
		static_cast<uint64>(tag),					// データコマンド
		send.GetData(),								// 送信データ
		send.GetDataLen()							// 送信サイズ
	);
	send.Clear();
#else
	ENLBuffer send;
	send.Write(data, length);
	ENLWriteRecord(
		connection,								// 送信先指定
		static_cast<uint16_t>(tag),					// データコマンド
		send.GetData(),								// 送信データ
		send.GetDataLen()							// 送信サイズ
	);
#endif // USE_MRS
}
