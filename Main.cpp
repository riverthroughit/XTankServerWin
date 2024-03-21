
#include "Data/DataManager.h"
#include "Socket/SocketServer.h"
#include <thread>

int main() {

	DataManager::Instance().Init();
	std::thread dataThread(&DataManager::Tick, &DataManager::Instance());

	SocketServer::Instance().InitServer();
	std::thread ltnThread(&SocketServer::ServerListenerThread, &SocketServer::Instance());
	std::thread commuThread(&SocketServer::ServerCommunicaThread, &SocketServer::Instance());

	dataThread.join();
	ltnThread.join();
	commuThread.join();

	//XTankMsg::LoginReq req;
	//req.mutable_playerid()->set_ip("123435.12345");
	//req.mutable_playerid()->set_pid(666);

	//int size = req.ByteSizeLong();

	//char arr[1024]{0};

	//req.SerializeToArray(arr, size);

	//XTankMsg::LoginReq req2;
	//req2.ParseFromArray(arr, size);

}