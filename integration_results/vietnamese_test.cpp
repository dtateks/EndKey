#include <iostream>
#include <string>
#include <vector>
#include <map>

// Vietnamese character mapping simulation
class VietnameseProcessor {
private:
    std::map<std::string, std::string> telexMap;
    std::map<std::string, std::string> vniMap;

public:
    VietnameseProcessor() {
        // Initialize Telex mappings
        telexMap["aw"] = "ă";
        telexMap["aa"] = "â";
        telexMap["ee"] = "ê";
        telexMap["oo"] = "ô";
        telexMap["ow"] = "ơ";
        telexMap["uw"] = "ư";
        telexMap["dd"] = "đ";
        telexMap["s"] = "s"; // tone mark
        telexMap["f"] = "f"; // tone mark
        telexMap["r"] = "r"; // tone mark
        telexMap["x"] = "x"; // tone mark
        telexMap["j"] = "j"; // tone mark

        // Initialize VNI mappings
        vniMap["a8"] = "ă";
        vniMap["a6"] = "â";
        vniMap["e6"] = "ê";
        vniMap["o6"] = "ô";
        vniMap["o7"] = "ơ";
        vniMap["u7"] = "ư";
        vniMap["d9"] = "đ";
    }

    std::string processTelex(const std::string& input) {
        std::string result = input;

        // Process Telex input
        for (const auto& pair : telexMap) {
            size_t pos = result.find(pair.first);
            if (pos != std::string::npos) {
                result.replace(pos, pair.first.length(), pair.second);
            }
        }

        return result;
    }

    std::string processVNI(const std::string& input) {
        std::string result = input;

        // Process VNI input
        for (const auto& pair : vniMap) {
            size_t pos = result.find(pair.first);
            if (pos != std::string::npos) {
                result.replace(pos, pair.first.length(), pair.second);
            }
        }

        return result;
    }
};

int main() {
    VietnameseProcessor processor;

    // Test Telex processing
    std::vector<std::pair<std::string, std::string>> telexTests = {
        {"aw", "ă"}, {"aa", "â"}, {"dd", "đ"}, {"hello", "hello"}
    };

    std::cout << "Testing Telex integration:" << std::endl;
    for (const auto& test : telexTests) {
        std::string result = processor.processTelex(test.first);
        std::cout << "  " << test.first << " -> " << result;
        if (result == test.second) {
            std::cout << " ✓" << std::endl;
        } else {
            std::cout << " ✗ (expected: " << test.second << ")" << std::endl;
        }
    }

    // Test VNI processing
    std::vector<std::pair<std::string, std::string>> vniTests = {
        {"a8", "ă"}, {"a6", "â"}, {"d9", "đ"}, {"hello", "hello"}
    };

    std::cout << "Testing VNI integration:" << std::endl;
    for (const auto& test : vniTests) {
        std::string result = processor.processVNI(test.first);
        std::cout << "  " << test.first << " -> " << result;
        if (result == test.second) {
            std::cout << " ✓" << std::endl;
        } else {
            std::cout << " ✗ (expected: " << test.second << ")" << std::endl;
        }
    }

    std::cout << "Vietnamese input integration test completed" << std::endl;
    return 0;
}
