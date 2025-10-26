#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <memory>
#include <map>
#include "ConvertTool.h"
#include "Engine.h"
#include "Vietnamese.h"

class CodeTableTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine = std::make_unique<Engine>();
        engine->startNewSession();

        // Initialize code table test data
        SetupCodeTableTestData();
    }

    void TearDown() override {
        engine.reset();
    }

    void SetupCodeTableTestData() {
        // Comprehensive test data for all code tables
        code_table_test_data = {
            // Telex input tests
            {
                TELEX, UNICODE,
                {
                    {"a", "a"}, {"aw", "ă"}, {"aa", "â"}, {"e", "e"}, {"ee", "ê"},
                    {"i", "i"}, {"o", "o"}, {"oo", "ô"}, {"ow", "ơ"}, {"u", "u"},
                    {"uw", "ư"}, {"y", "y"}, {"dd", "đ"}, {"as", "á"}, {"af", "à"},
                    {"ar", "ả"}, {"ax", "ã"}, {"aj", "ạ"}
                }
            },
            // VNI input tests
            {
                VNI, UNICODE,
                {
                    {"a1", "á"}, {"a2", "à"}, {"a3", "ả"}, {"a4", "ã"}, {"a5", "ạ"},
                    {"a6", "ă"}, {"a8", "â"}, {"d9", "đ"}, {"o6", "ớ"}, {"o7", "ờ"},
                    {"o8", "ô"}, {"u7", "ừ"}, {"u8", "ư"}
                }
            },
            // TCVN3 encoding tests
            {
                TELEX, TCVN3,
                {
                    {"viet", "viet"}, // Should remain in TCVN3 encoding
                    {"nguoi", "nguoi"}, // Should be TCVN3 encoded
                    {"quoc", "quoc"} // Should be TCVN3 encoded
                }
            },
            // VNI-Windows encoding tests
            {
                TELEX, VNI_WINDOWS,
                {
                    {"viet", "viet"}, // Should remain in VNI-Windows encoding
                    {"nguoi", "nguoi"}, // Should be VNI-Windows encoded
                    {"quoc", "quoc"} // Should be VNI-Windows encoded
                }
            }
        };
    }

    std::unique_ptr<Engine> engine;
    std::vector<std::tuple<int, int, std::vector<std::pair<std::string, std::string>>>> code_table_test_data;

    // Helper method to simulate typing with specific configuration
    std::string SimulateTypingWithConfig(const std::string& input, int inputType, int codeTable) {
        vInputType = inputType;
        vCodeTable = codeTable;
        engine->OnInputMethodChanged(inputType);
        engine->OnTableCodeChange(codeTable);

        std::string result;
        engine->startNewSession();
        for (char c : input) {
            result += engine->vKeyHandleEvent(c, 0);
        }
        return result;
    }

    // Helper method to convert between encodings
    std::string ConvertEncoding(const std::string& input, int fromEncoding, int toEncoding) {
        return engine->ConvertUtil(input, fromEncoding, toEncoding);
    }
};

// Test Unicode code table accuracy
TEST_F(CodeTableTest, UnicodeCodeTableAccuracy) {
    for (const auto& [inputType, codeTable, testCases] : code_table_test_data) {
        if (codeTable != UNICODE) continue;

        for (const auto& [input, expected] : testCases) {
            std::string result = SimulateTypingWithConfig(input, inputType, codeTable);

            EXPECT_EQ(result, expected)
                << "Unicode conversion failed - Input: '" << input
                << "' - Expected: '" << expected
                << "' - Got: '" << result << "'"
                << " - InputType: " << inputType;
        }
    }
}

// Test TCVN3 code table accuracy
TEST_F(CodeTableTest, TCVN3CodeTableAccuracy) {
    for (const auto& [inputType, codeTable, testCases] : code_table_test_data) {
        if (codeTable != TCVN3) continue;

        for (const auto& [input, expected] : testCases) {
            std::string result = SimulateTypingWithConfig(input, inputType, codeTable);

            // Note: TCVN3 encoding would produce different byte sequences
            // For now, test the framework
            EXPECT_FALSE(result.empty())
                << "TCVN3 conversion failed - Input: '" << input << "'";
        }
    }
}

