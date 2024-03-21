#include "MessageQueue.h"

void MsgSendQueue::SendLoginAck(const PlayerId& playerId, bool success, const std::string& msg)
{
	XTankMsg::LoginAck ack;
	ack.set_success(success);
	ack.set_msg(msg);
	PushMsg({ playerId, MessageData{XTankMsg::LOGIN_ACK, std::make_shared<XTankMsg::LoginAck>(ack)} });
}

void MsgSendQueue::SendLogoutAck(const PlayerId& playerId, bool success, const std::string& msg)
{
	XTankMsg::LogoutAck ack;
	ack.set_success(success);
	ack.set_msg(msg);
	PushMsg({ playerId, MessageData{XTankMsg::LOGOUT_ACK, std::make_shared<XTankMsg::LogoutAck>(ack)} });
}

void MsgSendQueue::SendRoomCreateAck(const PlayerId& playerId, bool success, const std::string& msg)
{
	XTankMsg::RoomCreateAck ack;
	ack.set_success(success);
	ack.set_msg(msg);
	PushMsg({ playerId, MessageData{XTankMsg::ROOM_CREATE_ACK, std::make_shared<XTankMsg::RoomCreateAck>(ack)} });
}

void MsgSendQueue::SendRoomJoinAck(const PlayerId& playerId, bool success, const std::string& msg)
{
	XTankMsg::RoomJoinAck ack;
	ack.set_success(success);
	ack.set_msg(msg);
	PushMsg({ playerId, MessageData{XTankMsg::ROOM_JOIN_ACK, std::make_shared<XTankMsg::RoomJoinAck>(ack)} });
}

void MsgSendQueue::SendRoomExitAck(const PlayerId& playerId, bool success, const std::string& msg)
{
	XTankMsg::RoomExitAck ack;
	ack.set_success(success);
	ack.set_msg(msg);
	PushMsg({ playerId, MessageData{XTankMsg::ROOM_EXIT_ACK, std::make_shared<XTankMsg::RoomExitAck>(ack)} });
}

void MsgSendQueue::SendPlayerReadyAck(const PlayerId& playerId, bool success, const std::string& msg)
{
	XTankMsg::PlayerReadyAck ack;
	ack.set_success(success);
	ack.set_msg(msg);
	PushMsg({ playerId, MessageData{XTankMsg::PLAYER_READY_ACK, std::make_shared<XTankMsg::PlayerReadyAck>(ack)} });
}

void MsgSendQueue::SendPlayerReadyCancelAck(const PlayerId& playerId, bool success, const std::string& msg)
{
	XTankMsg::PlayerReadyCancelAck ack;
	ack.set_success(success);
	ack.set_msg(msg);
	PushMsg({ playerId, MessageData{XTankMsg::PLAYER_READY_CANCEL_ACK, std::make_shared<XTankMsg::PlayerReadyCancelAck>(ack)} });
}

void MsgSendQueue::SendGameReadyReq(const PlayerId& playerId, bool success)
{
	XTankMsg::GameReadyReq req;
	req.set_success(success);
	PushMsg({ playerId, MessageData{XTankMsg::GAME_READY_REQ, std::make_shared<XTankMsg::GameReadyReq>(req)} });
}

void MsgSendQueue::SendPlayerExitAck(const PlayerId& playerId, bool success, const std::string& msg)
{
	XTankMsg::PlayerExitAck ack;
	ack.set_success(success);
	ack.set_msg(msg);
	PushMsg({ playerId, MessageData{XTankMsg::PLAYER_EXIT_ACK, std::make_shared<XTankMsg::PlayerExitAck>(ack)} });
}