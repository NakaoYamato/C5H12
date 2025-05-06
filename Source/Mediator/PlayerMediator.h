#pragma once

#include <NetworkDefine.h>
#include "../Actor/Player/PlayerActor.h"

class PlayerMediator
{
public:
    struct PlayerInfo
    {
        int networkId = -1;
        std::weak_ptr<PlayerActor> player;
    };

public:
    PlayerMediator() = default;
    ~PlayerMediator() = default;

    void AddPlayer(std::weak_ptr<PlayerActor> player)
    {
        _players.push_back({ -1, player });
    }
    void RemovePlayer(std::weak_ptr<PlayerActor> player)
    {
        _players.erase(std::remove_if(_players.begin(), _players.end(),
            [&player](const PlayerInfo& p) { return p.player.lock() == player.lock(); }), _players.end());
    }

#pragma region ネットワーク処理
    void ReceiveSyncData(const Network::PlayerSync& playerSync);

#pragma endregion

private:
    std::vector<PlayerInfo> _players;
};