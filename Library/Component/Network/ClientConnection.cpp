#include "ClientConnection.h"

ClientConnection::~ClientConnection()
{
    if (_client)
    {
        // サーバー終了処理
        _client->Exit();
    }
}

// 開始処理
void ClientConnection::Start()
{
    // サーバー接続
    _client = std::make_shared<ClientAssignment>();
    _client->Execute();
}

// 更新処理
void ClientConnection::Update(float elapsedTime)
{
    // サーバー更新
    _client->Update();
}

// Gui描画
void ClientConnection::DrawGui()
{
    // サーバーGUI表示
    _client->DrawGui();
}
