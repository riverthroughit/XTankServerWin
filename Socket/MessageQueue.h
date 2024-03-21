#pragma once

#include <deque>
#include "Util/Macro.h"
#include <memory>
#include "Protobuf/XTankMsg.pb.h"
#include <shared_mutex>
#include <mutex>
#include <utility>
#include "Data/PlayerId.h"


struct MessageData {
	XTankMsg::MSG type{};
	std::shared_ptr<google::protobuf::Message> msgPtr{};
};


class MsgRecvQueue {
	SINGLETON(MsgRecvQueue);

private:
	using Message = google::protobuf::Message;

	//存放服务器来的消息
	std::deque<MessageData> msgDeque;

	//队列锁
	std::shared_mutex dequeMutex;

public:

	void PushMsg(const MessageData& msg){
		std::unique_lock<std::shared_mutex> lock(dequeMutex);
		msgDeque.push_back(msg);
	}

	void PopMsg() {
		std::unique_lock<std::shared_mutex> lock(dequeMutex);

		if (!msgDeque.empty()) {
			msgDeque.pop_front();
		}
	}

	MessageData GetTopMsg() {
		std::shared_lock<std::shared_mutex> slock(dequeMutex);
		MessageData msg;

		if (!msgDeque.empty()) {
			msg = msgDeque.front();
		}

		return msg;
	}

	MessageData GetAndPopTopMsg() {
		std::unique_lock<std::shared_mutex> lock(dequeMutex);
		MessageData msg;

		if (!msgDeque.empty()) {
			msg = msgDeque.front();
			msgDeque.pop_front();
		}

		return msg;
	}

	void ClearMsg() {
		std::unique_lock<std::shared_mutex> lock(dequeMutex);
		msgDeque.clear();
	}
};


class MsgSendQueue {

	SINGLETON(MsgSendQueue);

private:
	using Message = google::protobuf::Message;

	using MsgPair = std::pair<PlayerId, MessageData>;

	//存放服务器来的消息
	std::deque<MsgPair> msgDeque;

	//队列锁
	std::shared_mutex dequeMutex;

public:

	void PushMsg(const MsgPair& msgData) {
		std::unique_lock<std::shared_mutex> lock(dequeMutex);
		msgDeque.push_back(msgData);
	}

	void PopMsg() {
		std::unique_lock<std::shared_mutex> lock(dequeMutex);

		if (!msgDeque.empty()) {
			msgDeque.pop_front();
		}
	}

	MsgPair GetTopMsg() {
		std::shared_lock<std::shared_mutex> slock(dequeMutex);
		MsgPair msg;

		if (!msgDeque.empty()) {
			msg = msgDeque.front();
		}

		return msg;
	}

	MsgPair GetAndPopTopMsg() {
		std::unique_lock<std::shared_mutex> lock(dequeMutex);
		MsgPair msg;

		if (!msgDeque.empty()) {
			msg = msgDeque.front();
			msgDeque.pop_front();
		}

		return msg;
	}

	void ClearMsg() {
		std::unique_lock<std::shared_mutex> lock(dequeMutex);
		msgDeque.clear();
	}

	void SendLoginAck(const PlayerId& playerId, bool success = true, const std::string& msg = "");

	void SendLogoutAck(const PlayerId& playerId, bool success = true, const std::string& msg = "");

	void SendRoomCreateAck(const PlayerId& playerId, bool success = true, const std::string& msg = "");

	void SendRoomJoinAck(const PlayerId& playerId, bool success = true, const std::string& msg = "");

	void SendRoomExitAck(const PlayerId& playerId, bool success = true, const std::string& msg = "");

	void SendPlayerReadyAck(const PlayerId& playerId, bool success = true, const std::string& msg = "");

	void SendPlayerReadyCancelAck(const PlayerId& playerId, bool success = true, const std::string& msg = "");

	void SendGameReadyReq(const PlayerId& playerId, bool success = true);

	void SendPlayerExitAck(const PlayerId& playerId, bool success = true, const std::string& msg = "");


};