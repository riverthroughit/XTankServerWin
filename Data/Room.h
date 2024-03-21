#pragma once
#include "TypeConfig.h"
#include <vector>
#include <array>
#include <memory>
#include "Protobuf/XTankMsg.pb.h"
#include "Player.h"
#include "Config.h"
#include "Util/TickUtil.h"
#include <memory>
#include <map>

class Room {

	//���������
	std::array<std::shared_ptr<Player>, MAX_ROOM_PLAYER_NUM> mPlayers{};

	//����ʱ���ڵ����
	std::vector<std::shared_ptr<Player>> initPlayers;

	//ÿһ֡������ҵ�ָ�� {֡id����Ӧָ������}
	std::map<int, std::array<BUTTON::Type, MAX_ROOM_PLAYER_NUM>> playersCmdMap{};

	ROOM_STATE::Type roomState{};

	//���ӵ�ָ�� ��������׷֡ ����˳�
	std::array<BUTTON::Type, MAX_ROOM_PLAYER_NUM> additionPlayersCmd;

	TickUtil outGameTick;
	TickUtil gamingTick;


private:

	int GetPlayerIndex(std::shared_ptr<Player> player);

	void SendRoomDataNtf(std::vector<std::shared_ptr<Player>>& players);

	//������������Ҷ�׼���� ������Ϸ׼������ �����ӳټ���
	void SendGameReadyReq(const std::vector<std::shared_ptr<Player>>& players);

	void SendGameInitNtf(const std::vector<std::shared_ptr<Player>>& players);

	void SendGameStartNtf(std::vector<std::shared_ptr<Player>>& players);

	void SendGameForwardNtf(std::vector<std::shared_ptr<Player>>& players);

	//��;����㲥 ���ͼ���������Ϣ
	void SendPlayerCutInNtf(std::vector<std::shared_ptr<Player>>& players);

	//��Ϸ��ʼ��ʼ��
	void GameStartInit();

	//�ж��Ƿ��������;����
	void JudgePlayerCutIn();

	//�ж�����Ƿ�׷֡���
	void JudgePlayerChaseUp();

	//����׷֡��ɵ���ҵ�����
	void AddAdditionCmdToPlayersCmd();

	//��ȡ������ָ��״̬�����
	std::vector<std::shared_ptr<Player>> GetPlayersInState(PlAYER_STATE::Type state);

	std::vector<std::shared_ptr<Player>> GetPlayersNotInState(PlAYER_STATE::Type state);

public:

	void Init();

	void RemovePlayer(std::shared_ptr<Player> player);

	void AddPlayer(std::shared_ptr<Player> player);

	void AddPlayerGameInput(std::shared_ptr<Player> player,int frameId, BUTTON::Type btnType);

	int GetPlayersNum();

	bool IsInGame();

	bool JudgeIsAllReady();

	bool IsStart();

	bool JudgeCanStart();

	bool JudgeCanStartCutIn();

	void Tick();

	void GamingTick();

	void OutGameTick();
};