#include "NetworkMediator.h"

#include "../../Source/Enemy/Wyvern/WyvernActor.h"

#include <string>
#include <imgui.h>

NetworkMediator::~NetworkMediator()
{
    // サーバー終了処理
    _client.Exit();
    // 音声レコーダーの停止
    _voiceRecorder.StopRecording();
}

// 生成時処理
void NetworkMediator::OnCreate()
{
    // 音声レコーダーの初期化
    _voiceRecorder.StartRecording();
}

void NetworkMediator::OnStart()
{
    // コールバック設定
    SetClientCollback();
}

/// 更新前処理
void NetworkMediator::OnPreUpdate(float elapsedTime)
{
	// ステートによる処理分岐
    switch (_state)
    {
    case NetworkMediator::State::Awaiting:
		// 接続開始フラグが立っている場合、接続を開始
		if (_isConnecting)
		{
			_isConnecting = false; // フラグを下ろす
			_state = NetworkMediator::State::Connecting; // ステートを接続中に変更
			// サーバー接続開始
			_client.Execute(_ipAddress.c_str());
		}
        break;
    case NetworkMediator::State::Connecting:
        // サーバー更新
        _client.Update();

        /// 受け取ったデータを処理
        ProcessNetworkData();

        // リーダーの再設定解決処理
        if (nextLeaderPlayerId != -1)
            ResetLeader();

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
            _client.WriteRecord(Network::DataTag::EnemyCreate, &enemyCreate, sizeof(enemyCreate));
        }
        break;
    }
}

// 遅延更新処理
void NetworkMediator::OnLateUpdate(float elapsedTime)
{
    // ステートによる処理分岐
    switch (_state)
    {
    case NetworkMediator::State::Awaiting:
        break;
    case NetworkMediator::State::Connecting:
        /// 自身のプレイヤーのダメージ送信
        SendMyPlayerDamage();
        /// 敵のダメージ送信
        SendEnemyDamage();

        // 送信タイマー更新
		_sendTimer += elapsedTime;
		// 一定時間ごとに送信
        if (_sendTimer >= _sendInterval)
		{
			_sendTimer -= _sendInterval; // タイマーリセット

            // プレイヤーの移動情報送信
            auto playerActor = _players[myPlayerId].playerActor.lock();
            if (playerActor)
            {
                Network::PlayerMove playerMove{};
                playerMove.playerUniqueID = myPlayerId;
                playerMove.position = playerActor->GetTransform().GetPosition();
                auto playerController = _players[myPlayerId].playerController.lock();
                if (playerController)
                {
                    auto stateMachine = playerController->GetPlayerStateMachine();
                    if (stateMachine)
                    {
                        playerMove.movement = stateMachine->GetMovement();
                        playerMove.state = Network::GetPlayerMainStateFromName(stateMachine->GetStateName());
                        playerMove.subState = Network::GetPlayerSubStateFromName(stateMachine->GetSubStateName());
                    }
                }
                // サーバーに送信
                _client.WriteRecord(Network::DataTag::PlayerMove, &playerMove, sizeof(playerMove));
            }

            // 敵の移動情報送信
            for (auto& [uniqueID, enemyData] : _enemies)
            {
                // 自身が管理していない敵ならスキップ
                if (enemyData.controllerID != myPlayerId)
                    continue;

                auto enemyActor = enemyData.enemyActor.lock();
                auto enemyController = enemyData.enemyController.lock();
                if (enemyActor && enemyController)
                {
                    Network::EnemyMove enemyMove{};
                    enemyMove.uniqueID = uniqueID;
                    enemyMove.position = enemyActor->GetTransform().GetPosition();
                    enemyMove.angleY = enemyActor->GetTransform().GetRotation().y;
                    enemyMove.target = enemyController->GetTargetPosition();
                    strcpy_s(enemyMove.mainState, enemyController->GetStateName());
                    strcpy_s(enemyMove.subState, enemyController->GetSubStateName());
                    // サーバーに送信
                    _client.WriteRecord(Network::DataTag::EnemyMove, &enemyMove, sizeof(enemyMove));
                }
            }
        }
        break;
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
    if (_players.find(id) != _players.end() && _players[id].playerActor.lock())
        return _players[id].playerActor;

    auto player = _scene->RegisterActor<PlayerActor>("Player" + std::to_string(id), ActorTag::Player, isControlled);

    // ユーザーが操作するプレイヤーか
    if (isControlled)
    {
        myPlayerId = id;
    }

    // コンテナに登録
    _players[id].playerActor = player;
	_players[id].playerController = player->GetComponent<PlayerController>();
	_players[id].damageable = player->GetComponent<Damageable>();
    return player;
}

/// 敵の生成
std::weak_ptr<EnemyActor> NetworkMediator::CreateEnemy(
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

        EnemyData enemyData{};
        enemyData.controllerID = controllerID;
        enemyData.enemyActor = enemy;
        enemyData.enemyController = enemy->GetComponent<EnemyController>();
        enemyData.damageable = enemy->GetComponent<Damageable>();

        enemy->GetTransform().SetPosition(position);
        enemy->GetTransform().SetAngleY(angleY);
        enemy->SetIsExecuteBehaviorTree(controllerID == myPlayerId);
        enemyData.damageable.lock()->ResetHealth(health);


        // コンテナに登録
        _enemies[uniqueID] = enemyData;
        return enemyData.enemyActor;
    }
    return std::weak_ptr<EnemyActor>();
}

