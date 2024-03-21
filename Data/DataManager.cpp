#include "DataManager.h"
#include "Socket/MessageQueue.h"

void DataManager::Init()
{
	mPlayerManager = std::make_unique<PlayerManager>();
	mRoomManager = std::make_unique<RoomManager>();

	mPlayerManager->Init();
	mRoomManager->Init();
}

void DataManager::CreatePlayer(const PlayerId& playerId)
{
	auto player = mPlayerManager->CreatePlayer(playerId);
	mPlayerManager->SetPlayerRoomId(playerId, -1);
	mRoomManager->AddPlayerToLobby(player);
}

void DataManager::RemovePlayer(const PlayerId& playerId)
{
	auto player = mPlayerManager->GetPlayer(playerId);
	if (player->roomId == -1) {
		mRoomManager->RemovePlayerFromLobby(player);
	}
	else {
		mRoomManager->RemovePlayerFromRoom(player->roomId, player);
	}
}

void DataManager::MovePlayerToRoom(const PlayerId& playerId, int roomIndex)
{
	auto player = mPlayerManager->GetPlayer(playerId);
	mRoomManager->RemovePlayerFromLobby(player);
	mPlayerManager->SetPlayerRoomId(playerId, roomIndex);
	mRoomManager->AddPlayerToRoom(roomIndex, player);
}

void DataManager::MovePlayerFromRoomToLobby(const PlayerId& playerId)
{
	int roomId = mPlayerManager->GetPlayerRoomId(playerId);
	auto player = mPlayerManager->GetPlayer(playerId);
	mRoomManager->RemovePlayerFromRoom(roomId, player);
	mRoomManager->AddPlayerToLobby(player);
	mPlayerManager->SetPlayerRoomId(playerId, -1);
}

int DataManager::CreateRoom()
{
	return mRoomManager->CreateRoom();
}

void DataManager::RemoveRoom(int roomId)
{
	mRoomManager->RemoveRoom(roomId);
}

void DataManager::SetPlayerState(const PlayerId& playerId, PlAYER_STATE::Type state)
{
	mPlayerManager->SetPlayerState(playerId, state);
}

int DataManager::GetPlayerRoomId(const PlayerId& playerId)
{
	return mPlayerManager->GetPlayerRoomId(playerId);
}

bool DataManager::IsRoomInGame(int roomId)
{
	return mRoomManager->IsRoomInGame(roomId);
}


void DataManager::AddPlayerGameInput(const PlayerId& playerId, int frameId, BUTTON::Type btnType)
{
	int roomId = mPlayerManager->GetPlayerRoomId(playerId);
	auto player = mPlayerManager->GetPlayer(playerId);
	mRoomManager->AddPlayerGameInput(roomId, player, frameId, btnType);
}

