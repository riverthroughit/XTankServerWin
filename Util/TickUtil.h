#pragma once

#include <chrono>

class TickUtil {

	using clock = std::chrono::high_resolution_clock;
	using duration = std::chrono::duration<float, std::milli>;
	
	//����
	float tickTime{};
	//��ǰ֡id
	int frameId{ -1 };
	//��ǰ֡�ڵ�ʱ��
	float frameTime{};
	//��ǰ֡����
	float percent{};
	//�Ƿ񾭹�һ���߼�֡����
	bool isNeedTick{};

	clock::time_point preTime{};
	clock::time_point curTime{};

	bool isStart{};

public:
	TickUtil() {
		Reset();
	}

	void Reset() {
		frameTime = tickTime;
		frameId = -1;
		percent = 0;
		isNeedTick = false;
		isStart = false;

	}

	void SetTickTime(float val) {
		tickTime = val;
		Reset();
	}


	void Tick() {

		if (!isStart) {
			isStart = true;
			preTime = clock::now();
		}

		curTime = clock::now();
		duration dtn = std::chrono::duration_cast<duration>(curTime - preTime);
		preTime = curTime;
		frameTime += dtn.count();

		if (frameTime > tickTime) {
			int dframe = frameTime / tickTime;
			//assert(dframe == 1 && "too slow");
			frameTime -= dframe * tickTime;
			frameId += dframe;
			isNeedTick = true;
		}
		else {
			isNeedTick = false;
		}

		percent = frameTime / tickTime;
	}

	bool IsNeedTick() {
		return isNeedTick;
	}

	//����һ���ӳ�
	void SetDelay(float val) {
		frameTime -= val;
	}

	int GetFrameId() {
		return frameId;
	}

};