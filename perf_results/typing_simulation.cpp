#include <iostream>
#include <chrono>
#include <vector>
#include <string>
#include <thread>

// Simulate typing Vietnamese text
class VietnameseTypingSimulator {
private:
    std::vector<std::string> commonWords = {
        "chào", "bạn", "phải", "không", "nào", "có", "muốn", "đi", "về", "nhà",
        "học", "tập", "làm", "việc", "ăn", "uống", "ngủ", "nghỉ", "chơi", "game"
    };

public:
    double simulateTyping(int wordCount) {
        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < wordCount; ++i) {
            std::string word = commonWords[i % commonWords.size()];

            // Simulate key press processing (microseconds)
            std::this_thread::sleep_for(std::chrono::microseconds(10));

            // Simulate Vietnamese character conversion
            for (char c : word) {
                // Process character (very fast operation)
                volatile char processed = c;
            }
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        return static_cast<double>(duration.count()) / wordCount;
    }
};

int main() {
    VietnameseTypingSimulator simulator;

    // Test different typing scenarios
    std::vector<int> testSizes = {100, 500, 1000, 5000};

    for (int size : testSizes) {
        double avgTime = simulator.simulateTyping(size);
        std::cout << "Typing " << size << " words: " << avgTime << " μs/word" << std::endl;
        std::cout << "Equivalent WPM: " << (60000000.0 / (avgTime * 5)) << std::endl;
    }

    return 0;
}
