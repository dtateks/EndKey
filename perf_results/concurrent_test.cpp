#include <iostream>
#include <chrono>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>

std::atomic<int> counter{0};
std::mutex output_mutex;

void simulateConcurrentTyping(int threadId, int operations) {
    for (int i = 0; i < operations; ++i) {
        // Simulate concurrent Vietnamese input processing
        std::string input = "thread" + std::to_string(threadId) + "word" + std::to_string(i);

        // Process input (simulate Vietnamese conversion)
        for (char c : input) {
            if (c >= 'a' && c <= 'z') {
                // Simulate Vietnamese character conversion
                volatile char result = c;
            }
        }

        counter++;

        // Small delay to simulate real processing
        std::this_thread::sleep_for(std::chrono::microseconds(1));
    }
}

int main() {
    const int totalOperations = 10000;
    const int threadCount = std::thread::hardware_concurrency();

    std::cout << "Running concurrent test with " << threadCount << " threads" << std::endl;
    std::cout << "Total operations: " << totalOperations << std::endl;

    auto start = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> threads;
    int operationsPerThread = totalOperations / threadCount;

    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back(simulateConcurrentTyping, i, operationsPerThread);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Completed " << counter.load() << " operations" << std::endl;
    std::cout << "Total time: " << duration.count() << " ms" << std::endl;
    std::cout << "Throughput: " << (counter.load() * 1000.0 / duration.count()) << " ops/sec" << std::endl;
    std::cout << "Average latency: " << (duration.count() * 1000.0 / counter.load()) << " μs/op" << std::endl;

    return 0;
}
