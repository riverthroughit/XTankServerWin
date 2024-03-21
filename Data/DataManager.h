#pragma once
#include "PlayerId.h"
#include "Socket/MessageQueue.h"
#include "Util/Macro.h"
#include "PlayerManager.h"
#include "RoomManager.h"
#include <memory>
#include "TypeConfig.h"

class DataManager {

	SINGLETON(DataManager);

private:

	std::unique_ptr<PlayerManager> mPlayerManager;
	std::unique_ptr<RoomManager> mRoomManager;

public:

	void Init();

	void CreatePlayer(const PlayerId& playerId);

	void RemovePlayer(const PlayerId& playerId);

	void MovePlayerToRoom(const PlayerId& playerId, int roomIndex);

	void MovePlayerFromRoomToLobby(const PlayerId& playerId);

	int CreateRoom();
	
	void RemoveRoom(int roomId);

	void SetPlayerState(const PlayerId& playerId,PlAYER_STATE::Type state);

	int GetPlayerRoomId(const PlayerId& playerId);

	bool IsRoomInGame(int roomId);

	void AddPlayerGameInput(const PlayerId& playerId, int frameId, BUTTON::Type btnType);

	void ParseMsg(const MessageData& msg);

	void Tick();

};