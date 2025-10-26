#include <iostream>
#include <vector>
#include <chrono>
#include <string>

// Simulate EndKey memory usage patterns
int main() {
    const int iterations = 10000;
    std::vector<std::string> vietnamese_words;

    // Simulate Vietnamese character processing
    for (int i = 0; i < iterations; ++i) {
        vietnamese_words.push_back("tiếng việt ");
        vietnamese_words.push_back("chào buổi sáng ");
        vietnamese_words.push_back("phát triển phần mềm ");
    }

    std::cout << "Memory test completed with " << vietnamese_words.size() << " strings" << std::endl;
    return 0;
}
