#pragma once

#include "TypeConfig.h"
#include "PlayerId.h"
#include "Latency.h"

struct Player {
	PlayerId id{};
	PlAYER_STATE::Type state{};
	int roomId{-1};
	Latency latency{};
};