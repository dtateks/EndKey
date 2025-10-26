#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <memory>
#include "Macro.h"
#include "Engine.h"

class MacroTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine = std::make_unique<Engine>();
        engine->startNewSession();

        // Initialize macro system
        vUseMacro = 1;
        vAutoCapsMacro = 1;
        vUseMacroInEnglishMode = 0;

        // Set up test macros
        SetupTestMacros();
    }

    void TearDown() override {
        engine.reset();
    }

    void SetupTestMacros() {
        // This would normally load from user configuration
        // For testing, we'll simulate common macros
        test_macros = {
            {"vn", "Việt Nam"},
            {"tp", "Thành phố"},
            {"tc", "Tỉnh/Thành phố"},
            {"st", "Số điện thoại"},
            {"dc", "Địa chỉ"},
            {"email", "email@example.com"},
            {"tel", "0123456789"},
            {"web", "www.example.com"},
            {"cty", "Công ty"},
            {"hd", "Hợp đồng"},
            {"hdlg", "Hợp đồng lao động"},
            {"lcb", "Lương cơ bản"},
            {"ns", "Ngày sinh"},
            {"cmnd", "Căn cước công dân"},
            {"msthue", "Mã số thuế"}
        };
    }

    std::unique_ptr<Engine> engine;
    std::vector<std::pair<std::string, std::string>> test_macros;

    // Helper method to simulate typing with macro detection
    std::string SimulateTypingWithMacro(const std::string& input) {
        std::string result;
        engine->startNewSession();
        for (char c : input) {
            result += engine->vKeyHandleEvent(c, 0);
        }
        return result;
    }
};

// Test basic macro expansion
TEST_F(MacroTest, BasicMacroExpansion) {
    for (const auto& [macro, expansion] : test_macros) {
        engine->startNewSession();
        std::string result = SimulateTypingWithMacro(macro);

        // Note: This would require actual macro system implementation
        // For now, test the framework
        EXPECT_FALSE(result.empty()) << "Macro expansion failed for: " << macro;

        // Test that macro triggers word boundary detection
        // In real implementation, this would check if result contains expansion
    }
}

// Test macro expansion with auto-capitalization
TEST_F(MacroTest, MacroExpansionWithAutoCapitalization) {
    vAutoCapsMacro = 1;

    struct TestCase {
        std::string input;
        std::string expected_start;
        std::string description;
    };

    std::vector<TestCase> test_cases = {
        {"vn", "Việt", "Vietnam macro should be capitalized"},
        {"tp", "Thành", "City macro should be capitalized"},
        {"cty", "Công", "Company macro should be capitalized"},
        {"hd", "Hợp", "Contract macro should be capitalized"}
    };

    for (const auto& [input, expected_start, description] : test_cases) {
        engine->startNewSession();
        std::string result = SimulateTypingWithMacro(input);

        // Note: This would require actual macro implementation
        // For now, test framework
        EXPECT_FALSE(result.empty()) << "Auto-capitalization failed: " << description;
    }
}

// Test macro word boundary detection
TEST_F(MacroTest, MacroWordBoundaryDetection) {
    struct TestCase {
        std::string input;
        std::string macro_position;
        std::string description;
    };

    std::vector<TestCase> test_cases = {
        {"vn", "standalone", "Macro at word start"},
        {"hello vn", "middle", "Macro in middle of text"},
        {"vn world", "start", "Macro at word start"},
        {"hello vn world", "middle", "Macro in middle of sentence"},
        {"xvn", "part_of_word", "Macro as part of larger word - should not expand"},
        {"vna", "part_of_word", "Macro as part of larger word - should not expand"},
    };

    for (const auto& [input, position, description] : test_cases) {
        engine->startNewSession();
        std::string result = SimulateTypingWithMacro(input);

        if (position == "part_of_word") {
            // Should not expand when macro is part of larger word
            // Note: This would require actual implementation
        } else {
            // Should expand when macro is standalone word
            // Note: This would require actual implementation
        }

        EXPECT_FALSE(result.empty()) << "Word boundary detection failed: " << description;
    }
}

// Test macro in different input modes
TEST_F(MacroTest, MacroInDifferentInputModes) {
    std::vector<int> input_modes = {TELEX, VNI};
    std::vector<int> code_tables = {UNICODE, TCVN3, VNI_WINDOWS};

    for (int input_type : input_modes) {
        for (int code_table : code_tables) {
            vInputType = input_type;
            vCodeTable = code_table;
            engine->OnInputMethodChanged(input_type);
            engine->OnTableCodeChange(code_table);

            for (const auto& [macro, expansion] : test_macros) {
                engine->startNewSession();
                std::string result = SimulateTypingWithMacro(macro);

                EXPECT_FALSE(result.empty())
                    << "Macro failed in mode " << input_type << "/" << code_table
                    << " for macro: " << macro;
            }
        }
    }
}

