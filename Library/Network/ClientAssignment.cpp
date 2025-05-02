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
}
#pragma endregion

// GUI表示
void ClientAssignment::DrawGui()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu(u8"デバッグ"))
		{
			ImGui::Checkbox(u8"サーバー", &_drawGui);
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	if (_drawGui)
	{
		if (ImGui::Begin(u8"サーバー"))
		{
			ImGui::Text(u8"ログ");
			ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(250, 470), ImGuiWindowFlags_NoTitleBar);
			for (std::string message : _logs) {
				ImGui::Text(u8"%s", message.c_str());
			}
			ImGui::EndChild();
			ImGui::Spacing();
		}
		ImGui::End();
	}
}
