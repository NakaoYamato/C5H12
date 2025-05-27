#include "ServerAssignment.h"


using namespace Network;

/// 開始処理
void ServerAssignment::Execute()
{
#ifdef USE_MRS
	// サーバ情報設定
	// IPアドレス、ポート番号、バックログ数を指定
	char address[40] = { "0.0.0.0" };
	int32 backlog = 10;

	// mrsライブラリの初期化
	// trueなら正常に完了。falseの場合失敗。
	if (!mrs_initialize())
	{
		std::cout << "\tサーバー初期化失敗" << std::endl;
		return;
	}
	std::cout << "サーバー初期化成功" << std::endl;

	// サーバオブジェクト作成(accept実行)
	mrsServer = mrs_server_create(NETWORK_CONNECTION_TYPE, address, NETWORK_PORT_ADDRESS, backlog);
	if (mrsServer == nullptr)
	{
		std::cout << "\tサーバーオブジェクト生成失敗" << std::endl;
		return;
	}
	std::cout << "サーバーオブジェクト生成成功" << std::endl;
	mrs_server_set_data(mrsServer, this);
	// 接続されたときのコールバック関数設定
	//	mrs_server_createで作ったサーバオブジェクトの指定
	//	接続成功時に実行したい処理の関数ポインタを指定
	mrs_server_set_new_connection_callback(mrsServer, Accept);

	// サーバ側からコマンド入力で終了されるまでループする。
	// キーボードでexitを入力するとループを抜けるための別スレッドを用意
	std::thread th(&ServerAssignment::Exit, this);

	do {
		mrs_update();
	} while (loop);
	th.join();

	// クライアント全削除
	for (Client client : clients)
	{
		mrs_close(client.connection);
		client.connection = 0;
	}
	clients.clear();

    mrs_close(mrsServer);
	std::cout << "サーバー封鎖" << std::endl;
	mrs_update();

	// mrs終了処理
	mrs_finalize();
	std::cout << "サーバー終了" << std::endl;
#else
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
	mrsServer = ENLServerCreate(NETWORK_CONNECTION_TYPE, address, NETWORK_PORT_ADDRESS, backlog);
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
		ENLClose(client.connection);
		client.connection = 0;
	}
	clients.clear();

	ENLClose(mrsServer);
	std::cout << "サーバー封鎖" << std::endl;

	// mrs終了処理
	ENLFinalize();
	std::cout << "サーバー終了" << std::endl;
#endif // USE_MRS
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
				std::cout << "angle Y: " << client.player.angleY << std::endl;
				std::cout << "============================================" << std::endl;

				index++;
			}
		}
	}
}

