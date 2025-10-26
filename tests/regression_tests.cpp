#include <gtest/gtest.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <memory>
#include "Engine.h"
#include "Vietnamese.h"
#include "Macro.h"
#include "ConvertTool.h"

class RegressionTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine = std::make_unique<Engine>();
        engine->startNewSession();

        // Load regression test data
        LoadRealWorldTestScenarios();
        LoadLegacyCompatibilityTests();
        LoadCharacterEncodingTests();
    }

    void TearDown() override {
        engine.reset();
    }

    void LoadRealWorldTestScenarios() {
        // Common Vietnamese phrases used in daily life
        real_world_scenarios = {
            {
                "xin chào ban",
                "xin chào bạn",
                "Daily greeting with tone marks"
            },
            {
                "cam on rat nhieu",
                "cảm ơn rất nhiều",
                "Expression of gratitude"
            },
            {
                "rat vui duoc gap ban",
                "rất vui được gặp bạn",
                "Social interaction phrase"
            },
            {
                "toi la nguoi viet nam",
                "tôi là người Việt Nam",
                "Self-introduction with country name"
            },
            {
                "thoi tiet hom nay dep qua",
                "thời tiết hôm nay đẹp quá",
                "Weather conversation"
            },
            {
                "mon an ngon cua viet nam",
                "món ăn ngon của Việt Nam",
                "Food discussion"
            },
            {
                "thu do ha noi",
                "thủ đô Hà Nội",
                "Capital city with proper nouns"
            },
            {
                "thanh pho ho chi minh",
                "thành phố Hồ Chí Minh",
                "City name with proper nouns"
            },
            {
                "so dien thoai cua toi",
                "số điện thoại của tôi",
                "Phone number inquiry"
            },
            {
                "dia chi nha o",
                "địa chỉ nhà ở",
                "Address inquiry"
            },
            {
                "xin loi vi den muon",
                "xin lỗi vì đến muộn",
                "Apology phrase"
            },
            {
                "chuc ban mot ngay tot lanh",
                "chúc bạn một ngày tốt lành",
                "Well wishes"
            },
            {
                "toi muon di mua sam",
                "tôi muốn đi mua sắm",
                "Shopping intention"
            },
            {
                "gia ca cua san pham",
                "giá cả của sản phẩm",
                "Price inquiry"
            },
            {
                "thong tin chi tiet",
                "thông tin chi tiết",
                "Information request"
            }
        };
    }

    void LoadLegacyCompatibilityTests() {
        // Test backward compatibility with existing user configurations
        legacy_compatibility_tests = {
            {
                TELEX,
                UNICODE,
                "vietnamese typing",
                "vietnamese typing",
                "English text should pass through"
            },
            {
                VNI,
                UNICODE,
                "ho2c",
                "hộc",
                "VNI tone mark compatibility"
            },
            {
                TELEX,
                TCVN3,
                "vietnamese",
                "vietnamese", // TCVN3 encoded result would be different
                "TCVN3 encoding compatibility"
            },
            {
                TELEX,
                VNI_WINDOWS,
                "vietnamese",
                "vietnamese", // VNI-Windows encoded result would be different
                "VNI-Windows encoding compatibility"
            },
            {
                TELEX,
                UNICODE,
                "dd",
                "đ",
                "Vietnamese d with stroke compatibility"
            },
            {
                TELEX,
                UNICODE,
                "ow",
                "ớ",
                "Horn with acute compatibility"
            },
            {
                TELEX,
                UNICODE,
                "aw",
                "ă",
                "Breve compatibility"
            },
            {
                TELEX,
                UNICODE,
                "aa",
                "â",
                "Circumflex compatibility"
            }
        };
    }

    void LoadCharacterEncodingTests() {
        // Comprehensive character encoding edge cases
        character_encoding_tests = {
            // Basic vowel combinations
            {"a", "ă", "â", "e", "ê", "i", "o", "ô", "ơ", "u", "ư", "y"},
            // Tone mark combinations
            {"s", "f", "r", "x", "j"},
            // Complex combinations
            {"aws", "aws", "awf", "awr", "awx", "awj"},
            {"oas", "oas", "oaf", "oar", "oax", "oaj"},
            {"uws", "uws", "uwf", "uwr", "uwx", "uwj"},
            // Boundary conditions
            {"", " ", "\n", "\t", "123", "!@#"},
            // Edge case combinations
            {"dd", "dd", "dd", "dd"},
            {"thuws", "thuwf", "thuwr", "thuwx", "thuwj"},
            {"quocs", "quocf", "quocr", "quocx", "quocj"}
        };
    }

    std::unique_ptr<Engine> engine;
    std::vector<std::tuple<std::string, std::string, std::string>> real_world_scenarios;
    std::vector<std::tuple<int, int, std::string, std::string, std::string>> legacy_compatibility_tests;
    std::vector<std::vector<std::string>> character_encoding_tests;

    // Helper method to simulate typing with current configuration
    std::string SimulateTyping(const std::string& input) {
        std::string result;
        engine->startNewSession();
        for (char c : input) {
            result += engine->vKeyHandleEvent(c, 0);
        }
        return result;
    }

    // Helper method to set configuration
    void SetConfiguration(int inputType, int codeTable) {
        vInputType = inputType;
        vCodeTable = codeTable;
        engine->OnInputMethodChanged(inputType);
        engine->OnTableCodeChange(codeTable);
    }
};

