#pragma once


namespace BUTTON {
	enum Type {
		NONE,
		UP,
		DOWN,
		LEFT,
		RIGHT,
		FIRE,
		IDLE,
		CUT_IN,	//中途加入
		EXIT,	//退出
		MAX,
	};
}

namespace PlAYER_STATE {
	enum Type {
		IDLE,	//在大厅或在房间中尚未准备
		READY, //已点击准备
		READY_START,	//已发送GAME_READY_ACK
		CUT_IN, //已点击加入
		CHASE_UP,	//已完成追帧
		GAMING, //游戏中
	};
}

namespace ROOM_STATE {
	enum Type {
		OUT_GAME,	//游戏外 未开赛
		WAITING_START,	//等待玩家的GAME_READY_ACK
		GAMING,		//所有玩家均在游戏中
		WAITING_CUT_IN,		//等待中途加入玩家的GAME_READY_ACK
	};
}