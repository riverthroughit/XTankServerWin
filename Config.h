#pragma once

constexpr int MAX_ROOM_NUM{100};
constexpr int MAX_ROOM_PLAYER_NUM{4};


//逻辑帧刷新率
constexpr int LOCKSTEP_FPS{ 20 };
//逻辑帧周期
constexpr float LOCKSTEP_TICK{ 1.0 / LOCKSTEP_FPS * 1000 };
