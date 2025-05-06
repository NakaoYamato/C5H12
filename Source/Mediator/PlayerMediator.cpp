#include "PlayerMediator.h"

void PlayerMediator::ReceiveSyncData(const Network::PlayerSync& playerSync)
{
    for (auto& player : _players)
    {
        if (player.networkId == playerSync.id)
        {
            auto actor = player.player.lock();
            if (actor)
            {
                actor->GetTransform().SetPosition(playerSync.position);
                actor->GetTransform().SetAngle(playerSync.angle);
            }
            break;
        }
    }
}