#pragma region コールバック関数
/// レコードが読み込み可能になった際に呼ばれるコールバック関数
#ifdef USE_MRS
void ServerAssignment::ReadRecord(MrsConnection connection, void* connectionData, uint32 seqnum, uint16 options, uint16 payloadType, const void* payload, uint32 payloadLen)
#else
void ServerAssignment::ReadRecord(ENLConnection connection, void* connectionData, uint16_t payloadType, const void* payload, uint32_t payloadLen)
#endif // USE_MRS
{
	ServerAssignment* self = reinterpret_cast<ServerAssignment*>(connectionData);
#ifdef USE_MRS
	mrs::Buffer buffer;
#else
	ENLBuffer buffer;
#endif // USE_MRS
	// 受信データをバッファに書き込み
	buffer.Write(payload, payloadLen);

	switch (static_cast<Network::DataTag>(payloadType))
	{
	case DataTag::Message:
	{
        MessageData massageData;
		// バッファデータからpayLoadStrにデータに読み込み
		if (!buffer.Read(&massageData, payloadLen)) {
			std::cout << "Read Error" << std::endl;
			return;
		}
		// メッセージ表示
		std::cout << "RECV Message" << std::endl;
		std::cout << massageData.id << ":" << massageData.message << std::endl;
	}
	break;
	case DataTag::PlayerLogin:
	{
		// Loginはクライアントが受け取るだけなので処理することはない
		return;
	}
	break;
	case DataTag::PlayerLogout:
	{
		// サーバからクライアント情報を削除
		int eraseId = self->EraseClient(connection);

		std::cout << "RECV Logout" << std::endl;
		std::cout << eraseId << ":" << "Logout" << std::endl;
	}
	break;
	case DataTag::PlayerSync:
	{
		// Syncはクライアントが受け取るだけなので処理することはない
		return;
	}
	break;
	case DataTag::PlayerMove:
	{
		PlayerMove playerMove;
		// バッファデータからpayLoadStrにデータに読み込み
		if (!buffer.Read(&playerMove, payloadLen)) {
			std::cout << "Read Error" << std::endl;
			return;
		}

		std::cout << "RECV Move" << std::endl;
		//std::cout << playerMove.id << std::endl;
		//std::cout << "position x:" << playerMove.position.x;
		//std::cout << "y:" << playerMove.position.y;
		//std::cout << "z:" << playerMove.position.z << std::endl;
		//std::cout << "velocity x:" << playerMove.velocity.x;
		//std::cout << "y:" << playerMove.velocity.y;
		//std::cout << "z:" << playerMove.velocity.z << std::endl;
		//std::cout << "state :" << playerMove.state;

		// 送信元のプレイヤー情報を保存
        auto client = self->GetClientFromID(playerMove.id);
		if (client != nullptr)
		{
			client->player.position = playerMove.position;
			client->player.state = playerMove.state;
		}
	}
	break;
	}

#ifdef USE_MRS
	// 受信データを全クライアントに送信
	for (const Client& client : self->clients)
	{
		mrs_write_record(
			client.connection,						// 送信先コネクション
			NETWORK_RECORD_OPTION,	// 通信オプション
			payloadType,								// データコマンド
			payload,								// 送信データ
			payloadLen							// 送信サイズ
		);
	}
#else
	// 受信データを全クライアントに送信
	for (const Client& client : self->clients)
	{
		ENLWriteRecord(client.connection, payloadType, payload, payloadLen);
	}
#endif // USE_MRS
}

// ユーザが切断したときに呼ばれるコールバック関数
#ifdef USE_MRS
void ServerAssignment::Disconnect(MrsConnection connection, void* connectionData)
#else
void ServerAssignment::Disconnect(ENLConnection connection, void* connectionData)
#endif // USE_MRS
{
	ServerAssignment* self = reinterpret_cast<ServerAssignment*>(connectionData);
	PlayerLogout logout{};

	// 切断されたクライアントのIDを検索
	for (const Client& client : self->clients)
	{
		if (client.connection != connection)continue;// 切断されたクライアントとコネクションが違う場合continue
		logout.id = client.player.id;
	}
	for (const Client& client : self->clients)
	{
		if (client.connection == connection)continue;// 送信者には送らない
#ifdef USE_MRS
		mrs_write_record(
			client.connection,						// 送信先コネクション
			NETWORK_RECORD_OPTION,	// 通信オプション
			static_cast<uint16>(Network::DataTag::PlayerLogout),	// データコマンド
			&logout,								// 送信データ
			sizeof(logout)							// 送信サイズ
		);
#else
		ENLWriteRecord(
			client.connection,
			static_cast<uint16_t>(DataTag::Logout),
			&logout,
			sizeof(logout)
		);
#endif // USE_MRS
	}

	// プレイヤー情報削除
	self->EraseClient(connection);

	std::cout << "切断\t" << connection << std::endl;
}