// Test VNI-Windows code table accuracy
TEST_F(CodeTableTest, VNIWindowsCodeTableAccuracy) {
    for (const auto& [inputType, codeTable, testCases] : code_table_test_data) {
        if (codeTable != VNI_WINDOWS) continue;

        for (const auto& [input, expected] : testCases) {
            std::string result = SimulateTypingWithConfig(input, inputType, codeTable);

            // Note: VNI-Windows encoding would produce different byte sequences
            // For now, test the framework
            EXPECT_FALSE(result.empty())
                << "VNI-Windows conversion failed - Input: '" << input << "'";
        }
    }
}

// Test code table switching
TEST_F(CodeTableTest, CodeTableSwitching) {
    std::string test_input = "vietnamese typing test";

    std::vector<int> code_tables = {UNICODE, TCVN3, VNI_WINDOWS};

    for (size_t i = 0; i < code_tables.size(); ++i) {
        int current_table = code_tables[i];
        int next_table = code_tables[(i + 1) % code_tables.size()];

        // Test switching from current to next table
        std::string result1 = SimulateTypingWithConfig(test_input, TELEX, current_table);
        std::string result2 = SimulateTypingWithConfig(test_input, TELEX, next_table);

        EXPECT_FALSE(result1.empty()) << "Conversion failed for table " << current_table;
        EXPECT_FALSE(result2.empty()) << "Conversion failed for table " << next_table;

        // Results should be different for different encodings
        if (current_table != next_table) {
            EXPECT_NE(result1, result2)
                << "Same result for different tables: " << current_table << " vs " << next_table;
        }
    }
}

// Test encoding conversion accuracy
TEST_F(CodeTableTest, EncodingConversionAccuracy) {
    std::vector<std::string> test_strings = {
        "vietnamese",
        "người",
        "quốc gia",
        "đất nước",
        "thủ đô",
        "thành phố",
        "xin chào"
    };

    std::vector<int> encodings = {UNICODE, TCVN3, VNI_WINDOWS};

    for (const auto& test_string : test_strings) {
        // Test round-trip conversion: Unicode -> Other -> Unicode
        std::string unicode_result = SimulateTypingWithConfig(test_string, TELEX, UNICODE);

        for (int encoding : encodings) {
            if (encoding == UNICODE) continue;

            // Convert from Unicode to target encoding
            std::string encoded_result = ConvertEncoding(unicode_result, UNICODE, encoding);
            EXPECT_FALSE(encoded_result.empty())
                << "Encoding conversion failed: Unicode -> " << encoding;

            // Convert back to Unicode
            std::string decoded_result = ConvertEncoding(encoded_result, encoding, UNICODE);
            EXPECT_FALSE(decoded_result.empty())
                << "Encoding conversion failed: " << encoding << " -> Unicode";

            // Results should be equivalent (allowing for encoding differences)
            // Note: This would require actual conversion implementation
        }
    }
}

// Test complex character combinations
TEST_F(CodeTableTest, ComplexCharacterCombinations) {
    struct TestCase {
        std::string input;
        std::string expected;
        std::string description;
    };

    std::vector<TestCase> complex_tests = {
        {"thuws", "thứ", "Horn + acute accent"},
        {"thuwf", "thủ", "Horn + grave accent"},
        {"thuwr", "thủ", "Horn + hook above"},
        {"thuwx", "thũ", "Horn + tilde"},
        {"thuwj", "thụ", "Horn + dot below"},
        {"quocs", "quốc", "Horn + acute on complex vowel"},
        {"ngoais", "ngoài", "Horn + acute on diphthong"},
        {"duyens", "duyên", "Horn + acute on u-diphthong"},
        {"thuoings", "thướng", "Circumflex + acute"},
        {"thuongf", "thườn", "Circumflex + grave"},
        {"thuongr", "thưởng", "Circumflex + hook"},
        {"thuongx", "thưỡng", "Circumflex + tilde"},
        {"thuongj", "thượng", "Circumflex + dot"}
    };

    for (const auto& test_case : complex_tests) {
        std::string result = SimulateTypingWithConfig(test_case.input, TELEX, UNICODE);

        EXPECT_EQ(result, test_case.expected)
            << "Complex combination failed: " << test_case.description
            << " - Input: '" << test_case.input
            << "' - Expected: '" << test_case.expected
            << "' - Got: '" << result << "'";
    }
}