/// 自身のプレイヤーのダメージ送信
void NetworkMediator::SendMyPlayerDamage()
{
	// スレッドセーフ
	std::lock_guard<std::mutex> lock(_mutex);
	// 自身のプレイヤーのダメージを送信
    auto myPlayer = _players[myPlayerId];
	if (myPlayer.playerActor.lock())
	{
		auto damageable = myPlayer.damageable.lock();
		if (damageable && damageable->GetLastDamage() > 0.0f)
		{
			Network::PlayerApplyDamage playerApplyDamage{};
			playerApplyDamage.playerUniqueID = myPlayerId; // ダメージを受けたプレイヤーのユニークID
			playerApplyDamage.damage = damageable->GetLastDamage();
			playerApplyDamage.hitPosition = damageable->GetHitPosition();
			// サーバーに送信
			_client.WriteRecord(Network::DataTag::PlayerApplyDamage, &playerApplyDamage, sizeof(playerApplyDamage));
		}
	}
}

/// 敵のダメージ送信
void NetworkMediator::SendEnemyDamage()
{
	// スレッドセーフ
	std::lock_guard<std::mutex> lock(_mutex);
	// 敵のダメージを送信
	for (auto& [uniqueID, enemyData] : _enemies)
	{
		auto enemyActor = enemyData.enemyActor.lock();
		if (enemyActor)
		{
			auto damageable = enemyData.damageable.lock();
			if (damageable && damageable->GetLastDamage() > 0.0f)
			{
				Network::EnemyApplayDamage enemyApplayDamage{};
				enemyApplayDamage.playerUniqueID = myPlayerId; // ダメージを与えたプレイヤーのユニークID
                enemyApplayDamage.uniqueID = uniqueID;
                enemyApplayDamage.damage = damageable->GetLastDamage();
                enemyApplayDamage.hitPosition = damageable->GetHitPosition();
				// サーバーに送信
				_client.WriteRecord(Network::DataTag::EnemyApplayDamage, &enemyApplayDamage, sizeof(enemyApplayDamage));
			}
		}
	}
}

/// リーダーの再設定
void NetworkMediator::ResetLeader()
{
    leaderPlayerId = nextLeaderPlayerId;

	// 自身がリーダーになった場合敵の行動遷移処理を自身が行う
	if (myPlayerId == leaderPlayerId)
	{
		// 敵の行動遷移処理を自身が行う
		for (auto& [uniqueID, enemyData] : _enemies)
		{
			auto enemyActor = enemyData.enemyActor.lock();
			if (enemyActor)
			{
                enemyActor->SetIsExecuteBehaviorTree(true);
			}
		}
	}
	// 次のリーダーのユニークIDをリセット
	nextLeaderPlayerId = -1;
}

