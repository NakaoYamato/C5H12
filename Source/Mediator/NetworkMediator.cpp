#include "NetworkMediator.h"

#include "../../Source/Enemy/Wyvern/WyvernActor.h"

#include <string>
#include <imgui.h>

NetworkMediator::~NetworkMediator()
{
    if (_client)
    {
        // サーバー終了処理
        _client->Exit();
    }
}

void NetworkMediator::OnCreate()
{
    // サーバー作成
    _client = std::make_shared<ClientAssignment>();
    SetClientCollback();
    _client->Execute();
}

void NetworkMediator::OnStart()
{
}

/// 更新前処理
void NetworkMediator::OnPreUpdate(float elapsedTime)
{
    // サーバー更新
    _client->Update();

    /// 受け取ったデータを処理
    ProcessNetworkData();

	// 自身がリーダーで、敵が存在しない場合は敵を生成
	if (myPlayerId != -1 && myPlayerId == leaderPlayerId && _enemies.empty() && !_sendEnemyCreate)
	{
		_sendEnemyCreate = true; // 敵生成フラグを立てる

		// 敵の生成命令を送る
		Network::EnemyCreate enemyCreate{};
		enemyCreate.type = Network::EnemyType::Wyvern; // ここではワイバーンを生成する例
		enemyCreate.uniqueID = -1; // ユニークIDは適宜設定
		enemyCreate.leaderID = myPlayerId; // リーダーのユニークIDを設定
        enemyCreate.position = Vector3(0.0f, 10.0f, 10.0f);
		enemyCreate.health = 100.0f; // 初期体力を設定
        _client->WriteRecord(Network::DataTag::EnemyCreate, &enemyCreate, sizeof(enemyCreate));
	}
}

// 遅延更新処理
void NetworkMediator::OnLateUpdate(float elapsedTime)
{
    // プレイヤーの同期処理
    _syncTimer += elapsedTime;
    if (_syncTimer >= _syncTime && _players[myPlayerId].lock())
    {
        //auto myPlayer = _players[myPlayerId].lock();
        //// プレイヤーの同期
        //Network::AllPlayerSync sync{};
        //// players[0]にデータを入れる
        //sync.players[0].id = myPlayerId;
        //sync.players[0].position = myPlayer->GetTransform().GetPosition();
        //sync.players[0].angleY = myPlayer->GetTransform().GetRotation().y;
        //auto playerController = myPlayer->GetPlayerController();
        //if (playerController != nullptr)
        //{
        //    auto stateMachine = playerController->GetPlayerStateMachine();
        //    if (stateMachine != nullptr)
        //    {
        //        sync.players[0].state = GetPlayerMainStateFromName(stateMachine->GetStateName());
        //        sync.players[0].subState = GetPlayerSubStateFromName(stateMachine->GetSubStateName());
        //    }
        //}
        //// サーバーに送信
        //_client->WriteRecord(Network::DataTag::AllSync, &sync, sizeof(sync));

        _syncTimer = 0.0f;
    }
}

// 固定間隔更新処理
void NetworkMediator::OnFixedUpdate()
{
	// プレイヤーの移動情報送信
    auto myPlayer = _players[myPlayerId].lock();
	if (myPlayer)
    {
		Network::PlayerMove playerMove{};
		playerMove.playerUniqueID = myPlayerId;
		playerMove.position = myPlayer->GetTransform().GetPosition();
        auto playerController = myPlayer->GetPlayerController();
        if (playerController != nullptr)
        {
            auto stateMachine = playerController->GetPlayerStateMachine();
            if (stateMachine != nullptr)
            {
				playerMove.movement = stateMachine->GetMovement();
                playerMove.state = Network::GetPlayerMainStateFromName(stateMachine->GetStateName());
                playerMove.subState = Network::GetPlayerSubStateFromName(stateMachine->GetSubStateName());
            }
        }
        // サーバーに送信
        _client->WriteRecord(Network::DataTag::PlayerMove, &playerMove, sizeof(playerMove));
    }

	// 敵の移動情報送信
	for (auto& [uniqueID, enemyData] : _enemies)
	{
        // 自身が管理していない敵ならスキップ
        if (enemyData.controllerID != myPlayerId)
            continue;

		auto enemyController = enemyData.enemyController.lock();
		if (enemyController)
		{
			Network::EnemyMove enemyMove{};
			enemyMove.uniqueID = uniqueID;
			enemyMove.position = enemyController->GetActor()->GetTransform().GetPosition();
			enemyMove.angleY = enemyController->GetActor()->GetTransform().GetRotation().y;
			enemyMove.target = enemyController->GetTargetPosition();
            strcpy_s(enemyMove.mainState, enemyController->GetStateName());
            strcpy_s(enemyMove.subState, enemyController->GetSubStateName());
			// サーバーに送信
			_client->WriteRecord(Network::DataTag::EnemyMove, &enemyMove, sizeof(enemyMove));
		}
	}
}

