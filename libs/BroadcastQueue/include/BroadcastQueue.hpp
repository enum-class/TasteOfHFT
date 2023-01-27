#pragma once

#include <atomic>
#include <array>
#include <cstring>

namespace broadcast
{

template<typename T>
struct Block {
	std::atomic<int8_t> version = 0;
	T data;
};

template<typename T, std::size_t size>
struct Header {
	static constexpr int64_t mask = size - 1;
	alignas(64) std::atomic<int64_t> counter = 0;
	alignas(64) std::array<Block<T>, size> blocks;
};

template<typename T, std::size_t size>
static inline void write(Header<T, size> &header, const T &data) {
	auto &block = header.blocks[header.counter.load(std::memory_order_relaxed)];

	block.version.fetch_add(1, std::memory_order_release);
    std::atomic_signal_fence(std::memory_order_acq_rel);

	std::memcpy(&block.data, &data, sizeof(T));

    std::atomic_signal_fence(std::memory_order_acq_rel);
	block.version.fetch_add(1, std::memory_order_release);

	header.counter.fetch_add(1, std::memory_order_relaxed);
	header.counter.fetch_and(header.mask, std::memory_order_relaxed);
}

template<typename T, std::size_t size>
static inline bool read(const Header<T, size> &header, T &data, int64_t &index) {
	index = index & header.mask;
	const auto& block = header.blocks[index];

	const int8_t ver = block.version.load(std::memory_order_acquire);
    std::atomic_signal_fence(std::memory_order_acq_rel);
	if (ver == 0 || (ver & 1) != 0) [[unlikely]]
		return false;

	std::memcpy(&data, &block.data, sizeof(T));

    std::atomic_signal_fence(std::memory_order_acq_rel);
	return ver == block.version.load(std::memory_order_acquire);
}
}