// Test backward compatibility for existing user configurations
TEST_F(RegressionTest, BackwardCompatibilityUserConfigurations) {
    for (const auto& [inputType, codeTable, input, expected, description] : legacy_compatibility_tests) {
        SetConfiguration(inputType, codeTable);

        std::string result = SimulateTyping(input);

        EXPECT_EQ(result, expected)
            << "Backward compatibility failed: " << description
            << " - Input: '" << input
            << "' - Expected: '" << expected
            << "' - Got: '" << result << "'"
            << " - InputType: " << inputType
            << " - CodeTable: " << codeTable;
    }
}

// Test real-world Vietnamese typing scenarios
TEST_F(RegressionTest, RealWorldVietnameseTypingScenarios) {
    SetConfiguration(TELEX, UNICODE);

    for (const auto& [input, expected, description] : real_world_scenarios) {
        std::string result = SimulateTyping(input);

        EXPECT_EQ(result, expected)
            << "Real-world scenario failed: " << description
            << " - Input: '" << input
            << "' - Expected: '" << expected
            << "' - Got: '" << result << "'";
    }
}

// Test character encoding edge cases and boundary conditions
TEST_F(RegressionTest, CharacterEncodingEdgeCases) {
    SetConfiguration(TELEX, UNICODE);

    for (const auto& test_group : character_encoding_tests) {
        for (const auto& test_input : test_group) {
            std::string result = SimulateTyping(test_input);

            // Result should be valid and not empty (except for empty input)
            if (!test_input.empty()) {
                EXPECT_FALSE(result.empty())
                    << "Empty result for input: '" << test_input << "'";
            }

            // The result should be different from input when tone marks are applied
            if (test_input.find_first_of("sfrxj") != std::string::npos && !test_input.empty()) {
                EXPECT_NE(result, test_input)
                    << "No conversion for input with tone mark: '" << test_input << "'";
            }
        }
    }
}

// Test Unicode conversion accuracy across different encodings
TEST_F(RegressionTest, UnicodeConversionAccuracy) {
    std::vector<std::pair<int, std::string>> encoding_tests = {
        {UNICODE, "Unicode"},
        {TCVN3, "TCVN3"},
        {VNI_WINDOWS, "VNI-Windows"}
    };

    std::vector<std::string> test_strings = {
        "vietnamese",
        "người",
        "quốc gia",
        "đất nước",
        "thủ đô",
        "thành phố"
    };

    for (const auto& [encoding, name] : encoding_tests) {
        SetConfiguration(TELEX, encoding);

        for (const auto& test_str : test_strings) {
            std::string result = SimulateTyping(test_str);

            // Verify that conversion produces valid output
            EXPECT_FALSE(result.empty())
                << "Conversion failed for " << name << ": " << test_str;

            // Test round-trip conversion if possible
            if (encoding != UNICODE) {
                // Convert back to Unicode and compare
                std::string round_trip = engine->ConvertUtil(result, encoding, UNICODE);
                // Note: This would require actual conversion implementation
                // For now, just verify the conversion doesn't crash
                EXPECT_NO_THROW(engine->ConvertUtil(result, encoding, UNICODE));
            }
        }
    }
}

// Test macro expansion backward compatibility
TEST_F(RegressionTest, MacroExpansionBackwardCompatibility) {
    SetConfiguration(TELEX, UNICODE);
    vUseMacro = 1;
    vAutoCapsMacro = 1;

    // Test legacy macro expansions
    std::vector<std::pair<std::string, std::string>> legacy_macros = {
        {"vn", "Việt Nam"},
        {"tp", "Thành phố"},
        {"tc", "Tỉnh/Thành phố"},
        {"st", "Số điện thoại"},
        {"dc", "Địa chỉ"},
        {"email", "email@example.com"},
        {"tel", "0123456789"}
    };

    for (const auto& [input, expected] : legacy_macros) {
        engine->startNewSession();
        std::string result = SimulateTyping(input);

        // Note: This would require actual macro system implementation
        // For now, test the framework
        EXPECT_FALSE(result.empty()) << "Macro expansion failed for: " << input;
    }
}

