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

	//����socket
	unsigned long long listenSocket{};

	//��ȷ�����id��socket
	std::map<PlayerId,unsigned long long> socketMap;

	//����tempSocketMap����
	std::shared_mutex socketSetMutex;
	//���еĿͻ���socket
	std::set<unsigned long long> tempSocketSet;

	//���ܵ���Ϣ�ֽ�����
	char* msgRcvData{ new char[BUFFER_SIZE] {0} };

	//���͵���Ϣ�ֽ�����
	char* msgSendData{ new char[BUFFER_SIZE] {0} };


	std::shared_ptr<google::protobuf::Message> ParseMsg(int msgId, int dataSize);

public:

	~SocketServer() {
		delete[] msgRcvData;
		delete[] msgSendData;
	}

	//��ʼ��socket����
	bool InitServer();

	//�����������߳�
	void ServerListenerThread();

	//������ͨ���߳�
	void ServerCommunicaThread();

	//����tempSocketSet������Ϣ
	void ReceiveFromTempClient();

	//����socketMap������Ϣ
	void ReceiveFromPlayerClient();

	//�Ӷ�����ȡ��һ��������Ӧ�ͻ���
	bool SendToClient();

	//ע����Ҫ�÷�����socket
	std::pair<XTankMsg::MSG, int> ReceiveFromSocket(unsigned long long socket);

	//��ӿͻ���socket��socketSet
	void AddSocketToTempSet(unsigned long long socket);

	//�Զ�socket�ر�
	void ClosePlayerSocket(const PlayerId& playerId, unsigned long long socket);

	std::pair<unsigned long long,bool> GetSocketByPlayerId(const PlayerId& playerId);

};