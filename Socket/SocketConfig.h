#pragma once

////服务器ip和端口
//const char* const SERVER_IP = "";
constexpr unsigned int SOCKET_PORT = 6666;

//包头大小（存储消息体的大小(不含消息体类型id)）
constexpr int MSG_HEAD_SIZE = 4;
//消息体id大小(枚举类型 4字节)
constexpr int MSG_TYPE_SIZE = 4;
//总共头部大小
constexpr int MSG_TOTAL_HEAD_SIZE = MSG_HEAD_SIZE + MSG_TYPE_SIZE;
//接收和发送缓冲大小
constexpr int BUFFER_SIZE = 102400;
//游戏外发送消息周期 ms
constexpr int OUT_GAME_MSG_TICK = 1000;
//模拟游戏内延迟
constexpr int SIMULATE_PING = 300;