void NetworkMediator::OnDrawGui()
{
    /// ネットワークGUIの表示
    DrawNetworkGui();

    /// ログの表示
    DrawLogGui();

    /// メッセージの表示
    DrawMessageGui();
}

/// プレイヤー作成
std::weak_ptr<PlayerActor> NetworkMediator::CreatePlayer(int id, bool isControlled)
{
    // 要素チェック
    if (_players[id].lock()) return std::weak_ptr<PlayerActor>();

    // ユーザーが操作するプレイヤーか
    if (isControlled)
    {
        myPlayerId = id;
    }

    auto player = _scene->RegisterActor<PlayerActor>("Player" + std::to_string(id), ActorTag::Player, isControlled);

    // コンテナに登録
    _players[id] = player;
    return player;
}

/// 敵の生成
std::weak_ptr<EnemyController> NetworkMediator::CreateEnemy(
    int uniqueID,
    int controllerID, 
    Network::EnemyType type, 
    const Vector3& position,
    float angleY,
    float health)
{
    if (type == Network::EnemyType::Wyvern)
    {
        // 敵キャラクターの生成
        auto enemy = _scene->RegisterActor<WyvernActor>(
            "Enemy" + std::to_string(uniqueID),
            ActorTag::Enemy
        );
        enemy->GetTransform().SetPosition(position);
        enemy->GetTransform().SetAngleY(angleY);
        enemy->SetExecuteBehaviorTree(controllerID == myPlayerId);
		enemy->GetWyvernEnemyController().lock()->ResetHealth(health);

        EnemyData enemyData{};
        enemyData.controllerID = controllerID;
        enemyData.enemyController = enemy->GetWyvernEnemyController();

        // コンテナに登録
        _enemies[uniqueID] = enemyData;
        return enemyData.enemyController;
    }
    return std::weak_ptr<EnemyController>();
}