/// サーバーからの各種データ受け取りを行ったときのコールバック関数設定
void NetworkMediator::SetClientCollback()
{
    _client.SetPlayerMessageDataCallback(
        [this](const Network::MessageData& messageData)
        {
            // スレッドセーフ
            std::lock_guard<std::mutex> lock(_mutex);

			_messageDatas.push_back(messageData);
        });
    _client.SetPlayerSyncCallback(
        [this](const Network::PlayerSync& playerSync)
        {
            // スレッドセーフ
            std::lock_guard<std::mutex> lock(_mutex);

            _logs.push_back("PlayerSync" + std::to_string(playerSync.playerUniqueID));

            _playerSyncs.push_back(playerSync);
        });
    _client.SetPlayerLoginCallback(
        [this](const Network::PlayerLogin& playerLogin)
        {
            // スレッドセーフ
            std::lock_guard<std::mutex> lock(_mutex);

            _logs.push_back("PlayerLogin" + std::to_string(playerLogin.playerUniqueID));

            _playerLogins.push_back(playerLogin);
        });
    _client.SetPlayerLogoutCallback(
        [this](const Network::PlayerLogout& playerLogout)
        {
            // スレッドセーフ
            std::lock_guard<std::mutex> lock(_mutex);

            _logs.push_back("PlayerLogout" + std::to_string(playerLogout.playerUniqueID));

            _playerLogouts.push_back(playerLogout);
        });
	_client.SetPlayerSetLeaderCallback(
		[this](const Network::PlayerSetLeader& playerSetLeader)
		{
			// スレッドセーフ
			std::lock_guard<std::mutex> lock(_mutex);
			_logs.push_back("PlayerSetLeader" + std::to_string(playerSetLeader.playerUniqueID));

			// 次のリーダーのユニークIDを設定
			nextLeaderPlayerId = playerSetLeader.playerUniqueID;
		});
    _client.SetPlayerMoveCallback(
        [this](const Network::PlayerMove& playerMove)
        {
            // スレッドセーフ
            std::lock_guard<std::mutex> lock(_mutex);

            _logs.push_back("PlayerMove" + std::to_string(playerMove.playerUniqueID));

            _playerMoves.push_back(playerMove);
        });
	_client.SetPlayerApplyDamageCallback(
		[this](const Network::PlayerApplyDamage& playerApplyDamage)
		{
			// スレッドセーフ
			std::lock_guard<std::mutex> lock(_mutex);
			_logs.push_back("PlayerApplyDamage" + std::to_string(playerApplyDamage.playerUniqueID));
			_playerApplyDamages.push_back(playerApplyDamage);
		});
	_client.SetEnemyCreateCallback(
		[this](const Network::EnemyCreate& enemyCreate)
		{
			// スレッドセーフ
			std::lock_guard<std::mutex> lock(_mutex);
			_logs.push_back("EnemyCreate" + std::to_string(enemyCreate.uniqueID));

            _enemyCreates.push_back(enemyCreate);
		});
	_client.SetEnemySyncCallback(
		[this](const Network::EnemySync& enemySync)
		{
			// スレッドセーフ
			std::lock_guard<std::mutex> lock(_mutex);
			_logs.push_back("EnemySync" + std::to_string(enemySync.uniqueID));

            _enemySyncs.push_back(enemySync);
		});
	_client.SetEnemyMoveCallback(
		[this](const Network::EnemyMove& enemyMove)
		{
			// スレッドセーフ
			std::lock_guard<std::mutex> lock(_mutex);
			_logs.push_back("EnemyMove" + std::to_string(enemyMove.uniqueID));

			_enemyMoves.push_back(enemyMove);
		});
	_client.SetEnemyApplayDamageCallback(
		[this](const Network::EnemyApplayDamage& enemyApplayDamage)
		{
			// スレッドセーフ
			std::lock_guard<std::mutex> lock(_mutex);
			_logs.push_back("EnemyApplayDamage" + std::to_string(enemyApplayDamage.uniqueID));

            _enemyApplayDamages.push_back(enemyApplayDamage);
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
		auto playerActor = _players[sync.playerUniqueID].playerActor.lock();
        if (!playerActor)
        {
            // プレイヤーが存在しないなら作成
            playerActor = CreatePlayer(sync.playerUniqueID, false).lock();
        }
        playerActor->GetTransform().SetPosition(sync.position);
        playerActor->GetTransform().SetAngleY(sync.angleY);
    }
    _playerSyncs.clear();
    //===============================================================================

    //===============================================================================
    // ログアウトデータの処理
    for (auto& logout : _playerLogouts)
    {
        // プレイヤー情報を削除
        auto playerActor = _players[logout.playerUniqueID].playerActor.lock();
        if (playerActor)
        {
            // プレイヤー削除
            playerActor->Remove();
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
        auto playerActor = _players[move.playerUniqueID].playerActor.lock();
        if (playerActor == nullptr)
            continue;

		// 自身の場合はスキップ
		if (move.playerUniqueID == myPlayerId)
			continue;

        playerActor->GetTransform().SetPosition(move.position);
        // プレイヤーの状態を更新
        auto playerController = _players[move.playerUniqueID].playerController.lock();
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
	// プレイヤーのダメージデータの処理
	for (auto& playerApplyDamage : _playerApplyDamages)
	{
		// 自身が送信したダメージならスキップ
		if (playerApplyDamage.playerUniqueID == myPlayerId)
			continue;

		// プレイヤー情報を更新
        auto playerActor = _players[playerApplyDamage.playerUniqueID].playerActor.lock();
		if (playerActor)
		{
            auto damageable = _players[playerApplyDamage.playerUniqueID].damageable.lock();
			if (damageable)
			{
				damageable->SetHelth(damageable->GetHealth() - playerApplyDamage.damage);
				damageable->SetHitPosition(playerApplyDamage.hitPosition);
			}
		}
	}
	_playerApplyDamages.clear();
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

	//===============================================================================
	// 敵のダメージデータの処理
	for (auto& enemyApplayDamage : _enemyApplayDamages)
	{
		// 自身が送信したダメージならスキップ
		if (enemyApplayDamage.playerUniqueID == myPlayerId)
			continue;

		auto& enemyData = _enemies[enemyApplayDamage.uniqueID];
		auto enemyActor = enemyData.enemyActor.lock();
		if (enemyActor)
		{
            auto damageable = enemyData.damageable.lock();
			if (damageable)
			{
				damageable->SetHelth(damageable->GetHealth() - enemyApplayDamage.damage);
				damageable->SetHitPosition(enemyApplayDamage.hitPosition);
			}
		}
	}
	_enemyApplayDamages.clear();
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
			ImGui::InputText(u8"IPアドレス", &_ipAddress);
            if (_state == State::Awaiting)
            {
                if (ImGui::Button(u8"接続開始"))
                {
					_isConnecting = true;
                }
            }

            ImGui::Separator();

			ImGui::DragFloat(u8"送信間隔", &_sendInterval, 0.1f, 0.1f, 10.0f, u8"%.1f秒");
            ImGui::Separator();

            if (ImGui::Button(u8"ワイバーン追加"))
            {
                if (myPlayerId != -1 && myPlayerId == leaderPlayerId)
                {
                    _sendEnemyCreate = true; // 敵生成フラグを立てる

                    // 敵の生成命令を送る
                    Network::EnemyCreate enemyCreate{};
                    enemyCreate.type = Network::EnemyType::Wyvern; // ここではワイバーンを生成する例
                    enemyCreate.uniqueID = -1; // ユニークIDは適宜設定
                    enemyCreate.leaderID = myPlayerId; // リーダーのユニークIDを設定
                    enemyCreate.position = Vector3(0.0f, 10.0f, 10.0f);
                    enemyCreate.health = 100.0f; // 初期体力を設定
                    _client.WriteRecord(Network::DataTag::EnemyCreate, &enemyCreate, sizeof(enemyCreate));
                }
            }

            // サーバーGUI表示
            _client.DrawGui();

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
				_client.WriteRecord(Network::DataTag::Message, &messageData, sizeof(messageData));
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
