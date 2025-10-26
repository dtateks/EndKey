#include <iostream>
#include <chrono>
#include <vector>
#include <string>
#include <memory>

int main() {
    const int iterations = 10000;
    std::vector<std::unique_ptr<std::string>> strings;

    auto start = std::chrono::high_resolution_clock::now();

    // Simulate memory allocation patterns in EndKey
    for (int i = 0; i < iterations; ++i) {
        auto str = std::make_unique<std::string>("tiếng việt " + std::to_string(i));
        strings.push_back(std::move(str));
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    double avgTime = static_cast<double>(duration.count()) / iterations;
    std::cout << "Memory allocation: " << avgTime << " μs per allocation" << std::endl;
    std::cout << "Total allocations: " << iterations << std::endl;
    std::cout << "Memory usage: " << (strings.size() * 20) << " bytes (estimated)" << std::endl;

    return 0;
}
