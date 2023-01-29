#pragma once

#include <TraritiCore.h>
#include <TraritiImpl.h>

namespace trariti
{
    Trariti *create(std::size_t size);

    void free(Trariti* tr);

    void reset(Trariti *tr);

    template <SyncType sync, Behavior behavior>
    static inline bool enqueue(Trariti *tr, void* obj) {
        unsigned int free_space = 0;
        return core::do_enqueue<sync, behavior>(tr, &obj, 1, &free_space);
    }

    template <SyncType sync, Behavior behavior>
    static inline bool dequeue(Trariti *tr, void* &obj) {
        unsigned int free_space = 0;
        auto ret = core::do_dequeue<sync, behavior>(tr, &obj, 1, &free_space);

        return ret;
    }

    unsigned int count_entries(const Trariti *tr);

    std::size_t count_free_entries(const Trariti *tr);

    bool is_full(const Trariti *tr);

    bool is_empty(const Trariti *tr);
}

