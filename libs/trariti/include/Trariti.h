#pragma once

#include <TraritiCore.h>
#include <TraritiImpl.h>

#include <cstring>

namespace {
std::size_t calculate_memsize_element(unsigned int size, unsigned int count) {
  std::size_t sz;
  if (size % 4 == 0) {
    // TODO: Error
    return 0;
  }

  // TODO: check power of 2 and not exceed maximum size
  sz = sizeof(trariti::Trariti) + count * size;
  sz = ALIGN(sz, trariti::CACHE_LINE_SIZE);
  return sz;
}

uint32_t combine32ms1b(uint32_t x) {
  x |= x >> 1;
  x |= x >> 2;
  x |= x >> 4;
  x |= x >> 8;
  x |= x >> 16;

  return x;
}

uint32_t align32pow2(uint32_t x) {
  x--;
  x = combine32ms1b(x);

  return x + 1;
}
}  // namespace

namespace trariti
{
static inline Trariti *create(std::size_t size) {
  trariti::Trariti *tr = nullptr;
  const unsigned int req_count = size;

  size = align32pow2(size + 1);
  std::size_t real_size = calculate_memsize_element(sizeof(uint8_t), size);
  if (size == 0) return nullptr;

  uint8_t *mz = new uint8_t[real_size];
  tr = reinterpret_cast<trariti::Trariti *>(mz);
  std::memset(tr, 0, sizeof(*tr));
  tr->size = size;
  tr->mask = tr->size - 1;
  tr->capacity = req_count;
  tr->memzone = mz;

  return tr;
}

static inline void free(Trariti *tr) { delete[] tr->memzone; }

static inline void reset(Trariti *tr) {
  tr->consumer.head = 0;
  tr->consumer.tail = 0;
  tr->producer.head = 0;
  tr->producer.tail = 0;
}

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

    static inline unsigned int count_entries(const Trariti *tr) {
      uint32_t prod_tail = tr->producer.tail;
      uint32_t cons_tail = tr->consumer.tail;
      uint32_t count = (prod_tail - cons_tail) & tr->mask;
      return (count > tr->capacity) ? tr->capacity : count;
    }

    static inline std::size_t count_free_entries(const Trariti *tr) {
      return tr->capacity - trariti::count_entries(tr);
    }

    static inline bool is_full(const Trariti *tr) {
      return trariti::count_free_entries(tr) == 0;
    }

    static inline bool is_empty(const Trariti *tr) {
      uint32_t prod_tail = tr->producer.tail;
      uint32_t cons_tail = tr->consumer.tail;
      return cons_tail == prod_tail;
    }
    }  // namespace trariti
