#pragma once

#include <TraritiCore.h>

#include <atomic>

namespace
{
    template <trariti::SyncType sync, trariti::Behavior behavior>
    unsigned int move_consumer_head(trariti::Trariti *tr, unsigned int n,
            uint32_t *old_head, uint32_t *new_head, uint32_t *entries) {
        unsigned int max = n;
        uint32_t prod_tail = 0;
        int success = 0;

        /* move cons.head atomically */
        *old_head = tr->consumer.head.load(std::memory_order_relaxed);
        do {
            /* Restore n as it may change every loop */
            n = max;

            /* Ensure the head is read before tail */
            std::atomic_thread_fence(std::memory_order_acquire);

            /* this load-acquire synchronize with store-release of ht->tail
             * in update_tail.
             */
            prod_tail = tr->producer.tail.load(std::memory_order_acquire);

            /* The subtraction is done between two unsigned 32bits value
             * (the result is always modulo 32 bits even if we have
             * cons_head > prod_tail). So 'entries' is always between 0
             * and size(ring)-1.
             */
            *entries = (prod_tail - *old_head);

            /* Set the actual entries for dequeue */
            if (n > *entries) {
                if constexpr (behavior == trariti::Behavior::FIXED) 
                    return 0;
                else {
                    n = *entries;
                    if (n == 0) [[unlikely]]
                        return 0;
                }
            }

            *new_head = *old_head + n;
            if constexpr (sync == trariti::SyncType::SINGLE_THREAD) {
                tr->consumer.head = *new_head;
                return n;
            }
            else {
                /* on failure, *old_head will be updated */
                success = atomic_compare_exchange_strong_explicit(
                    &tr->consumer.head, old_head, *new_head,
                    std::memory_order_relaxed, std::memory_order_relaxed);
            }
        } while (success == 0); // [[unlikely]]
        return n;
    }

    void dequeue_memory(trariti::Trariti *tr, uint32_t cons_head, void *obj_table, uint32_t n)
    {
        unsigned int i = 0;
        const uint32_t size = tr->size;
        uint32_t idx = cons_head & tr->mask;
        uint64_t *ring = (uint64_t *)&tr[1];
        uint64_t *obj = (uint64_t *)obj_table;
        if (idx + n <= size) [[likely]] {
            for (i = 0; i < (n & ~0x3); i += 4, idx += 4) {
                obj[i] = ring[idx];
                obj[i + 1] = ring[idx + 1];
                obj[i + 2] = ring[idx + 2];
                obj[i + 3] = ring[idx + 3];
            }
            switch (n & 0x3) {
                case 3: [[fallthrough]];
                        obj[i++] = ring[idx++];
                case 2: [[fallthrough]];
                        obj[i++] = ring[idx++];
                case 1: [[fallthrough]];
                        obj[i++] = ring[idx++];
            }
        } else {
            for (i = 0; idx < size; i++, idx++)
                obj[i] = ring[idx];
            /* Start at the beginning */
            for (idx = 0; i < n; i++, idx++)
                obj[i] = ring[idx];
        }
    }

    template <trariti::SyncType sync, trariti::Behavior behavior>
    unsigned int move_producer_head(trariti::Trariti *tr, unsigned int n, uint32_t *old_head, uint32_t *new_head,
            uint32_t *free_entries) {
        const uint32_t capacity = tr->capacity;
        uint32_t cons_tail = 0;
        unsigned int max = n;
        int success = 0;

        *old_head = tr->producer.head.load(std::memory_order_relaxed);
        do {
            n = max;
            std::atomic_thread_fence(std::memory_order_acquire);
            cons_tail = tr->consumer.tail.load(std::memory_order_acquire);

            *free_entries = (capacity + cons_tail - *old_head);

            if (n > *free_entries){
                if constexpr (behavior == trariti::Behavior::FIXED) 
                    return 0;
                else {
                    n = *free_entries;
                    if (n == 0)
                        return 0;
                }
            }

            *new_head = *old_head + n;
            if constexpr (sync == trariti::SyncType::SINGLE_THREAD) {
                tr->producer.head = *new_head;
                return n;
            } else {
              success = atomic_compare_exchange_strong_explicit(
                  &tr->producer.head, old_head, *new_head,
                  std::memory_order_relaxed, std::memory_order_relaxed);
            }
        } while (success == 0); // [[unlikely]]
        return n;
    }

    template <trariti::SyncType sync>
    void update_tail(trariti::HeadTail* ht, uint32_t old_val, uint32_t new_val) {
        if constexpr (sync == trariti::SyncType::MULTI_THREAD) {
            while (ht->tail.load(std::memory_order_relaxed) != old_val
                //_mm_pause();
            }
        }

        ht->tail.store(new_val, std::memory_order_release);
    }

    void enqueue_memory(trariti::Trariti *tr, uint32_t prod_head, const void *obj_table, uint32_t n) {
        unsigned int i = 0;
        const uint32_t size = tr->size;
        uint32_t idx = prod_head & tr->mask;
        uint64_t *ring = (uint64_t *)&tr[1];
        const uint64_t *obj = (const uint64_t *)obj_table;

        if (idx + n <= size) [[likely]] {
            for (i = 0; i < (n & ~0x3); i += 4, idx += 4) {
                ring[idx] = obj[i];
                ring[idx + 1] = obj[i + 1];
                ring[idx + 2] = obj[i + 2];
                ring[idx + 3] = obj[i + 3];
            }
            switch (n & 0x3) {
                case 3:
                    ring[idx++] = obj[i++];
                case 2:
                    ring[idx++] = obj[i++];
                case 1:
                    ring[idx++] = obj[i++];
            }
        } else {
            for (i = 0; idx < size; i++, idx++)
                ring[idx] = obj[i];
            for (idx = 0; i < n; i++, idx++)
                ring[idx] = obj[i];
        }
    }
}

namespace core
{
    template <trariti::SyncType sync, trariti::Behavior behavior>
    bool do_enqueue(trariti::Trariti* tr, const void *obj,
                unsigned int n, unsigned int *free_space)
    {
    uint32_t prod_head = 0, prod_next = 0;
    uint32_t free_entries = 0;

    n = move_producer_head<sync, behavior>(tr, n, &prod_head, &prod_next, &free_entries);
    if (n == 0) {
        *free_space = free_entries - n;
        return false;
    }

    enqueue_memory(tr, prod_head, obj, n);
    update_tail<sync>(&tr->producer, prod_head, prod_next);

    *free_space = free_entries - n;
    return true;
    }


    template <trariti::SyncType sync, trariti::Behavior behavior>
    bool do_dequeue(trariti::Trariti* tr, void *obj,
                unsigned int n, unsigned int *available)
{
    uint32_t cons_head = 0, cons_next = 0;
    uint32_t entries = 0;

    n = move_consumer_head<sync, behavior>(tr, n, &cons_head, &cons_next, &entries);
    if (n == 0) {
        *available = entries - n;
        return false;
    }

    dequeue_memory(tr, cons_head, obj, n);
    update_tail<sync>(&tr->consumer, cons_head, cons_next);

    *available = entries - n;
    return true;
}

}
