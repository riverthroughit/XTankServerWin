#include "PlayerManager.h"
#include <cassert>

std::shared_ptr<Player> PlayerManager::CreatePlayer(const PlayerId& playerId)
{
    auto player = std::make_shared<Player>();
    player->id = playerId;
    auto ite = players.insert({ playerId,player });
    return ite.first->second;
}

void PlayerManager::RemovePlayer(const PlayerId& playerId)
{
    players.erase(playerId);
}

std::shared_ptr<Player> PlayerManager::GetPlayer(const PlayerId& playerId)
{
    auto ite = players.find(playerId);
    assert(ite != players.end());
    return ite->second;
}

int PlayerManager::GetPlayerRoomId(const PlayerId& playerId)
{
    auto player = GetPlayer(playerId);
    return player->roomId;
}

void PlayerManager::SetPlayerRoomId(const PlayerId& playerId, int roomId)
{
    auto player = GetPlayer(playerId);
    player->roomId = roomId;
}

void PlayerManager::SetPlayerState(const PlayerId& playerId, PlAYER_STATE::Type state)
{
    auto player = GetPlayer(playerId);
    player->state = state;
}