/// サーバーからの各種データ受け取りを行ったときのコールバック関数設定
void NetworkMediator::SetClientCollback()
{
    _client->SetPlayerMessageDataCallback(
        [this](const Network::MessageData& messageData)
        {
            // スレッドセーフ
            std::lock_guard<std::mutex> lock(_mutex);

			_messageDatas.push_back(messageData);
        });
    _client->SetPlayerSyncCallback(
        [this](const Network::PlayerSync& playerSync)
        {
            // スレッドセーフ
            std::lock_guard<std::mutex> lock(_mutex);

            _logs.push_back("PlayerSync" + std::to_string(playerSync.playerUniqueID));

            _playerSyncs.push_back(playerSync);
        });
    _client->SetPlayerLoginCallback(
        [this](const Network::PlayerLogin& playerLogin)
        {
            // スレッドセーフ
            std::lock_guard<std::mutex> lock(_mutex);

            _logs.push_back("PlayerLogin" + std::to_string(playerLogin.playerUniqueID));

            _playerLogins.push_back(playerLogin);
        });
    _client->SetPlayerLogoutCallback(
        [this](const Network::PlayerLogout& playerLogout)
        {
            // スレッドセーフ
            std::lock_guard<std::mutex> lock(_mutex);

            _logs.push_back("PlayerLogout" + std::to_string(playerLogout.playerUniqueID));

            _playerLogouts.push_back(playerLogout);
        });
	_client->SetPlayerSetLeaderCallback(
		[this](const Network::PlayerSetLeader& playerSetLeader)
		{
			// スレッドセーフ
			std::lock_guard<std::mutex> lock(_mutex);
			_logs.push_back("PlayerSetLeader" + std::to_string(playerSetLeader.playerUniqueID));

			// リーダー設定
            leaderPlayerId = playerSetLeader.playerUniqueID;
		});
    _client->SetPlayerMoveCallback(
        [this](const Network::PlayerMove& playerMove)
        {
            // スレッドセーフ
            std::lock_guard<std::mutex> lock(_mutex);

            _logs.push_back("PlayerMove" + std::to_string(playerMove.playerUniqueID));

            _playerMoves.push_back(playerMove);
        });
	_client->SetEnemyCreateCallback(
		[this](const Network::EnemyCreate& enemyCreate)
		{
			// スレッドセーフ
			std::lock_guard<std::mutex> lock(_mutex);
			_logs.push_back("EnemyCreate" + std::to_string(enemyCreate.uniqueID));

            _enemyCreates.push_back(enemyCreate);
		});
	_client->SetEnemySyncCallback(
		[this](const Network::EnemySync& enemySync)
		{
			// スレッドセーフ
			std::lock_guard<std::mutex> lock(_mutex);
			_logs.push_back("EnemySync" + std::to_string(enemySync.uniqueID));

            _enemySyncs.push_back(enemySync);
		});
	_client->SetEnemyMoveCallback(
		[this](const Network::EnemyMove& enemyMove)
		{
			// スレッドセーフ
			std::lock_guard<std::mutex> lock(_mutex);
			_logs.push_back("EnemyMove" + std::to_string(enemyMove.uniqueID));

			_enemyMoves.push_back(enemyMove);
		});
}

/// 受け取ったデータを処理
void NetworkMediator::ProcessNetworkData()
{
    // スレッドセーフ
    std::lock_guard<std::mutex> lock(_mutex);

    //===============================================================================
    // ログインデータの処理
    for (auto& login : _playerLogins)
    {
        // プレイヤー作成
        CreatePlayer(login.playerUniqueID, myPlayerId == -1);
    }
    _playerLogins.clear();
    //===============================================================================

    //===============================================================================
    // 同期データの処理
    for (auto& sync : _playerSyncs)
    {
        // プレイヤー情報を更新
        auto player = _players[sync.playerUniqueID].lock();
        if (!player)
        {
            // プレイヤーが存在しないなら作成
            player = CreatePlayer(sync.playerUniqueID, false).lock();
        }
        player->GetTransform().SetPosition(sync.position);
        player->GetTransform().SetAngleY(sync.angleY);
    }
    _playerSyncs.clear();
    //===============================================================================

    //===============================================================================
    // ログアウトデータの処理
    for (auto& logout : _playerLogouts)
    {
        // プレイヤー情報を削除
        auto player = _players[logout.playerUniqueID].lock();
        if (player)
        {
            // プレイヤー削除
            player->Remove();
            _players.erase(logout.playerUniqueID);
        }
    }
    _playerLogouts.clear();
    //===============================================================================

	//===============================================================================
	// 移動データの処理
	for (auto& move : _playerMoves)
	{
		// プレイヤー情報を更新
		auto player = _players[move.playerUniqueID].lock();
        if (!player)
            continue;
		// 自身の場合はスキップ
		if (move.playerUniqueID == myPlayerId)
			continue;
		player->GetTransform().SetPosition(move.position);
        // プレイヤーの状態を更新
        auto playerController = player->GetPlayerController();
        if (playerController != nullptr)
        {
            auto stateMachine = playerController->GetPlayerStateMachine();
            if (stateMachine != nullptr)
            {
				stateMachine->SetMovement(move.movement);
                stateMachine->ChangeState(move.state, move.subState);
            }
        }
	}
    _playerMoves.clear();
	//===============================================================================

	//===============================================================================
	// 敵生成データの処理
	for (auto& enemyCreate : _enemyCreates)
	{
        CreateEnemy(
            enemyCreate.uniqueID,
            enemyCreate.leaderID,
            enemyCreate.type,
            enemyCreate.position,
            enemyCreate.angleY,
            enemyCreate.health);
	}
	_enemyCreates.clear();
	//===============================================================================

    //===============================================================================
	// 敵同期データの処理
	for (auto& enemySync : _enemySyncs)
	{
        if (_enemies.find(enemySync.uniqueID) == _enemies.end())
        {
			// 敵が存在しない場合は生成
            CreateEnemy(
                enemySync.uniqueID,
                enemySync.leaderID,
                enemySync.type,
                enemySync.position,
                enemySync.angleY,
                enemySync.health);
        }
	}
	_enemySyncs.clear();
    //===============================================================================

	//===============================================================================
	// 敵の移動データの処理
	for (auto& enemyMove : _enemyMoves)
	{
        if (_enemies.find(enemyMove.uniqueID) == _enemies.end())
			continue; // 敵が存在しない場合はスキップ

		auto& enemyData = _enemies[enemyMove.uniqueID];
		// 敵情報を更新
		auto enemy = enemyData.enemyController.lock();
		if (!enemy)
			continue;
		// 自身が管理している敵ならスキップ
        if (enemyData.controllerID == myPlayerId)
            continue;
		enemy->GetActor()->GetTransform().SetPosition(enemyMove.position);
		enemy->GetActor()->GetTransform().SetAngleY(enemyMove.angleY);
		enemy->SetTargetPosition(enemyMove.target);
		enemy->ChangeState(enemyMove.mainState, enemyMove.subState);
	}
	_enemyMoves.clear();
	//===============================================================================
}

