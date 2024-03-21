#include "SocketServer.h"
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include "Socket/MessageQueue.h"

std::shared_ptr<google::protobuf::Message> SocketServer::ParseMsg(int msgId, int dataSize)
{
	std::shared_ptr<Message> msgPtr;

	switch (msgId)
	{
	case XTankMsg::NONE:
		msgPtr = nullptr;
		break;

	case XTankMsg::LOGIN_REQ:
		msgPtr = std::make_shared<XTankMsg::LoginReq>();
		break;

	case XTankMsg::LOGOUT_REQ:
		msgPtr = std::make_shared<XTankMsg::LogoutReq>();
		break;

	case XTankMsg::ROOM_CREATE_REQ:
		msgPtr = std::make_shared<XTankMsg::RoomCreateReq>();
		break;

	case XTankMsg::ROOM_JOIN_REQ:
		msgPtr = std::make_shared<XTankMsg::RoomJoinReq>();
		break;

	case XTankMsg::ROOM_EXIT_REQ:
		msgPtr = std::make_shared<XTankMsg::RoomExitReq>();
		break;

	case XTankMsg::GAME_READY_ACK:
		msgPtr = std::make_shared<XTankMsg::GameReadyAck>();
		break;

	case XTankMsg::PLAYER_READY_REQ:
		msgPtr = std::make_shared<XTankMsg::PlayerReadyReq>();
		break;

	case XTankMsg::PLAYER_READY_CANCEL_REQ:
		msgPtr = std::make_shared<XTankMsg::PlayerReadyCancelReq>();
		break;

	case XTankMsg::PLAYER_INPUT_NTF:
		msgPtr = std::make_shared<XTankMsg::PlayerInputNtf>();
		break;

	case XTankMsg::PLAYER_EXIT_REQ:
		msgPtr = std::make_shared<XTankMsg::PlayerExitReq>();
		break;

	case XTankMsg::PLAYER_CUT_IN_REQ:
		msgPtr = std::make_shared<XTankMsg::PlayerCutInReq>();
		break;

	case XTankMsg::PLAYER_CHASE_UP_NTF:
		msgPtr = std::make_shared<XTankMsg::PlayerChaseUpNtf>();
		break;

	default:
		break;
	}

	if (msgPtr == nullptr
		|| !msgPtr->ParseFromArray(msgRcvData + MSG_TOTAL_HEAD_SIZE, dataSize)) {
		return nullptr;
	}

	return msgPtr;
}

bool SocketServer::InitServer()
{

	//初始化WSA  
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0){
		WSACleanup();
		assert(0);
		return false;
	}

	//创建套接字  
	listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenSocket == INVALID_SOCKET){
		closesocket(listenSocket);//关闭socket
		WSACleanup();
		assert(0);
		return false;
	}

	//绑定IP和端口  
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(SOCKET_PORT);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	if (bind(listenSocket, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR){
		closesocket(listenSocket);//关闭socket
		WSACleanup();
		assert(0);
		return false;
	}

	//开始监听  
	if (listen(listenSocket, 5) == SOCKET_ERROR){
		closesocket(listenSocket);//关闭socket
		WSACleanup();
		assert(0);
		return false;
	}

	return true;
}

void SocketServer::ServerListenerThread()
{

	sockaddr_in remoteAddr;
	int nAddrlen = sizeof(remoteAddr);
	u_long nonBlocking = 1;
	
	while (1) {

		SOCKET clientSocket = accept(listenSocket, (SOCKADDR*)&remoteAddr, &nAddrlen);

		//设置为非阻塞
		ioctlsocket(clientSocket, FIONBIO, &nonBlocking);

		AddSocketToTempSet(clientSocket);
	}
}

void SocketServer::ServerCommunicaThread()
{
	while (1) {
		//
		ReceiveFromTempClient();
		ReceiveFromPlayerClient();
		//发
		SendToClient();
	}
}

void SocketServer::ReceiveFromTempClient()
{
	
	std::unique_lock lck(socketSetMutex);
	for (auto ite = tempSocketSet.begin(); ite != tempSocketSet.end();) {

		auto tempSocket = *ite;

		auto [msgType, msgSize] = ReceiveFromSocket(tempSocket);

		if (msgSize == 0) {
			closesocket(tempSocket);
			ite = tempSocketSet.erase(ite);
		}
		else if (msgSize != -1) {
			//登录
			auto msg = ParseMsg(msgType, msgSize);

			std::shared_ptr<XTankMsg::LoginReq> req = std::static_pointer_cast<XTankMsg::LoginReq>(msg);

			PlayerId playerId;
			playerId.ip = req->playerid().ip();
			playerId.pid = req->playerid().pid();

			socketMap.insert({ playerId,tempSocket });
			ite = tempSocketSet.erase(ite);

			MsgRecvQueue::Instance().PushMsg({ msgType,msg });
		}
		else {
			++ite;
		}
	}

}