void DataManager::ParseMsg(const MessageData& msg)
{
	

	if (msg.type == XTankMsg::NONE) {
		return;
	}

	if (msg.type == XTankMsg::PLAYER_INPUT_NTF) {

		const XTankMsg::PlayerInputNtf& req = static_cast<XTankMsg::PlayerInputNtf&>(*msg.msgPtr);
		PlayerId playerid{ req.playerid().ip(),req.playerid().pid() };
		AddPlayerGameInput(playerid, req.frameid(), static_cast<BUTTON::Type>(req.playercmd()));

	}
	else if (msg.type == XTankMsg::LOGIN_REQ) {

		const XTankMsg::LoginReq& req = static_cast<XTankMsg::LoginReq&>(*msg.msgPtr);
		PlayerId playerid{ req.playerid().ip(),req.playerid().pid() };
		CreatePlayer(playerid);
		MsgSendQueue::Instance().SendLoginAck(playerid);

	}
	else if (msg.type == XTankMsg::LOGOUT_REQ) {

		const XTankMsg::LogoutReq& req = static_cast<XTankMsg::LogoutReq&>(*msg.msgPtr);
		PlayerId playerid{ req.playerid().ip(),req.playerid().pid() };
		RemovePlayer(playerid);
		MsgSendQueue::Instance().SendLogoutAck(playerid);

	}
	else if (msg.type == XTankMsg::ROOM_JOIN_REQ) {

		const XTankMsg::RoomJoinReq& req = static_cast<XTankMsg::RoomJoinReq&>(*msg.msgPtr);
		PlayerId playerid{ req.playerid().ip(),req.playerid().pid() };
		MovePlayerToRoom(playerid, req.roomid());
		MsgSendQueue::Instance().SendRoomJoinAck(playerid);

	}
	else if (msg.type == XTankMsg::ROOM_CREATE_REQ) {

		const XTankMsg::RoomCreateReq& req = static_cast<XTankMsg::RoomCreateReq&>(*msg.msgPtr);
		PlayerId playerid{ req.playerid().ip(),req.playerid().pid() };
		int roomId = CreateRoom();
		MovePlayerToRoom(playerid, roomId);
		MsgSendQueue::Instance().SendRoomCreateAck(playerid);

	}
	else if (msg.type == XTankMsg::ROOM_EXIT_REQ) {

		const XTankMsg::RoomExitReq& req = static_cast<XTankMsg::RoomExitReq&>(*msg.msgPtr);
		PlayerId playerid{ req.playerid().ip(),req.playerid().pid() };
		MovePlayerFromRoomToLobby(playerid);
		MsgSendQueue::Instance().SendRoomExitAck(playerid);

	}
	else if (msg.type == XTankMsg::PLAYER_READY_REQ) {

		const XTankMsg::PlayerReadyReq& req = static_cast<XTankMsg::PlayerReadyReq&>(*msg.msgPtr);
		PlayerId playerid{ req.playerid().ip(),req.playerid().pid() };
		SetPlayerState(playerid, PlAYER_STATE::READY);
		MsgSendQueue::Instance().SendPlayerReadyAck(playerid);
	}
	else if (msg.type == XTankMsg::PLAYER_READY_CANCEL_REQ) {

		const XTankMsg::PlayerReadyCancelReq& req = static_cast<XTankMsg::PlayerReadyCancelReq&>(*msg.msgPtr);
		PlayerId playerid{ req.playerid().ip(),req.playerid().pid() };
		
		int roomId = GetPlayerRoomId(playerid);
		
		if (!IsRoomInGame(roomId)) {
			SetPlayerState(playerid, PlAYER_STATE::IDLE);
			MsgSendQueue::Instance().SendPlayerReadyCancelAck(playerid);
		}
	}
	else if (msg.type == XTankMsg::GAME_READY_ACK) {

		const XTankMsg::GameReadyAck& ack = static_cast<XTankMsg::GameReadyAck&>(*msg.msgPtr);
		PlayerId playerid{ ack.playerid().ip(),ack.playerid().pid() };

		//¼ÆËãÑÓ³Ù
		auto player = mPlayerManager->GetPlayer(playerid);
		player->latency.StopCount();

		SetPlayerState(playerid, PlAYER_STATE::READY_START);

	}
	else if (msg.type == XTankMsg::PLAYER_EXIT_REQ) {

		const XTankMsg::PlayerExitReq& req = static_cast<XTankMsg::PlayerExitReq&>(*msg.msgPtr);
		PlayerId playerid{ req.playerid().ip(),req.playerid().pid() };
		MovePlayerFromRoomToLobby(playerid);
		MsgSendQueue::Instance().SendPlayerExitAck(playerid);

	}
	else if (msg.type == XTankMsg::PLAYER_CUT_IN_REQ) {

		const XTankMsg::PlayerCutInReq& req = static_cast<XTankMsg::PlayerCutInReq&>(*msg.msgPtr);
		PlayerId playerid{ req.playerid().ip(),req.playerid().pid() };
		SetPlayerState(playerid, PlAYER_STATE::CUT_IN);

	}
	else if (msg.type == XTankMsg::PLAYER_CHASE_UP_NTF) {

		const XTankMsg::PlayerChaseUpNtf& ntf = static_cast<XTankMsg::PlayerChaseUpNtf&>(*msg.msgPtr);
		PlayerId playerid{ ntf.playerid().ip(),ntf.playerid().pid() };
		SetPlayerState(playerid, PlAYER_STATE::CHASE_UP);

	}
	
}


void DataManager::Tick()
{
	
	while (1) {
		MessageData msg = MsgRecvQueue::Instance().GetAndPopTopMsg();

		ParseMsg(msg);

		mRoomManager->Tick();
	}

}
