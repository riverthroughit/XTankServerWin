#pragma once
#include <chrono>


class Latency {

	using clock = std::chrono::high_resolution_clock;
	using duration = std::chrono::duration<float, std::milli>;

	//��ǰ���ռ����ӳ���������
	int latencyNum{};

	//�ӳ�����֮��
	float latencySum{};

	//�����һ���ӳ�����
	float curLatency{};

	clock::time_point preTime{};
	clock::time_point curTime{};

	//����ӳ�����
	void AddLatency(float lat) {
		curLatency = lat;
		++latencyNum;
		latencySum += curLatency;
	}

public:
	float GetCurLatency() {
		return curLatency;
	}

	//��ȡƽ���ӳ�
	float GetAverageLatency() {
		return latencySum / latencyNum;
	}

	void StartCount() {
		preTime = clock::now();
	}

	void StopCount() {
		curTime = clock::now();
		duration dtn = std::chrono::duration_cast<duration>(curTime - preTime);
		AddLatency(dtn.count());
	}

};