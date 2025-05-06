#include "ServerAssignment.h"


using namespace Network;

/// 開始処理
void ServerAssignment::Execute()
{
	// サーバ情報設定
	// IPアドレス、ポート番号、バックログ数を指定
	char address[40] = { "0.0.0.0" };
	uint32_t backlog = 10;

	// ENLライブラリの初期化
	if (!ENLInitialize())
	{
		std::cout << "\tサーバー初期化失敗" << std::endl;
		return;
	}
	std::cout << "サーバー初期化成功" << std::endl;

	// サーバオブジェクト作成(accept実行)
	mrsServer = ENLServerCreate(ENL_CONNECTION_TYPE, address, ENL_PORT_ADDRESS, backlog);
	if (mrsServer <= 0)
	{
		std::cout << "\tサーバーオブジェクト生成失敗" << std::endl;
		return;
	}
	std::cout << "サーバーオブジェクト生成成功" << std::endl;
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
	}
	clients.clear();

	ENLClose(mrsServer);
	std::cout << "サーバー封鎖" << std::endl;

	// mrs終了処理
	ENLFinalize();
	std::cout << "サーバー終了" << std::endl;
}

/// 終了処理
void ServerAssignment::Exit()
{
	while (loop) {
		std::string input;
		std::cin >> input;
		if (input == "exit")
		{
			loop = false;
		}
		else if (input == "get")
		{
			std::cout << "============================================" << std::endl;
			// プレイヤー情報を表示
			int index = 0;
			for (const Client& client : clients)
			{
				std::cout << "id : " << index << std::endl;
				std::cout << "position : " << client.player.position.x << client.player.position.y << client.player.position.z << std::endl;
				std::cout << "angle : " << client.player.angle.x << client.player.angle.y << client.player.angle.z << std::endl;
				std::cout << "scale : " << client.player.scale.x << client.player.scale.y << client.player.scale.z << std::endl;
				std::cout << "============================================" << std::endl;

				index++;
			}
		}
	}
}

#pragma region コールバック関数
/// レコードが読み込み可能になった際に呼ばれるコールバック関数
void ServerAssignment::ReadRecord(ENLConnection connection, void* connection_data, uint16_t payload_type, const void* payload, uint32_t payload_len)
{
	ServerAssignment* self = reinterpret_cast<ServerAssignment*>(connection_data);
	ENLBuffer buffer;
	// 受信データをバッファに書き込み
	buffer.Write(payload, payload_len);

	switch (static_cast<Network::DataTag>(payload_type))
	{
	case DataTag::Message:
	{
        MessageData massageData;
		// バッファデータからpayLoadStrにデータに読み込み
		if (!buffer.Read(&massageData, payload_len)) {
			std::cout << "Read Error" << std::endl;
		}
		// メッセージ表示
		std::cout << "RECV Message" << std::endl;
		std::cout << massageData.id << ":" << massageData.message << std::endl;
	}
	break;
	case DataTag::Login:
	{
		// Loginはクライアントが受け取るだけなので処理することはない
		return;
	}
	break;
	case DataTag::Logout:
	{
		// サーバからクライアント情報を削除
		int eraseId = self->EraseClient(connection);

		std::cout << "RECV Logout" << std::endl;
		std::cout << eraseId << ":" << "Logout" << std::endl;
	}
	break;
	case DataTag::Sync:
	{
		std::cout << "RECV Sync" << std::endl;
        // クライアントからSyncを受信したときは受信元以外のクライアントに送信する
		for (const Client& client : self->clients)
		{
            // 受信者には送らない
			if (client.enlConnection == connection)
			{
				std::cout << client.player.id << ":" << "Sync" << std::endl;
                continue;
			}

			ENLWriteRecord(client.enlConnection, payload_type, payload, payload_len);
		}
		return;
	}
	break;
	case DataTag::Move:
	{
		PlayerMove playerMove;
		// バッファデータからpayLoadStrにデータに読み込み
		if (!buffer.Read(&playerMove, payload_len)) {
			std::cout << "Read Error" << std::endl;
		}

		std::cout << "RECV Logout" << std::endl;
		std::cout << playerMove.id << std::endl;
		std::cout << "velocity x:" << playerMove.velocity.x;
		std::cout << "y:" << playerMove.velocity.y;
		std::cout << "z:" << playerMove.velocity.z << std::endl;

		// 送信元のプレイヤー情報を保存
        auto client = self->GetClient(playerMove.id);
		if (client != nullptr)
		{
			//client->player.position = payloadData.position;
			//client->player.targetPosition = payloadData.clickPosition;
			//client->player.state = Player::State::Move;
		}
	}
	break;
	case DataTag::Attack:
	{
		//PlayerInput payloadData;
		//// バッファデータからpayLoadStrにデータに読み込み
		//if (!buffer.Read(&payloadData, payload_len)) {
		//	std::cout << "Read Error" << std::endl;
		//}

		//std::cout << "recv data attack id:" << std::to_string(payloadData.id) << std::endl;
		//std::cout << "position x:" << payloadData.position.x;
		//std::cout << "y:" << payloadData.position.y;
		//std::cout << "z:" << payloadData.position.z << std::endl;
		//std::cout << "click_position x:" << payloadData.clickPosition.x;
		//std::cout << "y:" << payloadData.clickPosition.y;
		//std::cout << "z:" << payloadData.clickPosition.z << std::endl;

		//for (const Client& client : self->clients)
		//{
		//	// 同じIDの場合
		//	if (client.player->id == payloadData.id)
		//	{
		//		client.player->position = payloadData.position;
		//		client.player->targetPosition = payloadData.clickPosition;
		//		client.player->state = Player::State::Move;
		//	}
		//	// 受信データをクライアントに送信
		//	ENLWriteRecord(client.enlConnection, payload_type, payload, payload_len);
		//}
		break;
	}
	}

	// 受信データを全クライアントに送信
	for (const Client& client : self->clients)
	{
		ENLWriteRecord(client.enlConnection, payload_type, payload, payload_len);
	}
}