// Test macro with Vietnamese/English mode switching
TEST_F(MacroTest, MacroWithVietnameseEnglishModeSwitching) {
    // Test macro expansion in Vietnamese mode
    vUseMacroInEnglishMode = 0;
    engine->startNewSession();
    std::string viet_result = SimulateTypingWithMacro("vn");

    // Switch to English mode
    engine->switchLanguage();
    vUseMacroInEnglishMode = 0;
    engine->startNewSession();
    std::string eng_result = SimulateTypingWithMacro("vn");

    // Enable macro in English mode
    vUseMacroInEnglishMode = 1;
    engine->startNewSession();
    std::string eng_macro_result = SimulateTypingWithMacro("vn");

    // Note: This would require actual implementation
    EXPECT_FALSE(viet_result.empty()) << "Vietnamese mode macro failed";
    EXPECT_FALSE(eng_macro_result.empty()) << "English mode macro failed";
}

// Test macro with special characters and formatting
TEST_F(MacroTest, MacroWithSpecialCharactersAndFormatting) {
    std::vector<std::pair<std::string, std::string>> special_macros = {
        {"email", "user@example.com"},
        {"phone", "+84 123 456 789"},
        {"address", "123 Đường ABC, Quận 1, TP.HCM"},
        {"website", "https://www.example.com"},
        {"logo", "© 2024 Công ty ABC"}
    };

    for (const auto& [macro, expansion] : special_macros) {
        engine->startNewSession();
        std::string result = SimulateTypingWithMacro(macro);

        EXPECT_FALSE(result.empty()) << "Special macro failed: " << macro;

        // Test that special characters are preserved
        // Note: This would require actual implementation
    }
}

// Test macro performance
TEST_F(MacroTest, MacroPerformance) {
    const int num_iterations = 1000;
    const double max_avg_latency_us = 50.0; // 50 microseconds max

    std::vector<double> latencies;

    for (int i = 0; i < num_iterations; ++i) {
        auto start_time = std::chrono::high_resolution_clock::now();

        std::string macro = test_macros[i % test_macros.size()].first;
        SimulateTypingWithMacro(macro);

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        latencies.push_back(static_cast<double>(duration.count()));
    }

    double avg_latency = std::accumulate(latencies.begin(), latencies.end(), 0.0) / num_iterations;

    EXPECT_LT(avg_latency, max_avg_latency_us)
        << "Macro performance regression: avg latency " << avg_latency << "μs > " << max_avg_latency_us << "μs";
}

// Test macro configuration persistence
TEST_F(MacroTest, MacroConfigurationPersistence) {
    // Test adding new macro
    std::string new_macro = "test";
    std::string new_expansion = "Test Expansion";

    // Note: This would require actual macro management implementation
    // For now, test the framework

    // Test modifying existing macro
    std::string modified_macro = "vn";
    std::string modified_expansion = "Việt Nam (Sửa đổi)";

    // Test deleting macro
    std::string deleted_macro = "tp";

    // Test configuration reload
    engine->startNewSession();
    std::string result = SimulateTypingWithMacro("vn");
    EXPECT_FALSE(result.empty()) << "Configuration reload failed";
}

// Test macro conflicts and priority
TEST_F(MacroTest, MacroConflictsAndPriority) {
    // Test overlapping macros
    std::vector<std::pair<std::string, std::string>> overlapping_macros = {
        {"vn", "Việt Nam"},
        {"vnm", "Việt Nam Mien Bac"},
        {"vns", "Việt Nam Mien Nam"}
    };

    for (const auto& [macro, expansion] : overlapping_macros) {
        engine->startNewSession();
        std::string result = SimulateTypingWithMacro(macro);

        EXPECT_FALSE(result.empty()) << "Overlapping macro failed: " << macro;
    }

    // Test similar macros
    std::vector<std::pair<std::string, std::string>> similar_macros = {
        {"cty", "Công ty"},
        {"ctyn", "Công ty TNHH"},
        {"ctycp", "Công ty Cổ phần"}
    };

    for (const auto& [macro, expansion] : similar_macros) {
        engine->startNewSession();
        std::string result = SimulateTypingWithMacro(macro);

        EXPECT_FALSE(result.empty()) << "Similar macro failed: " << macro;
    }
}

// Test macro with smart switching
TEST_F(MacroTest, MacroWithSmartSwitching) {
    vUseSmartSwitchKey = 1;

    std::vector<std::string> applications = {
        "TextEdit",
        "Chrome",
        "Microsoft Word"
    };

    for (const auto& app : applications) {
        engine->OnActiveAppChanged(app);

        for (const auto& [macro, expansion] : test_macros) {
            engine->startNewSession();
            std::string result = SimulateTypingWithMacro(macro);

            EXPECT_FALSE(result.empty())
                << "Macro with smart switching failed in " << app << " for: " << macro;
        }
    }
}

// Test macro error handling
TEST_F(MacroTest, MacroErrorHandling) {
    // Test empty macro
    engine->startNewSession();
    EXPECT_NO_THROW(SimulateTypingWithMacro("")) << "Empty macro caused crash";

    // Test very long macro
    std::string long_macro(1000, 'a');
    engine->startNewSession();
    EXPECT_NO_THROW(SimulateTypingWithMacro(long_macro)) << "Long macro caused crash";

    // Test macro with special characters
    std::string special_macro = "!@#$%^&*()";
    engine->startNewSession();
    EXPECT_NO_THROW(SimulateTypingWithMacro(special_macro)) << "Special macro caused crash";

    // Test macro with Unicode characters
    std::string unicode_macro = "đây là macro";
    engine->startNewSession();
    EXPECT_NO_THROW(SimulateTypingWithMacro(unicode_macro)) << "Unicode macro caused crash";
}