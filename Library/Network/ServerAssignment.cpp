#include "ServerAssignment.h"

#include <imgui.h>

void ServerAssignment::Execute()
{
	// サーバ情報設定
	// IPアドレス、ポート番号、バックログ数を指定
	char address[40] = { "0.0.0.0" };
	uint16_t port = 7000;
	uint32_t backlog = 10;

	// ENLライブラリの初期化
	if (!ENLInitialize())
	{
		_logs.push_back(u8"\tサーバー初期化失敗");
		return;
	}
	_logs.push_back(u8"サーバー初期化成功");

	// サーバオブジェクト作成(accept実行)
	mrsServer = ENLServerCreate(ENLConnectionType::CONNECTION_TYPE_TCP, address, port, backlog);
	if (mrsServer <= 0)
	{
		_logs.push_back(u8"\tサーバーオブジェクト生成失敗");
		return;
	}
	_logs.push_back(u8"サーバーオブジェクト生成成功");
	// static関数からメンバ変数にアクセスできるようにthisポインタを紐づける
	SetServerData(mrsServer, this);
	// 接続されたときのコールバック関数設定
	SetAcceptCallback(mrsServer, Accept);

	// サーバ側からコマンド入力で終了されるまでループする。
	// キーボードでexitを入力するとループを抜けるための別スレッドを用意
	std::thread th(&ServerAssignment::Exit, this);

	do {
		// スタックされているデータの送信、受信を行う。
		ENLUpdate();
	} while (loop);
	th.join();

	// クライアント全削除
	for (Client client : clients)
	{
		ENLClose(client.enlConnection);
		client.enlConnection = 0;
		//delete client.player;
		//client.player = nullptr;
	}
	clients.clear();

	ENLClose(mrsServer);
	_logs.push_back(u8"サーバー封鎖");

	// ENL終了処理
	ENLFinalize();
	_logs.push_back(u8"サーバー終了");
}

void ServerAssignment::Exit()
{
	while (loop) {
		std::string input;
		std::cin >> input;
		if (input == "exit")
		{
			loop = false;
		}
	}
}

void ServerAssignment::ReadRecord(ENLConnection connection, void* connection_data, uint16_t payload_type, const void* payload, uint32_t payload_len)
{
	//ServerAssignment* self = reinterpret_cast<ServerAssignment*>(connection_data);
	//ENLBuffer buffer;
	//// 受信データをバッファに書き込み
	//buffer.Write(payload, payload_len);
	//switch (static_cast<NetworkTag>(payload_type))
	//{
	//case NetworkTag::Message:
	//{
	//	char payloadStr[256];
	//	// バッファデータからpayLoadStrにデータに読み込み
	//	if (!buffer.Read(payloadStr, payload_len)) {
	//		std::cout << "Read Error" << std::endl;
	//	}
	//	std::cout << "recv data message" << std::endl;
	//	std::cout << payloadStr << std::endl;
	//	for (const Client& client : self->clients)
	//	{
	//		// 受信データをクライアントに送信
	//		ENLWriteRecord(client.enlConnection, payload_type, payload, payload_len);
	//	}

	//	break;
	//}
	//case NetworkTag::Move:
	//{
	//	PlayerInput payloadData;
	//	// バッファデータからpayLoadStrにデータに読み込み
	//	if (!buffer.Read(&payloadData, payload_len)) {
	//		std::cout << "Read Error" << std::endl;
	//	}

	//	std::cout << "recv data move id:" << std::to_string(payloadData.id) << std::endl;
	//	std::cout << "position x:" << payloadData.position.x;
	//	std::cout << "y:" << payloadData.position.y;
	//	std::cout << "z:" << payloadData.position.z << std::endl;
	//	std::cout << "click_position x:" << payloadData.clickPosition.x;
	//	std::cout << "y:" << payloadData.clickPosition.y;
	//	std::cout << "z:" << payloadData.clickPosition.z << std::endl;

	//	for (const Client& client : self->clients)
	//	{
	//		// 同じIDの場合
	//		if (client.player->id == payloadData.id)
	//		{
	//			client.player->position = payloadData.position;
	//			client.player->targetPosition = payloadData.clickPosition;
	//			client.player->state = Player::State::Move;
	//		}
	//		// 受信データをクライアントに送信
	//		ENLWriteRecord(client.enlConnection, payload_type, payload, payload_len);
	//	}
	//	break;
	//}
	//case NetworkTag::Attack:
	//{
	//	PlayerInput payloadData;
	//	// バッファデータからpayLoadStrにデータに読み込み
	//	if (!buffer.Read(&payloadData, payload_len)) {
	//		std::cout << "Read Error" << std::endl;
	//	}

	//	std::cout << "recv data attack id:" << std::to_string(payloadData.id) << std::endl;
	//	std::cout << "position x:" << payloadData.position.x;
	//	std::cout << "y:" << payloadData.position.y;
	//	std::cout << "z:" << payloadData.position.z << std::endl;
	//	std::cout << "click_position x:" << payloadData.clickPosition.x;
	//	std::cout << "y:" << payloadData.clickPosition.y;
	//	std::cout << "z:" << payloadData.clickPosition.z << std::endl;

	//	for (const Client& client : self->clients)
	//	{
	//		// 同じIDの場合
	//		if (client.player->id == payloadData.id)
	//		{
	//			client.player->position = payloadData.position;
	//			client.player->targetPosition = payloadData.clickPosition;
	//			client.player->state = Player::State::Move;
	//		}
	//		// 受信データをクライアントに送信
	//		ENLWriteRecord(client.enlConnection, payload_type, payload, payload_len);
	//	}
	//	break;
	//}
	//case NetworkTag::Logout:
	//{
	//	int id = -1;
	//	// クライアントに削除送信
	//	for (const Client& client : self->clients)
	//	{
	//		if (client.enlConnection == connection)
	//		{
	//			// ログアウトした人のIDを保存
	//			id = client.player->id;
	//			// 送信者には送らない
	//			continue;
	//		}
	//		// 受信データをクライアントに送信
	//		ENLWriteRecord(client.enlConnection, payload_type, payload, payload_len);
	//	}

	//	// サーバからもクライアント情報を削除
	//	self->EraseClient(connection);

	//	std::cout << "logouted clients id:" << std::to_string(id) << std::endl;
	//	break;
	//}
	//}
}

