#pragma once
#include "PlayerId.h"
#include "TypeConfig.h"
#include <map>
#include "Room.h"
#include <queue>
#include "Protobuf/XTankMsg.pb.h"
#include <memory>
#include "Util/TickUtil.h"

class RoomManager {

	std::map<int, Room> rooms;

	std::priority_queue<int,std::vector<int>,std::greater<int>> roomIds;

	std::set<std::shared_ptr<Player>> lobbyPlayers;

	TickUtil tickUtil;

private:

	Room& GetRoom(int roomId);

	void SendLobbyNtf();

public:

	void Init();

	void AddPlayerToLobby(std::shared_ptr<Player> player);

	void RemovePlayerFromLobby(std::shared_ptr<Player> player);

	void RemovePlayerFromRoom(int roomId, std::shared_ptr<Player> player);

	void AddPlayerToRoom(int roomId, std::shared_ptr<Player> player);

	int CreateRoom();

	bool IsRoomInGame(int roomId);

	void RemoveRoom(int roomId);

	void AddPlayerGameInput(int roomId, std::shared_ptr<Player> player,int frameId, BUTTON::Type btnType);

	void Tick();
};