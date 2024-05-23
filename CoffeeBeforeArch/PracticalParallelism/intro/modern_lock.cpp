#include <iostream>
#include <mutex>
#include <thread>

std::mutex mtx;

void call_from(int tid) {
    std::lock_guard<std::mutex> lock(mtx);
    std::cout << "Hello from thread - " << tid << std::endl;
}

int main() {
    std::thread t[10];

    for (int i = 0; i < 10; ++i) {
        t[i] = std::thread(call_from, i);
    }

    call_from(10);

    for (int i = 0; i < 10; ++i) {
        t[i].join();
    }
}