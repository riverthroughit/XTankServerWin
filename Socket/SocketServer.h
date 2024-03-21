#pragma once
#include <vector>
#include "SocketConfig.h"
#include "Protobuf/XTankMsg.pb.h"
#include <map>
#include "Data/PlayerId.h"
#include <set>
#include "Util/Macro.h"
#include <shared_mutex>
#include <mutex>

class SocketServer {

	SINGLETON(SocketServer);

private:

	using Message = google::protobuf::Message;

	//连接socket
	unsigned long long listenSocket{};

	//已确定玩家id的socket
	std::map<PlayerId,unsigned long long> socketMap;

	//管理tempSocketMap的锁
	std::shared_mutex socketSetMutex;
	//所有的客户端socket
	std::set<unsigned long long> tempSocketSet;

	//接受的消息字节数组
	char* msgRcvData{ new char[BUFFER_SIZE] {0} };

	//发送的消息字节数组
	char* msgSendData{ new char[BUFFER_SIZE] {0} };


	std::shared_ptr<google::protobuf::Message> ParseMsg(int msgId, int dataSize);

public:

	~SocketServer() {
		delete[] msgRcvData;
		delete[] msgSendData;
	}

	//初始化socket连接
	bool InitServer();

	//服务器监听线程
	void ServerListenerThread();

	//服务器通信线程
	void ServerCommunicaThread();

	//遍历tempSocketSet接受消息
	void ReceiveFromTempClient();

	//遍历socketMap接受消息
	void ReceiveFromPlayerClient();

	//从队列中取出一条发给对应客户端
	bool SendToClient();

	//注意需要用非阻塞socket
	std::pair<XTankMsg::MSG, int> ReceiveFromSocket(unsigned long long socket);

	//添加客户端socket到socketSet
	void AddSocketToTempSet(unsigned long long socket);

	//对端socket关闭
	void ClosePlayerSocket(const PlayerId& playerId, unsigned long long socket);

	std::pair<unsigned long long,bool> GetSocketByPlayerId(const PlayerId& playerId);

};