#pragma once
#include <string>
#include <unordered_map>

struct PlayerId {
    std::string ip{};
    unsigned long long pid{};
};

template <>
struct std::hash<PlayerId> {
    size_t operator()(const PlayerId& key) const noexcept {
        return std::hash<std::string>()(key.ip) + key.pid;
    }
};

inline bool operator == (const PlayerId& p1, const PlayerId& p2) {
    return p1.ip == p2.ip && p1.pid == p2.pid;
}

inline bool operator < (const PlayerId& p1, const PlayerId& p2) {
    return p1.ip == p2.ip ? p1.pid < p2.pid : p1.ip < p2.ip;
}
