#include "RoomManager.h"
#include <cassert>
#include "Config.h"
#include "Socket/MessageQueue.h"
#include "Socket/SocketConfig.h"

Room& RoomManager::GetRoom(int roomId)
{
    auto ite = rooms.find(roomId);
    assert(ite != rooms.end());
    return ite->second;
}

void RoomManager::SendLobbyNtf()
{
    auto msgPtr = std::make_shared<XTankMsg::LobbyNtf>();

    for (auto& [roomId, room] : rooms) {
        auto roomMsg = msgPtr->add_rooms();
        roomMsg->set_id(roomId);
        roomMsg->set_playernum(room.GetPlayersNum());
        roomMsg->set_ingame(room.IsInGame());
    }

    for (auto& playerPtr : lobbyPlayers) {
        MsgSendQueue::Instance().PushMsg({ playerPtr->id,MessageData{XTankMsg::LOBBY_NTF,msgPtr} });
    }
}

void RoomManager::Init()
{
    for (int i = 0; i < MAX_ROOM_NUM; ++i) {
        roomIds.push(i);
    }

    tickUtil.SetTickTime(OUT_GAME_MSG_TICK);
}

void RoomManager::AddPlayerToLobby(std::shared_ptr<Player> player)
{
    lobbyPlayers.insert(player);
}

void RoomManager::RemovePlayerFromLobby(std::shared_ptr<Player> player)
{
    lobbyPlayers.erase(player);
}

void RoomManager::RemovePlayerFromRoom(int roomId, std::shared_ptr<Player> player)
{
    Room& room = GetRoom(roomId);
    room.RemovePlayer(player);
    if (room.GetPlayersNum() == 0) {
        RemoveRoom(roomId);
    }
}

void RoomManager::AddPlayerToRoom(int roomId, std::shared_ptr<Player> player)
{
    Room& room = GetRoom(roomId);
    room.AddPlayer(player);
}

int RoomManager::CreateRoom()
{
    int id = roomIds.top();
    roomIds.pop();
    auto [ite,suc] = rooms.insert({id, Room()});
    assert(suc);
    ite->second.Init();

    return id;
}

bool RoomManager::IsRoomInGame(int roomId)
{
    Room& room = GetRoom(roomId);
    return room.IsInGame();
}


void RoomManager::RemoveRoom(int roomId)
{
    rooms.erase(roomId);
    roomIds.push(roomId);
}

void RoomManager::AddPlayerGameInput(int roomId, std::shared_ptr<Player> player, int frameId, BUTTON::Type btnType)
{
    Room& room = GetRoom(roomId);
    room.AddPlayerGameInput(player, frameId, btnType);
}

void RoomManager::Tick()
{
    tickUtil.Tick();
    if (tickUtil.IsNeedTick()) {

        SendLobbyNtf();
    }

    for (auto& [roomId, room] : rooms) {
        room.Tick();
    }
}
