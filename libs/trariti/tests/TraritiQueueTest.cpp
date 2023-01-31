#include <Trariti.h>
#include <cassert>
int main() {
    trariti::Trariti* tr = trariti::create(5);
    assert(trariti::is_empty(tr) == true);
    assert(trariti::count_entries(tr) ==  0);
    assert(trariti::count_free_entries(tr) == 5);

    uint64_t data = 419;
    auto res = trariti::enqueue<trariti::SyncType::SINGLE_THREAD, trariti::Behavior::VARIABLE>(tr,
                    reinterpret_cast<void*>(&data));

    assert(res == true);
    assert(trariti::is_empty(tr) == false);
    assert(trariti::count_entries(tr) == 1);
    assert(trariti::count_free_entries(tr) == 4);

    uint64_t data_2 = 813;
    res = trariti::enqueue<trariti::SyncType::SINGLE_THREAD, trariti::Behavior::VARIABLE>(tr,
                    reinterpret_cast<void*>(&data_2));
    void* read_data = 0;
    res = trariti::dequeue<trariti::SyncType::SINGLE_THREAD, trariti::Behavior::VARIABLE>(tr,
                    read_data);

    assert(*(uint64_t*)read_data == data);
    
    res = trariti::dequeue<trariti::SyncType::SINGLE_THREAD, trariti::Behavior::VARIABLE>(tr,
                    read_data);

    assert(*(uint64_t*)read_data == data_2);

    trariti::free(tr);
}