// ユーザが切断したときに呼ばれるコールバック関数
void ServerAssignment::Disconnect(ENLConnection connection, void* connection_data)
{
	ServerAssignment* self = reinterpret_cast<ServerAssignment*>(connection_data);
	PlayerLogout logout{};

	// 切断されたクライアントのIDを検索
	for (const Client& client : self->clients)
	{
		if (client.enlConnection != connection)continue;// 切断されたクライアントとコネクションが違う場合continue
		logout.id = client.player.id;
	}
	for (const Client& client : self->clients)
	{
		if (client.enlConnection == connection)continue;// 送信者には送らない
		ENLWriteRecord(
			client.enlConnection,
			static_cast<uint16_t>(DataTag::Logout),
			&logout,
			sizeof(logout)
		);
	}

	// プレイヤー情報削除
	self->EraseClient(connection);

	std::cout << "切断\t" << connection << std::endl;
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
	Player player = Player();
	player.id = self->id;
	player.position = DirectX::XMFLOAT3(0, 0, 0);
	player.angle = DirectX::XMFLOAT3(0, 0, 0);

	Client newClient;
	newClient.enlConnection = connection;
	newClient.player = player;

	self->AddID();
	self->clients.emplace_back(newClient);
	std::cout << "新規接続\t" << newClient.enlConnection << std::endl;

	// ID送信
	PlayerLogin playerLogin{};
	playerLogin.id = player.id;

	// クライアントに接続者送信(接続者含む)
	for (const Client& client : self->clients)
	{
		ENLWriteRecord(
			client.enlConnection,
			static_cast<uint16_t>(DataTag::Login),
			&playerLogin,
			sizeof(playerLogin)
		);
	}
	// 接続者に既存ログインユーザ送信
	for (const Client& client : self->clients)
	{
        // 送信者には送らない
		if (client.player.id == playerLogin.id)continue;

		PlayerSync player{};
		player.id = client.player.id;
		player.position = client.player.position;
		player.angle = client.player.angle;

		ENLWriteRecord(
			connection,
			static_cast<uint16_t>(DataTag::Sync),
			&player,
			sizeof(player)
		);
	}
}
#pragma endregion

int ServerAssignment::EraseClient(ENLConnection connection)
{
	int i = 0;
	for (Client& client : clients)
	{
		if (client.enlConnection == connection)
		{
			ENLClose(client.enlConnection);
			break;
		}
		++i;
	}
	clients.erase(clients.begin() + i);
    return i;
}
