#pragma once


#define SINGLETON(ClassType) \
private: \
	ClassType() = default; \
	ClassType(const ClassType&) = delete; \
	ClassType& operator = (const ClassType&) = delete; \
public: \
	static ClassType& Instance() { \
		static ClassType instance; \
		return instance; \
	} \
	