// ユーザが切断したときに呼ばれるコールバック関数
void ServerAssignment::Disconnect(ENLConnection connection, void* connection_data)
{
	ServerAssignment* self = reinterpret_cast<ServerAssignment*>(connection_data);
	PlayerLogout logout{};

	// 切断されたクライアントのIDを検索
	for (const Client& client : self->clients)
	{
		//if (client.enlConnection != connection)continue;// 切断されたクライアントとコネクションが違う場合continue
		//logout.id = client.player->id;
	}
	for (const Client& client : self->clients)
	{
		if (client.enlConnection == connection)continue;// 送信者には送らない
		ENLWriteRecord(
			client.enlConnection,
			static_cast<uint16_t>(NetworkTag::Logout),
			&logout,
			sizeof(logout)
		);
	}

	// プレイヤー情報削除
	self->EraseClient(connection);

	self->_logs.push_back(u8"切断\t" + connection);
}

// 接続されたときのコールバック関数
void ServerAssignment::Accept(ENLServer server, void* server_data, ENLConnection connection)
{
	// レコードが読み込み可能になった際に呼ばれるCallBack関数の設定
	SetReadCallback(connection, ReadRecord);
	// ユーザが切断したときに呼ばれるCallBack関数の設定
	SetDisconnectCallback(connection, Disconnect);

	// メンバ変数へアクセス
	ServerAssignment* self = reinterpret_cast<ServerAssignment*>(server_data);

	// connectionに紐づける
	SetClientData(connection, self);

	// サーバにプレイヤー追加
	//Player* player = new Player();
	//player->id = self->id;
	//player->position = DirectX::XMFLOAT3(0, 0, 0);
	//player->angle = DirectX::XMFLOAT3(0, 0, 0);
	//player->targetPosition = DirectX::XMFLOAT3(0, 0, 0);
	//player->state = Player::State::Idle;

	Client client;
	client.enlConnection = connection;
	//client.player = player;

	self->AddID();
	self->clients.emplace_back(client);
	self->_logs.push_back(u8"新規接続\t" + client.enlConnection);

	// ID送信
	PlayerLogin playerLogin{};
	//playerLogin.id = player->id;

	// クライアントに接続者送信(接続者含む)
	for (Client client : self->clients)
	{
		ENLWriteRecord(
			client.enlConnection,
			static_cast<uint16_t>(NetworkTag::Login),
			&playerLogin,
			sizeof(playerLogin)
		);
	}
	// 接続者に既存ログインユーザ送信
	//for (Client client : self->clients)
	//{
	//	if (client.player->id == playerLogin.id)continue;
	//	PlayerSync player{};
	//	player.id = client.player->id;
	//	player.position = client.player->position;
	//	player.targetPosition = client.player->targetPosition;
	//	player.state = client.player->state;

	//	ENLWriteRecord(
	//		connection,
	//		static_cast<uint16_t>(NetworkTag::Sync),
	//		&player,
	//		sizeof(player)
	//	);
	//}
}

void ServerAssignment::DrawGui()
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

void ServerAssignment::EraseClient(ENLConnection connection)
{
	int i = 0;
	for (Client client : clients)
	{
		if (client.enlConnection == connection)
		{
			ENLClose(client.enlConnection);
			//delete client.player;
			//client.player = nullptr;
			break;
		}
		++i;
	}
	clients.erase(clients.begin() + i);
}