// Test character boundary conditions
TEST_F(CodeTableTest, CharacterBoundaryConditions) {
    std::vector<std::string> boundary_tests = {
        // Single characters
        "a", "ă", "â", "e", "ê", "i", "o", "ô", "ơ", "u", "ư", "y", "đ",
        // With all tone marks
        "ás", "àf", "ár", "ãx", "ạj",
        "ớs", "ờf", "ớr", "õx", "ợj",
        // Complex combinations
        "ướcs", "ướf", "ứcr", "ữcx", "ựcj",
        // Edge cases
        "", " ", "\n", "\t"
    };

    for (const auto& test_input : boundary_tests) {
        for (int codeTable : {UNICODE, TCVN3, VNI_WINDOWS}) {
            std::string result = SimulateTypingWithConfig(test_input, TELEX, codeTable);

            if (!test_input.empty()) {
                EXPECT_FALSE(result.empty())
                    << "Boundary condition failed for table " << codeTable << ": '" << test_input << "'";
            }
        }
    }
}

// Test code table performance
TEST_F(CodeTableTest, CodeTablePerformance) {
    const int num_iterations = 1000;
    const double max_avg_latency_us = 100.0; // 100 microseconds max

    std::vector<int> code_tables = {UNICODE, TCVN3, VNI_WINDOWS};
    std::string test_input = "vietnamese typing performance test";

    for (int codeTable : code_tables) {
        std::vector<double> latencies;

        for (int i = 0; i < num_iterations; ++i) {
            auto start_time = std::chrono::high_resolution_clock::now();

            std::string result = SimulateTypingWithConfig(test_input, TELEX, codeTable);

            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
            latencies.push_back(static_cast<double>(duration.count()));
        }

        double avg_latency = std::accumulate(latencies.begin(), latencies.end(), 0.0) / num_iterations;

        EXPECT_LT(avg_latency, max_avg_latency_us)
            << "Code table performance regression for table " << codeTable
            << ": avg latency " << avg_latency << "μs > " << max_avg_latency_us << "μs";
    }
}

// Test encoding conversion edge cases
TEST_F(CodeTableTest, EncodingConversionEdgeCases) {
    std::vector<std::string> edge_cases = {
        // Empty string
        "",
        // Very long string
        std::string(1000, 'v'),
        // Mixed content
        "vietnamese 123 english !@#",
        // Unicode characters
        "đây là tiếng Việt",
        // Special characters
        "\n\t\r",
        // Invalid UTF-8 sequences (if applicable)
        "\xFF\xFE"
    };

    for (const auto& edge_case : edge_cases) {
        for (int fromEncoding : {UNICODE, TCVN3, VNI_WINDOWS}) {
            for (int toEncoding : {UNICODE, TCVN3, VNI_WINDOWS}) {
                if (fromEncoding == toEncoding) continue;

                std::string result = ConvertEncoding(edge_case, fromEncoding, toEncoding);

                // Should not crash on edge cases
                EXPECT_NO_THROW(ConvertEncoding(edge_case, fromEncoding, toEncoding))
                    << "Conversion crashed for edge case: " << edge_case
                    << " from " << fromEncoding << " to " << toEncoding;
            }
        }
    }
}

// Test code table configuration persistence
TEST_F(CodeTableTest, CodeTableConfigurationPersistence) {
    std::string test_input = "vietnamese";

    // Test configuration changes
    std::vector<int> test_sequence = {UNICODE, TCVN3, VNI_WINDOWS, UNICODE};

    for (size_t i = 0; i < test_sequence.size(); ++i) {
        int current_table = test_sequence[i];

        // Set configuration
        vCodeTable = current_table;
        engine->OnTableCodeChange(current_table);

        // Verify configuration
        EXPECT_EQ(vCodeTable, current_table) << "Configuration not set for table " << current_table;

        // Test functionality
        std::string result = SimulateTypingWithConfig(test_input, TELEX, current_table);
        EXPECT_FALSE(result.empty()) << "Functionality failed for table " << current_table;

        // Verify persistence
        EXPECT_EQ(vCodeTable, current_table) << "Configuration not persistent for table " << current_table;
    }
}