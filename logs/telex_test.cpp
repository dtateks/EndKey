#include <iostream>
#include <string>
#include <map>

// Simulate Telex input conversion
std::string simulateTelex(const std::string& input) {
    static std::map<std::string, std::string> telexMap = {
        {"aw", "ă"}, {"aa", "â"}, {"ee", "ê"}, {"oo", "ô"}, {"ow", "ơ"},
        {"dd", "đ"}, {"w", "ư"}, {"u", "ư"},
        {"s", "s"}, {"f", "f"}, {"r", "r"}, {"x", "x"}, {"j", "j"}
    };

    // Simple simulation for testing
    std::string result = input;
    for (const auto& pair : telexMap) {
        size_t pos = 0;
        while ((pos = result.find(pair.first, pos)) != std::string::npos) {
            result.replace(pos, pair.first.length(), pair.second);
            pos += pair.second.length();
        }
    }
    return result;
}

int main() {
    // Test basic Telex conversions
    std::string tests[] = {"chao", "xin chao", "tot qua", "phong vien"};

    for (const auto& test : tests) {
        std::string result = simulateTelex(test);
        std::cout << "Input: " << test << " -> Output: " << result << std::endl;
    }

    std::cout << "Telex simulation completed successfully" << std::endl;
    return 0;
}
