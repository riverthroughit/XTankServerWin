#pragma once
#include "PlayerId.h"
#include "Player.h"
#include <memory>
#include <map>

class PlayerManager {

	std::map<PlayerId, std::shared_ptr<Player>> players;

public:

	void Init() {}

	std::shared_ptr<Player> CreatePlayer(const PlayerId& playerId);

	void RemovePlayer(const PlayerId& playerId);

	std::shared_ptr<Player> GetPlayer(const PlayerId& playerId);

	int GetPlayerRoomId(const PlayerId& playerId);

	void SetPlayerRoomId(const PlayerId& playerId, int roomId);

	void SetPlayerState(const PlayerId& playerId, PlAYER_STATE::Type state);
};