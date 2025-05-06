#pragma once

#include <mutex>

#include "../../Library/Component/Network/ClientConnection.h" 
#include "../Actor/Player/PlayerActor.h"

class NetworkMediator
{
public:
    struct PlayerInfo
    {
        int networkId = -1;
        std::weak_ptr<PlayerActor> player;
    };

public:
    NetworkMediator() = default;
    ~NetworkMediator() = default;

    void SetClientCollback(std::weak_ptr<ClientConnection> clientConnection);

    void AddPlayer(std::weak_ptr<PlayerActor> player)
    {
        _players.push_back({ -1, player });
    }
    void RemovePlayer(std::weak_ptr<PlayerActor> player)
    {
        _players.erase(std::remove_if(_players.begin(), _players.end(),
            [&player](const PlayerInfo& p) { return p.player.lock() == player.lock(); }), _players.end());
    }

    const std::vector<std::string>& GetLogs()
    {
        // スレッドセーフ
        std::lock_guard<std::mutex> lock(_mutex);
        return _logs;
    }
private:
    std::vector<PlayerInfo> _players;
    std::weak_ptr<ClientConnection> _clientConnection;

    std::mutex _mutex;
    std::vector<std::string> _logs;
};