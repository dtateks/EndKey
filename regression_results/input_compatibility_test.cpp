#include <iostream>
#include <string>
#include <vector>
#include <map>

// Legacy input method simulation
class LegacyInputMethodTest {
private:
    std::map<std::string, std::string> legacyTelex;
    std::map<std::string, std::string> legacyVNI;

public:
    LegacyInputMethodTest() {
        // Legacy Telex mappings (must be preserved)
        legacyTelex["aw"] = "ă";
        legacyTelex["aa"] = "â";
        legacyTelex["ee"] = "ê";
        legacyTelex["oo"] = "ô";
        legacyTelex["ow"] = "ơ";
        legacyTelex["uw"] = "ư";
        legacyTelex["dd"] = "đ";

        // Legacy VNI mappings (must be preserved)
        legacyVNI["a8"] = "ă";
        legacyVNI["a6"] = "â";
        legacyVNI["e6"] = "ê";
        legacyVNI["o6"] = "ô";
        legacyVNI["o7"] = "ơ";
        legacyVNI["u7"] = "ư";
        legacyVNI["d9"] = "đ";
    }

    bool testTelexCompatibility() {
        std::vector<std::pair<std::string, std::string>> testCases = {
            {"aw", "ă"}, {"aa", "â"}, {"ee", "ê"}, {"oo", "ô"},
            {"ow", "ơ"}, {"uw", "ư"}, {"dd", "đ"}
        };

        for (const auto& testCase : testCases) {
            std::string result = processLegacyTelex(testCase.first);
            if (result != testCase.second) {
                std::cout << "Telex compatibility failed: " << testCase.first
                         << " -> " << result << " (expected: " << testCase.second << ")" << std::endl;
                return false;
            }
        }
        return true;
    }

    bool testVNICompatibility() {
        std::vector<std::pair<std::string, std::string>> testCases = {
            {"a8", "ă"}, {"a6", "â"}, {"e6", "ê"}, {"o6", "ô"},
            {"o7", "ơ"}, {"u7", "ư"}, {"d9", "đ"}
        };

        for (const auto& testCase : testCases) {
            std::string result = processLegacyVNI(testCase.first);
            if (result != testCase.second) {
                std::cout << "VNI compatibility failed: " << testCase.first
                         << " -> " << result << " (expected: " << testCase.second << ")" << std::endl;
                return false;
            }
        }
        return true;
    }

private:
    std::string processLegacyTelex(const std::string& input) {
        auto it = legacyTelex.find(input);
        return (it != legacyTelex.end()) ? it->second : input;
    }

    std::string processLegacyVNI(const std::string& input) {
        auto it = legacyVNI.find(input);
        return (it != legacyVNI.end()) ? it->second : input;
    }
};

int main() {
    LegacyInputMethodTest test;

    std::cout << "Testing legacy input method compatibility..." << std::endl;

    bool telexResult = test.testTelexCompatibility();
    std::cout << "Telex compatibility: " << (telexResult ? "PASS" : "FAIL") << std::endl;

    bool vniResult = test.testVNICompatibility();
    std::cout << "VNI compatibility: " << (vniResult ? "PASS" : "FAIL") << std::endl;

    if (telexResult && vniResult) {
        std::cout << "All legacy input methods compatible" << std::endl;
        return 0;
    } else {
        std::cout << "Legacy input method compatibility issues detected" << std::endl;
        return 1;
    }
}
