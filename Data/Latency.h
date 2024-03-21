#pragma once
#include <chrono>


class Latency {

	using clock = std::chrono::high_resolution_clock;
	using duration = std::chrono::duration<float, std::milli>;

	//当前已收集的延迟数据数量
	int latencyNum{};

	//延迟数据之和
	float latencySum{};

	//最近的一次延迟数据
	float curLatency{};

	clock::time_point preTime{};
	clock::time_point curTime{};

	//添加延迟数据
	void AddLatency(float lat) {
		curLatency = lat;
		++latencyNum;
		latencySum += curLatency;
	}

public:
	float GetCurLatency() {
		return curLatency;
	}

	//获取平均延迟
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