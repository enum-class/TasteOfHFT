#include <BroadcastQueue.hpp>

#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

struct Message {
	double data = 0.;
	char name[15] = "\0";
};

static constexpr std::size_t CONSUMER_COUNT = 2;

std::mutex g_i_mutex;

void print(std::string in) {
	const std::lock_guard<std::mutex> lock(g_i_mutex);
	std::cout << in;
}

int main() {
	broadcast::Header<Message, 16> queue;

	auto prod = std::jthread([&]() {
		Message msg;
		msg.data = 0.1;
		while (true) {
			msg.data *= 2;
			std::memcpy(msg.name, std::to_string(msg.data).c_str(), 10);

			print("Prod: " + std::to_string(msg.data) + "\t" + msg.name + "\t" +
				std::to_string(queue.counter) + "\n");
            broadcast::write(queue, msg);
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
		}
	});
    
    std::vector<std::jthread> consumers;
    for (int i = 0; i < CONSUMER_COUNT; i++) {
        consumers.push_back(std::jthread([&](int id) {
		    std::this_thread::sleep_for(std::chrono::milliseconds(60));
		    int64_t index = queue.counter.load(std::memory_order_relaxed);
		    if (index != 0)
		    	index--;
		    while (true) {
                Message msg;
                if (broadcast::read(queue, msg, index)) {
                    print("Consumer " + std::to_string(id) + "\t" +
                                        std::to_string(msg.data) + "\t" +
                                        msg.name + "\t" +
                                        std::to_string(index) + "\n");
                    index++;
                    std::this_thread::sleep_for(std::chrono::milliseconds(10 * (id + 1)));
                }
		    }
	    }, i));
    }
}