void SocketServer::ReceiveFromPlayerClient()
{

	for (auto ite = socketMap.begin(); ite != socketMap.end(); ) {

		auto& [playerId, socket] = *ite;
		auto [msgType, msgSize] = ReceiveFromSocket(socket);

		if (msgSize == 0) {
			//连接断开 关闭socket并发起用户退出请求
			ClosePlayerSocket(playerId, socket);
			ite = socketMap.erase(ite);
		}
		else {
			if (msgSize != -1) {
				auto msg = ParseMsg(msgType, msgSize);
				MsgRecvQueue::Instance().PushMsg({ msgType,msg });
			}
			++ite;
		}
	}
}


bool SocketServer::SendToClient()
{

	auto [playerId,msgData] = MsgSendQueue::Instance().GetAndPopTopMsg();
	auto& [msgType, msg] = msgData;

	if (msgType == XTankMsg::NONE) {
		return true;
	}

	memset(msgSendData, 0, BUFFER_SIZE);

	int msgByteSize = msg->ByteSizeLong();

	reinterpret_cast<int&>(*msgSendData) = msgByteSize;

	reinterpret_cast<XTankMsg::MSG&>(*(msgSendData + MSG_HEAD_SIZE)) = msgType;

	if (!msg->SerializeToArray(msgSendData + MSG_TOTAL_HEAD_SIZE, msgByteSize)) {
		assert(0);
		return false;
	}

	auto [socket,isExist] = GetSocketByPlayerId(playerId);

	if (!isExist) {
		return false;
	}

	if (send(socket, msgSendData, msgByteSize + MSG_TOTAL_HEAD_SIZE, 0) == -1) {
		
		auto ite = socketMap.find(playerId);
		if (ite != socketMap.end()) {
			socketMap.erase(ite);
			ClosePlayerSocket(playerId, socket);
		}
		else {
			std::unique_lock lck(socketSetMutex);
			tempSocketSet.erase(socket);
			closesocket(socket);
		}

		return false;
	}

	return true;
}

std::pair<XTankMsg::MSG, int> SocketServer::ReceiveFromSocket(unsigned long long socket)
{
	//读取头部以及id

	int len = 0;
	while (len != MSG_TOTAL_HEAD_SIZE) {
		int curLen = 0;
		curLen = recv(socket, msgRcvData + len, MSG_TOTAL_HEAD_SIZE - len, 0);
		if (curLen == 0 || curLen == -1) {
			return { XTankMsg::NONE,curLen };
		}
		len += curLen;
	}

	int dataSize = *reinterpret_cast<int*>(msgRcvData);
	XTankMsg::MSG msgId = *reinterpret_cast<XTankMsg::MSG*>(msgRcvData + MSG_HEAD_SIZE);

	//读取消息内容

	len = 0;
	while (len != dataSize) {
		int curLen = 0;
		curLen = recv(socket, msgRcvData + MSG_TOTAL_HEAD_SIZE + len, dataSize - len, 0);
		if (curLen == 0 || curLen == -1) {
			return { XTankMsg::NONE,curLen };
		}
		len += curLen;
	}

	return { msgId,dataSize };
}

void SocketServer::AddSocketToTempSet(unsigned long long socket)
{
	std::unique_lock<std::shared_mutex> lck(socketSetMutex);
	tempSocketSet.insert(socket);
}


void SocketServer::ClosePlayerSocket(const PlayerId& playerId, unsigned long long socket)
{
	closesocket(socket);
	XTankMsg::PlayerId ppid;
	ppid.set_ip(playerId.ip);
	ppid.set_pid(playerId.pid);
	std::shared_ptr<XTankMsg::LogoutReq> logoutReq = std::make_shared<XTankMsg::LogoutReq>();
	logoutReq->mutable_playerid()->CopyFrom(ppid);
	MsgRecvQueue::Instance().PushMsg({ XTankMsg::LOGOUT_REQ,logoutReq });
}

std::pair<unsigned long long, bool> SocketServer::GetSocketByPlayerId(const PlayerId& playerId)
{
	auto ite = socketMap.find(playerId);
	
	if (ite == socketMap.end()) {
		return { 0,false };
	}

	return { ite->second,true };
}


