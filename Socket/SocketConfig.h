#pragma once

////������ip�Ͷ˿�
//const char* const SERVER_IP = "";
constexpr unsigned int SOCKET_PORT = 6666;

//��ͷ��С���洢��Ϣ��Ĵ�С(������Ϣ������id)��
constexpr int MSG_HEAD_SIZE = 4;
//��Ϣ��id��С(ö������ 4�ֽ�)
constexpr int MSG_TYPE_SIZE = 4;
//�ܹ�ͷ����С
constexpr int MSG_TOTAL_HEAD_SIZE = MSG_HEAD_SIZE + MSG_TYPE_SIZE;
//���պͷ��ͻ����С
constexpr int BUFFER_SIZE = 102400;
//��Ϸ�ⷢ����Ϣ���� ms
constexpr int OUT_GAME_MSG_TICK = 1000;
//ģ����Ϸ���ӳ�
constexpr int SIMULATE_PING = 300;