#pragma once

#include "../Component.h"
#include "../../Library/Network/ClientAssignment.h"

class ClientConnection : public Component
{
public:
    ClientConnection() = default;
    ~ClientConnection() override;

    // 名前取得
    const char* GetName() const override { return "ClientConnection"; }
    // 開始処理
    void Start() override;
    // 更新処理
    void Update(float elapsedTime) override;
    // Gui描画
    void DrawGui() override;

#pragma region アクセサ
    std::weak_ptr<ClientAssignment> GetClient() { return _client; }
#pragma endregion
private:
    std::shared_ptr<ClientAssignment> _client;
};