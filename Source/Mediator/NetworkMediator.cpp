#include "NetworkMediator.h"

void NetworkMediator::SetClientCollback(std::weak_ptr<ClientConnection> clientConnection)
{
    auto client = clientConnection.lock()->GetClient().lock();

    client->SetPlayerMessageDataCallback(
        [this](const Network::MessageData& messageData)
        {
            // スレッドセーフ
            std::lock_guard<std::mutex> lock(_mutex);

            _logs.push_back(messageData.message);
        });
    client->SetPlayerLoginCallback(
        [this](const Network::PlayerLogin& playerLogin)
        {
            // スレッドセーフ
            std::lock_guard<std::mutex> lock(_mutex);

            _logs.push_back("PlayerLogin" + std::to_string(playerLogin.id));
            //for (auto& player : _playe rs)
            //{
            //    if (player.networkId == -1)
            //    {
            //        player.networkId = playerLogin.id;
            //        break;
            //    }
            //}
        });
}
