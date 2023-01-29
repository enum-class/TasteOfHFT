#include <gtest/gtest.h>
#include <Trariti.h>
#include <iostream>

TEST(LibTests, welcome) {
    trariti::Trariti* tr = trariti::create(5);
    EXPECT_EQ(trariti::is_empty(tr), true);
    EXPECT_EQ(trariti::count_entries(tr), 0);
    EXPECT_EQ(trariti::count_free_entries(tr), 5);

    uint64_t data = 419;
    auto res = trariti::enqueue<trariti::SyncType::SINGLE_THREAD, trariti::Behavior::VARIABLE>(tr,
                    reinterpret_cast<void*>(&data));

    EXPECT_EQ(res, true);
    EXPECT_EQ(trariti::is_empty(tr), false);
    EXPECT_EQ(trariti::count_entries(tr), 1);
    EXPECT_EQ(trariti::count_free_entries(tr), 4);

    uint64_t data_2 = 813;
    res = trariti::enqueue<trariti::SyncType::SINGLE_THREAD, trariti::Behavior::VARIABLE>(tr,
                    reinterpret_cast<void*>(&data_2));
    void* read_data = 0;
    res = trariti::dequeue<trariti::SyncType::SINGLE_THREAD, trariti::Behavior::VARIABLE>(tr,
                    read_data);

    // EXPECT_EQ(res, true);
    // EXPECT_EQ(trariti::is_empty(tr), true);
    // EXPECT_EQ(trariti::count_entries(tr), 0);
    // EXPECT_EQ(trariti::count_free_entries(tr), 5);

    EXPECT_EQ(*(uint64_t*)read_data, data);
    
    res = trariti::dequeue<trariti::SyncType::SINGLE_THREAD, trariti::Behavior::VARIABLE>(tr,
                    read_data);

    EXPECT_EQ(*(uint64_t*)read_data, data_2);

    trariti::free(tr);
}
