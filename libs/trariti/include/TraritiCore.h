#pragma once

#include <cstdint>

namespace trariti
{
    #define ALIGN_FLOOR(val, align) \
        (typeof(val))((val) & (~((typeof(val))((align) - 1))))

    #define ALIGN_CEIL(val, align) \
        ALIGN_FLOOR(((val) + ((typeof(val)) (align) - 1)), align)

    #define ALIGN(val, align) ALIGN_CEIL(val, align)

    static constexpr std::size_t CACHE_LINE_SIZE = 64; 
    static constexpr std::size_t CACHE_LINE_MASK = CACHE_LINE_SIZE - 1;

    enum class Behavior {
        FIXED,
        VARIABLE
    };

    enum class SyncType {
        MULTI_THREAD,
        SINGLE_THREAD,
    };

    struct HeadTail {
        volatile uint32_t head = 0;
        volatile uint32_t tail = 0;
    };

    struct Trariti
    {
        const uint8_t *memzone = nullptr;
        uint32_t size = 0;
        uint32_t mask = 0;
        uint32_t capacity = 0;

        HeadTail producer;
        HeadTail consumer;
    };
}
