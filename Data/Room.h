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

	//房价内玩家
	std::array<std::shared_ptr<Player>, MAX_ROOM_PLAYER_NUM> mPlayers{};

	//开局时存在的玩家
	std::vector<std::shared_ptr<Player>> initPlayers;

	//每一帧各个玩家的指令 {帧id，对应指令数组}
	std::map<int, std::array<BUTTON::Type, MAX_ROOM_PLAYER_NUM>> playersCmdMap{};

	ROOM_STATE::Type roomState{};

	//附加的指令 如玩家完成追帧 玩家退出
	std::array<BUTTON::Type, MAX_ROOM_PLAYER_NUM> additionPlayersCmd;

	TickUtil outGameTick;
	TickUtil gamingTick;


private:

	int GetPlayerIndex(std::shared_ptr<Player> player);

	void SendRoomDataNtf(std::vector<std::shared_ptr<Player>>& players);

	//房间内所有玩家都准备后 发送游戏准备请求 开启延迟计算
	void SendGameReadyReq(const std::vector<std::shared_ptr<Player>>& players);

	void SendGameInitNtf(const std::vector<std::shared_ptr<Player>>& players);

	void SendGameStartNtf(std::vector<std::shared_ptr<Player>>& players);

	void SendGameForwardNtf(std::vector<std::shared_ptr<Player>>& players);

	//中途加入广播 发送加入的玩家信息
	void SendPlayerCutInNtf(std::vector<std::shared_ptr<Player>>& players);

	//游戏开始初始化
	void GameStartInit();

	//判断是否有玩家中途加入
	void JudgePlayerCutIn();

	//判断玩家是否追帧完成
	void JudgePlayerChaseUp();

	//设置追帧完成的玩家的命令
	void AddAdditionCmdToPlayersCmd();

	//获取房间内指定状态的玩家
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