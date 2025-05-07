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
        Network::PlayerSync sync{};
        sync.id = myPlayerId;
        sync.position = myPlayer->GetTransform().GetPosition();
        sync.angle = myPlayer->GetTransform().GetRotation();
        // サーバーに送信
        _client->WriteRecord(Network::DataTag::Sync, &sync, sizeof(sync));

        _syncTimer = 0.0f;
    }
}

void NetworkMediator::DrawGui()
{
    Actor::DrawGui();

    /// ネットワークGUIの表示
    DrawNetworkGui();

    /// ログの表示
    DrawLogGui();
}

/// サーバーからの各種データ受け取りを行ったときのコールバック関数設定
void NetworkMediator::SetClientCollback()
{
    _client->SetPlayerMessageDataCallback(
        [this](const Network::MessageData& messageData)
        {
            // スレッドセーフ
            std::lock_guard<std::mutex> lock(_mutex);

            _logs.push_back(messageData.message);
        });
    _client->SetPlayerSyncCallback(
        [this](const Network::PlayerSync& playerSync)
        {
            // スレッドセーフ
            std::lock_guard<std::mutex> lock(_mutex);

            _logs.push_back("PlayerSync" + std::to_string(playerSync.id));

            _playerSyncs.push_back(playerSync);
        });
    _client->SetPlayerLoginCallback(
        [this](const Network::PlayerLogin& playerLogin)
        {
            // スレッドセーフ
            std::lock_guard<std::mutex> lock(_mutex);

            _logs.push_back("PlayerLogin" + std::to_string(playerLogin.id));

            _playerLogins.push_back(playerLogin);

        });
}

/// 受け取ったデータを処理
void NetworkMediator::ProcessNetworkData()
{
    // スレッドセーフ
    std::lock_guard<std::mutex> lock(_mutex);

    // ログインデータの処理
    for (auto& login : _playerLogins)
    {
        // 登録されているか確認
        if (!_players[login.id].lock())
        {
            // プレイヤー生成
            auto player = _scene->RegisterActor<PlayerActor>("Player" + std::to_string(login.id), ActorTag::Player);

            // コンテナに登録
            _players[login.id] = player;
        }

        // 操作するプレイヤーがいなければ登録
        if (myPlayerId == -1)
        {
            myPlayerId = login.id;
        }
    }
    _playerLogins.clear();

    // 同期データの処理
    for (auto& sync : _playerSyncs)
    {
        // プレイヤー情報を更新
        auto player = _players[sync.id].lock();
        if (!player)
        {
            // プレイヤーが存在しないなら作成
            player = _scene->RegisterActor<PlayerActor>("Player" + std::to_string(sync.id), ActorTag::Player);
            _players[sync.id] = player;
        }
        player->GetTransform().SetPosition(sync.position);
        player->GetTransform().SetRotation(sync.angle);
    }
    _playerSyncs.clear();
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