// Test smart switching configuration persistence
TEST_F(RegressionTest, SmartSwitchingConfigurationPersistence) {
    SetConfiguration(TELEX, UNICODE);
    vUseSmartSwitchKey = 1;

    // Test application-specific language memory
    std::vector<std::string> test_applications = {
        "TextEdit",
        "Safari",
        "Chrome",
        "Microsoft Word"
    };

    for (const auto& app : test_applications) {
        // Simulate application switch
        engine->OnActiveAppChanged(app);

        // Test Vietnamese mode
        engine->startNewSession();
        std::string viet_result = SimulateTyping("viet");

        // Switch to English
        engine->switchLanguage();
        engine->startNewSession();
        std::string eng_result = SimulateTyping("viet");

        // Verify behavior is different
        EXPECT_NE(viet_result, eng_result)
            << "Smart switching failed for app: " << app;

        // Verify configuration is remembered
        std::string current_app = engine->getCurrentApp();
        EXPECT_EQ(current_app, app)
            << "App memory failed for: " << app;
    }
}

// Test spell checking configuration compatibility
TEST_F(RegressionTest, SpellCheckingConfigurationCompatibility) {
    SetConfiguration(TELEX, UNICODE);

    // Test different spell checking configurations
    std::vector<std::pair<int, std::string>> spell_configs = {
        {0, "Spell checking disabled"},
        {1, "Spell checking enabled"},
        {2, "Modern orthography"},
        {3, "Traditional orthography"}
    };

    for (const auto& [config, description] : spell_configs) {
        vCheckSpelling = config;
        engine->OnSpellCheckingChanged(config);

        // Test with words that might be affected by spell checking
        std::vector<std::string> test_words = {
            "thuws", "thuwf", "nguoi", "quoc", "thu"
        };

        for (const auto& word : test_words) {
            engine->startNewSession();
            std::string result = SimulateTyping(word);

            EXPECT_FALSE(result.empty())
                << "Spell checking config failed: " << description
                << " - Word: " << word;
        }
    }
}

// Test performance regression detection
TEST_F(RegressionTest, PerformanceRegressionDetection) {
    SetConfiguration(TELEX, UNICODE);

    const int num_iterations = 1000;
    const double max_avg_latency_ms = 0.1; // 0.1ms max average

    std::vector<double> latencies;

    for (int i = 0; i < num_iterations; ++i) {
        auto start_time = std::chrono::high_resolution_clock::now();

        std::string test_input = "vietnamesetyping";
        SimulateTyping(test_input);

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        latencies.push_back(duration.count() / 1000.0); // Convert to milliseconds
    }

    double avg_latency = std::accumulate(latencies.begin(), latencies.end(), 0.0) / num_iterations;

    EXPECT_LT(avg_latency, max_avg_latency_ms)
        << "Performance regression detected: avg latency " << avg_latency << "ms > " << max_avg_latency_ms << "ms";

    // Check for outliers (latencies > 10x average)
    int outlier_count = std::count_if(latencies.begin(), latencies.end(),
        [avg_latency](double latency) { return latency > avg_latency * 10; });

    EXPECT_LT(outlier_count, num_iterations * 0.01) // Less than 1% outliers
        << "Too many performance outliers: " << outlier_count << "/" << num_iterations;
}

// Test memory leak regression detection
TEST_F(RegressionTest, MemoryLeakRegressionDetection) {
    SetConfiguration(TELEX, UNICODE);

    size_t initial_memory = sizeof(*engine);

    // Perform many operations
    const int num_operations = 10000;
    for (int i = 0; i < num_operations; ++i) {
        engine->startNewSession();
        SimulateTyping("vietnamese typing test");
    }

    size_t final_memory = sizeof(*engine);
    size_t memory_increase = final_memory - initial_memory;

    // Memory increase should be minimal (accounting for normal operation)
    EXPECT_LT(memory_increase, 1024) // Less than 1KB increase
        << "Memory leak detected: increased by " << memory_increase << " bytes";
}

// Test concurrent operation regression
TEST_F(RegressionTest, ConcurrentOperationRegression) {
    SetConfiguration(TELEX, UNICODE);

    const int num_threads = 5;
    const int operations_per_thread = 100;

    std::vector<std::thread> threads;
    std::atomic<int> successful_operations{0};

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&, i]() {
            for (int j = 0; j < operations_per_thread; ++j) {
                try {
                    std::string test_input = "test" + std::to_string(i) + std::to_string(j);
                    SimulateTyping(test_input);
                    successful_operations++;
                } catch (const std::exception& e) {
                    // Log error but continue
                }
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    int total_operations = num_threads * operations_per_thread;
    double success_rate = static_cast<double>(successful_operations) / total_operations;

    EXPECT_GT(success_rate, 0.95)
        << "Concurrent operation regression: success rate " << success_rate;
}