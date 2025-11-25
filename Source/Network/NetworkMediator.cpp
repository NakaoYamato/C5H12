#include "NetworkMediator.h"

#include "../../Library/Component/BehaviorController.h"
#include "../../Source/Player/PlayerActor.h"

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
    // コールバック設定
    SetClientCollback();
}

// 更新前処理
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
            if (_ipAddress.empty())
            {
                // ステートをオフラインに変更
                _state = NetworkMediator::State::NonServer;
                _logs.push_back("NonServerMode");

                // プレイヤーの生成
                CreatePlayer(0, myPlayerId == -1);
				// TODO : メタAIに任せる
				// 敵の生成
                CreateEnemy(
                    0,
                    0,
                    Network::CharacterType::Wyvern,
                    Vector3(14.0f, 0.3f, 0.0f),
                    0.0f,
                    100.0f);
            }
            else
            {
                // ステートを接続中に変更
                _state = NetworkMediator::State::Connecting;
                // サーバー接続開始
                _client.Execute(_ipAddress.c_str());
            }
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

        // TODO : メタAIに任せる
        // 自身がリーダーで、敵が存在しない場合は敵を生成
        if (myPlayerId != -1 && myPlayerId == leaderPlayerId && !_sendEnemyCreate)
        {
            _sendEnemyCreate = true; // 敵生成フラグを立てる

            // 敵の生成命令を送る
            Network::EnemyCreate enemyCreate{};
            enemyCreate.type = Network::CharacterType::Wyvern; // ここではワイバーンを生成する例
            enemyCreate.uniqueID = -1; // ユニークIDは適宜設定
            enemyCreate.leaderID = myPlayerId; // リーダーのユニークIDを設定
            enemyCreate.position = Vector3(0.0f, 5.0f, 10.0f);
            enemyCreate.health = 100.0f; // 初期体力を設定
            _client.WriteRecord(Network::DataTag::EnemyCreate, &enemyCreate, sizeof(enemyCreate));

            //enemyCreate = {};
            //enemyCreate.type = Network::CharacterType::Weak;
            //enemyCreate.uniqueID = -1; // ユニークIDは適宜設定
            //enemyCreate.leaderID = myPlayerId; // リーダーのユニークIDを設定
            //enemyCreate.position = Vector3(5.0f, 5.0f, -5.0f);
            //enemyCreate.health = 10.0f; // 初期体力を設定
            //_client.WriteRecord(Network::DataTag::EnemyCreate, &enemyCreate, sizeof(enemyCreate));
        }
        break;
	case NetworkMediator::State::NonServer:
		// オフラインモードでは何もしない
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
		// キャラクターのダメージ送信
        for (auto& [id, character] : _characters)
        {
            auto receiver = character.receiver.lock();
            if (!receiver)
                continue; // 受信者が無効ならスキップ
            // 自身が管理しているキャラクターか確認
            if (receiver->GetManagerId() != myPlayerId)
                continue;

            auto sender = character.sender.lock();
            if (sender)
            {
				Network::CharacterApplyDamage characterApplyDamage = sender->GetApplyDamageData();
                if (characterApplyDamage.uniqueID == -1)
                    continue; // 無効なユニークIDならスキップ
                characterApplyDamage.uniqueID = id; // ユニークIDを設定
                characterApplyDamage.senderID = myPlayerId; // 送信者のIDを設定
                // サーバーに送信
				_client.WriteRecord(Network::DataTag::CharacterApplyDamage, &characterApplyDamage, sizeof(characterApplyDamage));
            }
        }

        // 送信タイマー更新
		_sendTimer += elapsedTime;
		// 一定時間ごとに送信
        if (_sendTimer >= _sendInterval)
		{
			_sendTimer -= _sendInterval; // タイマーリセット

            // スレッドセーフ
            std::lock_guard<std::mutex> lock(_mutex);
			for (auto& [id, character] : _characters)
			{
				auto receiver = character.receiver.lock();
				if (!receiver)
					continue; // 受信者が無効ならスキップ
                // 自身が管理しているキャラクターか確認
				if (receiver->GetManagerId() != myPlayerId)
					continue;

                auto sender = character.sender.lock();
                if (sender)
                {
					Network::CharacterMove characterMove = sender->GetMoveData();
					if (characterMove.uniqueID == -1)
						continue; // 無効なユニークIDならスキップ
					characterMove.uniqueID = id; // ユニークIDを設定
					characterMove.senderID = myPlayerId; // 送信者のIDを設定
					// サーバーに送信
					_client.WriteRecord(Network::DataTag::CharacterMove, &characterMove, sizeof(characterMove));
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

// サーバー開始
void NetworkMediator::ExecuteServer(const std::string& ipAddress)
{
    // 接続開始フラグを立てる
	_isConnecting = true;
    // IPアドレスを設定
	_ipAddress = ipAddress;
}

/// プレイヤー作成
std::weak_ptr<Actor> NetworkMediator::CreatePlayer(int id, bool isControlled)
{
    // 要素チェック
    if (_characters[id].actor.lock())
        return _characters[id].actor;

    auto player = _scene->RegisterActor<PlayerActor>("Player" + std::to_string(id), ActorTag::Player, isControlled);
    auto receiver = player->GetComponent<NetworkReceiver>();
	auto sender     = player->GetComponent<NetworkSender>();

    // 初期位置
    player->GetTransform().SetPosition(Vector3(-45.0f, 0.23f, 0.0f));
    player->GetTransform().SetAngleY(DirectX::XMConvertToRadians(90.0f));

    // 管理者のIDを設定
    receiver->SetManagerId(id);
    // ユーザーが操作するプレイヤーか
    if (isControlled)
    {
        myPlayerId = id;
		// ユーザーが操作するプレイヤーの場合レシーバーは起動しない
		receiver->SetActive(false);
	}

    // コンテナに登録
    _characters[id].uniqueID = id;
    _characters[id].actor = player;
    _characters[id].receiver = receiver;
	_characters[id].sender = sender;
    return player;
}

/// 敵の生成
std::weak_ptr<Actor> NetworkMediator::CreateEnemy(
    int uniqueID,
    int controllerID, 
    Network::CharacterType type,
    const Vector3& position,
    float angleY,
    float health)
{
    if (type == Network::CharacterType::Wyvern)
    {
        // 敵キャラクターの生成
        auto enemy = _scene->RegisterActor<WyvernActor>(
            "Enemy" + std::to_string(uniqueID),
            ActorTag::Enemy
        );
        auto receiver = enemy->GetComponent<NetworkReceiver>();
		auto sender = enemy->GetComponent<NetworkSender>();
        // 管理者のIDを設定
        receiver->SetManagerId(controllerID);
        // ユーザーが操作するプレイヤーの場合レシーバーは起動しない
		if (myPlayerId == controllerID)
            receiver->SetActive(false);
        // コンテナに登録
        _characters[uniqueID].uniqueID = uniqueID;
        _characters[uniqueID].actor = enemy;
        _characters[uniqueID].receiver = receiver;
		_characters[uniqueID].sender = sender;
        return enemy;
    }
    return std::weak_ptr<EnemyActor>();
}

/// リーダーの再設定
void NetworkMediator::ResetLeader()
{
    leaderPlayerId = nextLeaderPlayerId;

	// 自身がリーダーになった場合敵の行動遷移処理を自身が行う
	if (myPlayerId == leaderPlayerId)
	{
		// 敵の行動遷移処理を自身が行う
        for (auto& [id, character] : _characters)
        {
            auto behavior = character.actor.lock()->GetComponent<BehaviorController>();
            if (behavior)
            {
                behavior->SetIsExecute(true);
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
    _client.SetCharacterSyncCallback(
        [this](const Network::CharacterSync& characterSync)
        {
            // スレッドセーフ
            std::lock_guard<std::mutex> lock(_mutex);

            _logs.push_back("CharacterSync" + std::to_string(characterSync.uniqueID));

            _characterSyncs.push_back(characterSync);
        });
    _client.SetCharacterMoveCallback(
        [this](const Network::CharacterMove& characterMove)
        {
            // スレッドセーフ
            std::lock_guard<std::mutex> lock(_mutex);

            _logs.push_back("CharacterMove" + std::to_string(characterMove.uniqueID));

            _characterMoves.push_back(characterMove);
        });
	_client.SetCharacterApplyDamageCallback(
		[this](const Network::CharacterApplyDamage& characterApplyDamage)
		{
			// スレッドセーフ
			std::lock_guard<std::mutex> lock(_mutex);
			_logs.push_back("CharacterApplyDamage" + std::to_string(characterApplyDamage.uniqueID));
            _characterApplyDamages.push_back(characterApplyDamage);
		});
	_client.SetEnemyCreateCallback(
		[this](const Network::EnemyCreate& enemyCreate)
		{
			// スレッドセーフ
			std::lock_guard<std::mutex> lock(_mutex);
			_logs.push_back("EnemyCreate" + std::to_string(enemyCreate.uniqueID));

            _enemyCreates.push_back(enemyCreate);
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
    for (auto& sync : _characterSyncs)
    {
        auto& character = _characters[sync.uniqueID];
        // キャラクターが存在しない場合は生成
        if (character.actor.lock() == nullptr)
        {
            // キャラクターの生成
            switch (sync.type)
            {
            case Network::CharacterType::Player :
                // プレイヤーキャラクターの生成
                character.actor = CreatePlayer(sync.uniqueID, myPlayerId == -1);
                break;
            default:
                // 敵キャラクターの生成
                character.actor = CreateEnemy(
                    sync.uniqueID,
                    sync.senderID, // リーダーのユニークIDを設定
                    sync.type,
                    sync.position,
                    sync.angleY,
                    sync.health);
                break;
            }
        }
        // キャラクター情報を更新
        auto receiver = character.receiver.lock();
        if (receiver)
        {
            if (receiver->IsActive())
                receiver->GetEventBus().Publish<Network::CharacterSync>(sync);
        }
    }
    _characterSyncs.clear();
    //===============================================================================

    //===============================================================================
    // ログアウトデータの処理
    for (auto& logout : _playerLogouts)
    {
        // プレイヤー情報を削除
        auto playerActor = _characters[logout.playerUniqueID].actor.lock();
        if (playerActor)
        {
            // プレイヤー削除
            playerActor->Remove();
            _characters.erase(logout.playerUniqueID);
        }
    }
    _playerLogouts.clear();
    //===============================================================================

	//===============================================================================
	// 移動データの処理
	for (auto& move : _characterMoves)
	{
        auto& character = _characters[move.uniqueID];
        auto receiver = character.receiver.lock();
        if (receiver)
        {
            if (receiver->IsActive())
                receiver->GetEventBus().Publish<Network::CharacterMove>(move);
        }
	}
    _characterMoves.clear();
	//===============================================================================

    //===============================================================================
	// プレイヤーのダメージデータの処理
    for (auto& applyDamage : _characterApplyDamages)
    {
        auto& character = _characters[applyDamage.uniqueID];
        auto receiver = character.receiver.lock();
        if (receiver)
        {
            if (receiver->IsActive())
                receiver->GetEventBus().Publish<Network::CharacterApplyDamage>(applyDamage);
        }
    }
    _characterApplyDamages.clear();
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
                    enemyCreate.type = Network::CharacterType::Wyvern; // ここではワイバーンを生成する例
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
