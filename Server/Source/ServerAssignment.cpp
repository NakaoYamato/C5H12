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

/// データ送信
#ifdef USE_MRS
void ServerAssignment::WriteRecord(MrsConnection connection, Network::DataTag tag, const void* data, uint32_t dataLength)
#else
void ServerAssignment::WriteRecord(ENLConnection connection, Network::DataTag tag, const void* data, uint32_t dataLength)
#endif
{
#ifdef USE_MRS
	mrs_write_record(
		connection,					// 送信先コネクション
		NETWORK_RECORD_OPTION,		// 通信オプション
		static_cast<uint16>(tag),	// データコマンド
		data,						// 送信データ
		dataLength					// 送信サイズ
	);
#else
	ENLWriteRecord(connection, static_cast<uint16>(tag), data, dataLength);
#endif // USE_MRS
}

/// リーダーの選定
void ServerAssignment::SelectingLeader()
{
	// リーダーがいるか確認
	for (const Client& client : clients)
	{
		if (client.player.isLeader)
		{
			// リーダーがいる場合は処理しない
			return;
		}
	}

	playerLeaderID = -1; // リーダーIDを初期化
	// 最初のプレイヤーをリーダーにする
	if (!clients.empty())
	{
		playerLeaderID = clients[0].player.uniqueID;
		clients[0].player.isLeader = true;
		std::cout << "リーダー選定: " << playerLeaderID << std::endl;
	}
	// リーダーがいる場合は、リーダーの情報を全クライアントに送信
	if (playerLeaderID != -1)
	{
		PlayerSetLeader setLeader{};
		setLeader.playerUniqueID = playerLeaderID;
		for (const Client& client : clients)
		{
			WriteRecord(client.connection, Network::DataTag::PlayerSetLeader, &setLeader, sizeof(setLeader));
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
		std::cout << massageData.playerUniqueID << ":" << massageData.message << std::endl;
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
	case DataTag::PlayerSetLeader:
	{
		// PlayerSetLeaderはクライアントが受け取るだけなので処理することはない
		return;
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

		//std::cout << "RECV Move" << std::endl;
		//std::cout << playerMove.id << std::endl;
		//std::cout << "position x:" << playerMove.position.x;
		//std::cout << "y:" << playerMove.position.y;
		//std::cout << "z:" << playerMove.position.z << std::endl;
		//std::cout << "velocity x:" << playerMove.velocity.x;
		//std::cout << "y:" << playerMove.velocity.y;
		//std::cout << "z:" << playerMove.velocity.z << std::endl;
		//std::cout << "state :" << playerMove.state;

		// 送信元のプレイヤー情報を保存
		auto client = self->GetClientFromID(playerMove.playerUniqueID);
		if (client != nullptr)
		{
			client->player.position = playerMove.position;
			client->player.movement = playerMove.movement;
			client->player.angleY = playerMove.angleY;
			client->player.state = playerMove.state;
			client->player.subState = playerMove.subState;
		}
	}
	break;
	case DataTag::PlayerApplyDamage:
	{
		// プレイヤーにダメージを適用する場合の処理
		PlayerApplyDamage playerDamage;
		// バッファデータからpayLoadStrにデータに読み込み
		if (!buffer.Read(&playerDamage, payloadLen)) {
			std::cout << "Read Error" << std::endl;
			return;
		}
		std::cout << "RECV PlayerApplyDamage" << std::endl;
		// プレイヤー情報を更新
		auto client = self->GetClientFromID(playerDamage.playerUniqueID);
		if (client != nullptr)
		{
			client->player.health -= playerDamage.damage; // ダメージを適用
		}
	}
	break;
	case DataTag::EnemyCreate:
	{
		// クライアントからサーバに敵生成要求が来た場合の処理
		EnemyCreate enemyCreate;
		// バッファデータからpayLoadStrにデータに読み込み
		if (!buffer.Read(&enemyCreate, payloadLen)) {
			std::cout << "Read Error" << std::endl;
			return;
		}

		// 敵のユニークIDを設定
		enemyCreate.uniqueID = self->enemyNextUniqueID++;
		std::cout << "RECV EnemyCreate" << std::endl;

		// 敵情報を保存
		Enemy enemy;
		enemy.type = enemyCreate.type;
		enemy.uniqueID = enemyCreate.uniqueID;
		enemy.leaderID = enemyCreate.leaderID; // リーダーIDを設定
		enemy.position = enemyCreate.position;
		enemy.angleY = enemyCreate.angleY;
		enemy.health = enemyCreate.health;
		self->enemiesMap[enemyCreate.uniqueID] = enemy;

		payload = &enemyCreate; // 送信データを再設定
	}
	break;
	case DataTag::EnemySync:
	{
		// EnemySyncはクライアントが受け取るだけなので処理することはない
		return;
	}
	break;
	case DataTag::EnemyMove:
	{
		// クライアントからサーバに敵移動要求が来た場合の処理
		EnemyMove enemyMove;
		// バッファデータからpayLoadStrにデータに読み込み
		if (!buffer.Read(&enemyMove, payloadLen)) {
			std::cout << "Read Error" << std::endl;
			return;
		}
		std::cout << "RECV EnemyMove" << std::endl;

		//　送信元の敵情報を保存
		auto& enemy = self->enemiesMap[enemyMove.uniqueID];
		enemy.position = enemyMove.position;
		enemy.angleY = enemyMove.angleY;
		enemy.target = enemyMove.target;
		strcpy_s(enemy.mainState, enemyMove.mainState);
		strcpy_s(enemy.subState, enemyMove.subState);
	}
	break;
	case DataTag::EnemyApplayDamage:
	{
		// 敵にダメージを適用する場合の処理
		EnemyApplayDamage enemyDamage;
		// バッファデータからpayLoadStrにデータに読み込み
		if (!buffer.Read(&enemyDamage, payloadLen)) {
			std::cout << "Read Error" << std::endl;
			return;
		}
		std::cout << "RECV EnemyApplayDamage" << std::endl;

		// 敵情報を更新
		auto& enemy = self->enemiesMap[enemyDamage.uniqueID];
		enemy.health -= enemyDamage.damage; // ダメージを適用
	}
	break;
	}

	// 受信データを全クライアントに送信
	for (const Client& client : self->clients)
	{
		self->WriteRecord(
			client.connection,					// 送信先コネクション
			static_cast<Network::DataTag>(payloadType),	// データコマンド
			payload,								// 送信データ
			payloadLen								// 送信サイズ
		);
	}
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
		logout.playerUniqueID = client.player.uniqueID;
	}
	for (const Client& client : self->clients)
	{
		if (client.connection == connection)continue;// 送信者には送らない
		self->WriteRecord(client.connection, Network::DataTag::PlayerLogout, &logout, sizeof(logout));
	}

	// プレイヤー情報削除
	self->EraseClient(connection);

	std::cout << "切断\t" << connection << std::endl;

	// リーダー選定
	self->SelectingLeader();
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
	player.uniqueID = self->playerNextUniqueID;
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
	playerLogin.playerUniqueID = player.uniqueID;

	// クライアントに接続者送信(接続者含む)
	for (const Client& client : self->clients)
	{
		self->WriteRecord(client.connection, Network::DataTag::PlayerLogin, &playerLogin, sizeof(playerLogin));
	}
	// "接続者"に既存ログインユーザ送信
	for (const Client& client : self->clients)
	{
        // 送信者には送らない
		if (client.player.uniqueID == playerLogin.playerUniqueID)continue;

		PlayerSync player{};
		player.playerUniqueID = client.player.uniqueID;
		player.position = client.player.position;
		player.angleY = client.player.angleY;

		self->WriteRecord(connection, Network::DataTag::PlayerSync, &player, sizeof(player));
	}
	// "接続者"に既存敵送信
	for (const auto& enemyPair : self->enemiesMap)
	{
		const Enemy& enemy = enemyPair.second;
		EnemySync enemySync{};
		enemySync.type = enemy.type;
		enemySync.uniqueID = enemy.uniqueID;
		enemySync.leaderID = enemy.leaderID; // リーダーIDを設定
		enemySync.position = enemy.position;
		enemySync.angleY = enemy.angleY;
		enemySync.health = enemy.health;
		self->WriteRecord(connection, Network::DataTag::EnemySync, &enemySync, sizeof(enemySync));
	}

	// リーダー選定
	self->SelectingLeader();
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