// 接続されたときのコールバック関数
#ifdef USE_MRS
void ServerAssignment::Accept(MrsServer server, void* serverData, MrsConnection connection)
#else
void ServerAssignment::Accept(ENLServer server, void* serverData, ENLConnection connection)
#endif // USE_MRS
{
	// メンバ変数へアクセス
	ServerAssignment* self = reinterpret_cast<ServerAssignment*>(serverData);

#ifdef USE_MRS
	// connectionに紐づける
	mrs_connection_set_data(connection, self);
	// レコードが読み込み可能になった際に呼ばれるCallBack関数の設定
	mrs_set_read_record_callback(connection, ReadRecord);
	// ユーザが切断したときに呼ばれるCallBack関数の設定
	mrs_set_disconnect_callback(connection, Disconnect);
#else
	// レコードが読み込み可能になった際に呼ばれるCallBack関数の設定
	SetReadCallback(connection, ReadRecord);
	// ユーザが切断したときに呼ばれるCallBack関数の設定
	SetDisconnectCallback(connection, Disconnect);

	// connectionに紐づける
	SetClientData(connection, self);
#endif // USE_MRS

	// サーバにプレイヤー追加
	Player player = Player();
	player.id = self->playerNextUniqueID;
	player.position = DirectX::XMFLOAT3(0, 0, 0);
	player.angleY = 0.0f;

	Client newClient;
	newClient.connection = connection;
	newClient.player = player;

	// ユニークIDをインクリメント
	self->playerNextUniqueID++;
	self->clients.emplace_back(newClient);
	std::cout << "新規接続\t" << newClient.connection << std::endl;

	// ID送信
	PlayerLogin playerLogin{};
	playerLogin.id = player.id;

	// クライアントに接続者送信(接続者含む)
	for (const Client& client : self->clients)
	{
#ifdef USE_MRS
		mrs_write_record(
			client.connection,						// 送信先コネクション
			NETWORK_RECORD_OPTION,	// 通信オプション
			static_cast<uint16>(Network::DataTag::PlayerLogin),	// データコマンド
			&playerLogin,								// 送信データ
			sizeof(playerLogin)							// 送信サイズ
		);
#else
		ENLWriteRecord(
			client.connection,
			static_cast<uint16_t>(DataTag::Login),
			&playerLogin,
			sizeof(playerLogin)
		);
#endif // USE_MRS
	}
	// 接続者に既存ログインユーザ送信
	for (const Client& client : self->clients)
	{
        // 送信者には送らない
		if (client.player.id == playerLogin.id)continue;

		PlayerSync player{};
		player.id = client.player.id;
		player.position = client.player.position;
		player.angleY = client.player.angleY;

#ifdef USE_MRS
		mrs::Buffer send;
		send.Write(&player, sizeof(player));
		mrs_write_record(
			connection,						// 送信先コネクション
			NETWORK_RECORD_OPTION,	// 通信オプション
			static_cast<uint16>(Network::DataTag::PlayerSync),	// データコマンド
			send.GetData(),								// 送信データ
			send.GetDataLen()							// 送信サイズ
		);
#else
		ENLWriteRecord(
			connection,
			static_cast<uint16_t>(DataTag::Sync),
			&player,
			sizeof(player)
		);
#endif // USE_MRS
	}

	// 最初の接続者なら敵を生成する
	if (self->clients.size() == 1)
	{
		// 敵生成
		EnemyCreate enemyCreate{};
		enemyCreate.type = EnemyType::Wyvern;
		enemyCreate.uniqueID = self->enemyNextUniqueID; // ユニークIDは0で初期化
		enemyCreate.position = DirectX::XMFLOAT3(0, 10.0f, 10.0f);
		enemyCreate.angleY = 0.0f;
	}
}
#pragma endregion

#ifdef USE_MRS
/// クライアント削除
int ServerAssignment::EraseClient(MrsConnection connection)
#else
/// クライアント削除
int ServerAssignment::EraseClient(ENLConnection connection)
#endif // USE_MRS
{
	int i = 0;
	for (Client& client : clients)
	{
		if (client.connection == connection)
		{
#ifdef USE_MRS
			mrs_close(client.connection);
#else
			ENLClose(client.connection);
#endif // USE_MRS
			break;
		}
		++i;
	}
	clients.erase(clients.begin() + i);
    return i;
}
