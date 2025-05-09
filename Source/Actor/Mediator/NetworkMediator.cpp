#include "NetworkMediator.h"

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
    Actor::OnCreate();

    // サーバー作成
    _client = std::make_shared<ClientAssignment>();
    SetClientCollback();
    _client->Execute();
}

void NetworkMediator::Start()
{
    Actor::Start();
}

void NetworkMediator::Update(float elapsedTime)
{
    // サーバー更新
    _client->Update();

    /// 受け取ったデータを処理
    ProcessNetworkData();

    Actor::Update(elapsedTime);

    // プレイヤーの同期処理
    _syncTimer += elapsedTime;
    if (_syncTimer >= _syncTime && _players[myPlayerId].lock())
    {
        auto myPlayer = _players[myPlayerId].lock();
        // プレイヤーの同期
        Network::AllPlayerSync sync{};
        // players[0]にデータを入れる
        sync.players[0].id          = myPlayerId;
        sync.players[0].position    = myPlayer->GetTransform().GetPosition();
        sync.players[0].angle       = myPlayer->GetTransform().GetRotation();
        // サーバーに送信
        _client->WriteRecord(Network::DataTag::AllSync, &sync, sizeof(sync));

        _syncTimer = 0.0f;
    }
}

// 固定間隔更新処理
void NetworkMediator::FixedUpdate()
{
    Actor::FixedUpdate();

	// プレイヤーの移動情報送信
    auto myPlayer = _players[myPlayerId].lock();
	if (myPlayer)
    {
		Network::PlayerMove playerMove{};
		playerMove.id = myPlayerId;
		playerMove.position = myPlayer->GetTransform().GetPosition();
		playerMove.velocity = myPlayer->GetCharactorController()->GetVelocity();
        playerMove.state = 0;
        // サーバーに送信
        _client->WriteRecord(Network::DataTag::Move, &playerMove, sizeof(playerMove));
    }
}

void NetworkMediator::DrawGui()
{
    Actor::DrawGui();

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

            _logs.push_back("PlayerSync" + std::to_string(playerSync.id));

            _playerSyncs.push_back(playerSync);
        });
    _client->SetAllPlayerSyncCallback(
        [this](const Network::AllPlayerSync& allPlayerSync)
        {
            // スレッドセーフ
            std::lock_guard<std::mutex> lock(_mutex);

            _logs.push_back("AllPlayerSync");

            _allPlayerSyncs.push_back(allPlayerSync);
        });
    _client->SetPlayerLoginCallback(
        [this](const Network::PlayerLogin& playerLogin)
        {
            // スレッドセーフ
            std::lock_guard<std::mutex> lock(_mutex);

            _logs.push_back("PlayerLogin" + std::to_string(playerLogin.id));

            _playerLogins.push_back(playerLogin);
        });
    _client->SetPlayerLogoutCallback(
        [this](const Network::PlayerLogout& playerLogout)
        {
            // スレッドセーフ
            std::lock_guard<std::mutex> lock(_mutex);

            _logs.push_back("PlayerLogout" + std::to_string(playerLogout.id));

            _playerLogouts.push_back(playerLogout);
        });
    _client->SetPlayerMoveCallback(
        [this](const Network::PlayerMove& playerMove)
        {
            // スレッドセーフ
            std::lock_guard<std::mutex> lock(_mutex);

            _logs.push_back("PlayerMove" + std::to_string(playerMove.id));

            _playerMoves.push_back(playerMove);
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
        CreatePlayer(login.id, myPlayerId == -1);
    }
    _playerLogins.clear();
    //===============================================================================

    //===============================================================================
    // 同期データの処理
    for (auto& sync : _playerSyncs)
    {
        // プレイヤー情報を更新
        auto player = _players[sync.id].lock();
        if (!player)
        {
            // プレイヤーが存在しないなら作成
            player = CreatePlayer(sync.id, false).lock();
        }
        player->GetTransform().SetPosition(sync.position);
        player->GetTransform().SetRotation(sync.angle);
    }
    _playerSyncs.clear();
    //===============================================================================

    //===============================================================================
    // 全体同期データの処理
    for (auto& sync : _allPlayerSyncs)
    {
        // プレイヤー情報を更新
        for (int i = 0; i < NETWORK_MAX_CONNECTION; i++)
        {
			// プレイヤーが存在しないならスキップ
            if (sync.players[i].id == -1)continue;

			// 自身のデータはスキップ
			if (sync.players[i].id == myPlayerId)continue;

            auto player = _players[sync.players[i].id].lock();
            if (!player)
            {
                // プレイヤーが存在しないなら作成
                player = CreatePlayer(sync.players[i].id, false).lock();
            }
            player->GetTransform().SetPosition(sync.players[i].position);
            player->GetTransform().SetRotation(sync.players[i].angle);
        }
    }
    _allPlayerSyncs.clear();
    //===============================================================================

    //===============================================================================
    // ログアウトデータの処理
    for (auto& logout : _playerLogouts)
    {
        // プレイヤー情報を削除
        auto player = _players[logout.id].lock();
        if (player)
        {
            // プレイヤー削除
            player->Destroy();
            _players.erase(logout.id);
        }
    }
    _playerLogouts.clear();
    //===============================================================================

	//===============================================================================
	// 移動データの処理
	for (auto& move : _playerMoves)
	{
		// プレイヤー情報を更新
		auto player = _players[move.id].lock();
        if (!player)
            continue;
		// 自身の場合はスキップ
		if (move.id == myPlayerId)
			continue;
		player->GetTransform().SetPosition(move.position);
        auto charactor = player->GetCharactorController();
		if (charactor)
		{
			charactor->SetVelocity(move.velocity);
		}
	}
    _playerMoves.clear();
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
				messageData.id = myPlayerId;
				_message.copy(messageData.message, sizeof(messageData.message) - 1);
				// メッセージデータを送信
				_client->WriteRecord(Network::DataTag::Message, &messageData, sizeof(messageData));
				// メッセージをクリア
				_message.clear();
            }
            auto& messages = _messageDatas;
            ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(0, 0), ImGuiWindowFlags_NoTitleBar);
            for (auto& message : messages) {
                ImGui::Text(u8"%d : %s", message.id, message.message);
            }
            ImGui::EndChild();
            ImGui::Spacing();

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
}