/// ネットワークGUIの表示
void NetworkMediator::DrawNetworkGui()
{
    static ImGuiTabBarFlags tab_bar_flags =
        ImGuiTabBarFlags_AutoSelectNewTabs |
        ImGuiTabBarFlags_Reorderable |
        ImGuiTabBarFlags_FittingPolicyResizeDown;
    if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
    {
        if (ImGui::BeginTabItem(u8"サーバー"))
        {
            ImGui::DragFloat(u8"データ同期間隔(秒)", &_syncTime, 0.001f, 0.001f, 2.0f);

            // サーバーGUI表示
            _client->DrawGui();

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
}

/// ログの表示
void NetworkMediator::DrawLogGui()
{
    static ImGuiTabBarFlags tab_bar_flags =
        ImGuiTabBarFlags_AutoSelectNewTabs |
        ImGuiTabBarFlags_Reorderable |
        ImGuiTabBarFlags_FittingPolicyResizeDown;
    if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
    {
        if (ImGui::BeginTabItem(u8"ログ"))
        {
            bool flag = myPlayerId == leaderPlayerId;
			ImGui::Checkbox(u8"リーダーかどうか", &flag);

            auto logs = GetLogs();
            ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(0, 0), ImGuiWindowFlags_NoTitleBar);
            for (std::string message : logs) {
                ImGui::Text(u8"%s", message.c_str());
            }
            ImGui::EndChild();
            ImGui::Spacing();

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
}

void NetworkMediator::DrawMessageGui()
{
    static ImGuiTabBarFlags tab_bar_flags =
        ImGuiTabBarFlags_AutoSelectNewTabs |
        ImGuiTabBarFlags_Reorderable |
        ImGuiTabBarFlags_FittingPolicyResizeDown;
    if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
    {
        if (ImGui::BeginTabItem(u8"チャット"))
        {
            ImGui::InputText(u8"メッセージ", &_message);
            if (ImGui::Button(u8"送信"))
            {
				Network::MessageData messageData{};
				messageData.playerUniqueID = myPlayerId;
				_message.copy(messageData.message, sizeof(messageData.message) - 1);
				// メッセージデータを送信
				_client->WriteRecord(Network::DataTag::Message, &messageData, sizeof(messageData));
				// メッセージをクリア
				_message.clear();
            }
            auto& messages = _messageDatas;
            ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(0, 0), ImGuiWindowFlags_NoTitleBar);
            for (auto& message : messages) {
                ImGui::Text(u8"%d : %s", message.playerUniqueID, message.message);
            }
            ImGui::EndChild();
            ImGui::Spacing();

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
}
