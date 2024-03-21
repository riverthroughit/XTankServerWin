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
		CUT_IN,	//��;����
		EXIT,	//�˳�
		MAX,
	};
}

namespace PlAYER_STATE {
	enum Type {
		IDLE,	//�ڴ������ڷ�������δ׼��
		READY, //�ѵ��׼��
		READY_START,	//�ѷ���GAME_READY_ACK
		CUT_IN, //�ѵ������
		CHASE_UP,	//�����׷֡
		GAMING, //��Ϸ��
	};
}

namespace ROOM_STATE {
	enum Type {
		OUT_GAME,	//��Ϸ�� δ����
		WAITING_START,	//�ȴ���ҵ�GAME_READY_ACK
		GAMING,		//������Ҿ�����Ϸ��
		WAITING_CUT_IN,		//�ȴ���;������ҵ�GAME_READY_ACK
	};
}