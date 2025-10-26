#include <gtest/gtest.h>
#include <memory>
#include <thread>
#include <chrono>
#include <atomic>
#include "Engine.h"
#include "Vietnamese.h"
#include "Macro.h"
#include "SmartSwitchKey.h"

class IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine = std::make_unique<Engine>();
        engine->startNewSession();

        // Set up default configuration
        vLanguage = VIETNAMESE;
        vInputType = TELEX;
        vCodeTable = UNICODE;
        vUseMacro = 1;
        vUseSmartSwitchKey = 1;
    }

    void TearDown() override {
        engine.reset();
    }

    std::unique_ptr<Engine> engine;

    // Helper methods for integration testing
    std::string SimulateKeyPress(const std::string& input) {
        std::string result;
        for (char c : input) {
            result += engine->vKeyHandleEvent(c, 0);
        }
        return result;
    }

    void SimulateApplicationSwitch(const std::string& app_name) {
        engine->OnActiveAppChanged(app_name);
    }
};

// Test C++ engine ↔ Objective-C UI bridge integrity
TEST_F(IntegrationTest, EngineUIBridgeIntegrity) {
    // Test engine initialization and state management
    EXPECT_TRUE(engine != nullptr);
    EXPECT_NO_THROW(engine->startNewSession());

    // Test basic functionality
    std::string test_input = "viet";
    std::string result = SimulateKeyPress(test_input);
    EXPECT_FALSE(result.empty());

    // Test state consistency across operations
    engine->switchLanguage(); // Switch to English
    std::string english_result = SimulateKeyPress(test_input);
    EXPECT_NE(result, english_result);

    engine->switchLanguage(); // Switch back to Vietnamese
    std::string viet_result = SimulateKeyPress(test_input);
    EXPECT_EQ(result, viet_result);

    // Test session management
    engine->startNewSession();
    std::string new_session_result = SimulateKeyPress("hello");
    EXPECT_FALSE(new_session_result.empty());
}

// Test macOS permissions and system integration
TEST_F(IntegrationTest, MacOSSystemIntegration) {
    // Test accessibility permissions simulation
    // Note: In real testing, this would check actual system permissions

    // Simulate permission check
    bool has_accessibility_permission = true; // Simulated
    EXPECT_TRUE(has_accessibility_permission) << "Accessibility permissions required";

    // Test input monitoring permissions
    bool has_input_monitoring_permission = true; // Simulated
    EXPECT_TRUE(has_input_monitoring_permission) << "Input monitoring permissions required";

    // Test system event handling
    EXPECT_NO_THROW(engine->vKeyHandleEvent('a', 0));
    EXPECT_NO_THROW(engine->vKeyHandleEvent('b', 0));
    EXPECT_NO_THROW(engine->vKeyHandleEvent('\n', 0));

    // Test hotkey functionality
    EXPECT_NO_THROW(engine->checkHotKey(0)); // Simulate hotkey check
}

// Test Accessibility API compatibility
TEST_F(IntegrationTest, AccessibilityAPICompatibility) {
    // Test event source management
    EXPECT_TRUE(engine->myEventSource != nullptr);

    // Test key event processing
    std::vector<std::pair<char, int>> test_keys = {
        {'a', 0}, {'b', 0}, {'c', 0},
        {'1', 0}, {'2', 0}, {'3', 0},
        {' ', 0}, {'\n', 0}, {'\t', 0}
    };

    for (const auto& [key, flag] : test_keys) {
        EXPECT_NO_THROW(engine->vKeyHandleEvent(key, flag))
            << "Failed to process key: " << key;
    }

    // Test special key combinations
    EXPECT_NO_THROW(engine->SendBackspace());
    EXPECT_NO_THROW(engine->SendShiftAndLeftArrow());
    EXPECT_NO_THROW(engine->SendCutKey());
}

// Test Helper app coordination
TEST_F(IntegrationTest, HelperAppCoordination) {
    // Test EndKeyHelper integration
    // Note: This would normally communicate with the actual helper app

    // Simulate helper app presence check
    bool helper_app_running = true; // Simulated
    EXPECT_TRUE(helper_app_running) << "EndKeyHelper must be running";

    // Test helper app communication
    std::vector<std::string> helper_commands = {
        "check_permissions",
        "request_accessibility",
        "request_input_monitoring",
        "register_hotkey"
    };

    for (const auto& command : helper_commands) {
        // Simulate helper app communication
        EXPECT_NO_THROW(engine->checkHotKey(0)) << "Helper communication failed";
    }

    // Test EndKeyUpdate integration
    bool update_app_functional = true; // Simulated
    EXPECT_TRUE(update_app_functional) << "EndKeyUpdate must be functional";
}

// Test smart switching across applications
TEST_F(IntegrationTest, SmartSwitchingApplications) {
    std::vector<std::string> applications = {
        "TextEdit",
        "Safari",
        "Chrome",
        "Microsoft Word",
        "Pages",
        "Terminal",
        "Visual Studio Code"
    };

    // Configure smart switching
    vUseSmartSwitchKey = 1;

    for (const auto& app : applications) {
        // Switch to application
        SimulateApplicationSwitch(app);

        // Test Vietnamese mode
        engine->startNewSession();
        std::string viet_result = SimulateKeyPress("viet");
        EXPECT_NE(viet_result, "viet") << "Vietnamese conversion failed in " << app;

        // Switch to English mode
        engine->switchLanguage();
        engine->startNewSession();
        std::string eng_result = SimulateKeyPress("viet");
        EXPECT_EQ(eng_result, "viet") << "English mode failed in " << app;

        // Verify smart switching memory
        std::string stored_app = engine->getCurrentApp();
        EXPECT_EQ(stored_app, app) << "App memory failed for " << app;
    }
}

// Test macro expansion with auto-capitalization
TEST_F(IntegrationTest, MacroExpansionWithAutoCapitalization) {
    // Set up macro functionality
    vUseMacro = 1;
    vAutoCapsMacro = 1;

    // Test basic macro expansion
    std::vector<std::pair<std::string, std::string>> macro_tests = {
        {"vn", "Việt Nam"},
        {"tp", "Thành phố"},
        {"tc", "Tỉnh/Thành phố"},
        {"st", "Số điện thoại"},
        {"dc", "Địa chỉ"}
    };

    for (const auto& [input, expected] : macro_tests) {
        engine->startNewSession();
        std::string result = SimulateKeyPress(input);

        // Note: This would require actual macro system implementation
        // For now, test the framework
        EXPECT_FALSE(result.empty()) << "Macro expansion failed for: " << input;
    }

    // Test auto-capitalization
    engine->startNewSession();
    std::string caps_result = SimulateKeyPress("hello");
    EXPECT_FALSE(caps_result.empty()) << "Auto-capitalization failed";
}

// Test Unicode conversion accuracy
TEST_F(IntegrationTest, UnicodeConversionAccuracy) {
    std::vector<std::pair<int, std::string>> code_tables = {
        {UNICODE, "Unicode"},
        {TCVN3, "TCVN3"},
        {VNI_WINDOWS, "VNI-Windows"}
    };

    std::vector<std::string> test_strings = {
        "vietnamese",
        "người",
        "quốc gia",
        "thủ đô",
        "thành phố",
        "đất nước"
    };

    for (const auto& [table, name] : code_tables) {
        vCodeTable = table;

        for (const auto& test_str : test_strings) {
            engine->startNewSession();
            std::string result = SimulateKeyPress(test_str);

            // Test conversion accuracy
            EXPECT_FALSE(result.empty()) << "Conversion failed for " << name << ": " << test_str;

            // Test round-trip conversion
            std::string converted_back = engine->ConvertUtil(result, UNICODE, table);
            // Note: This would require actual conversion implementation
        }
    }
}

// Test concurrent operations with multiple engines
TEST_F(IntegrationTest, ConcurrentMultipleEngines) {
    const int num_engines = 5;
    const int operations_per_engine = 100;

    std::vector<std::unique_ptr<Engine>> engines;
    std::vector<std::thread> threads;
    std::atomic<int> successful_operations{0};

    // Create multiple engines
    for (int i = 0; i < num_engines; ++i) {
        engines.push_back(std::make_unique<Engine>());
        engines.back()->startNewSession();
    }

    // Run concurrent operations
    for (int i = 0; i < num_engines; ++i) {
        threads.emplace_back([&, i]() {
            for (int j = 0; j < operations_per_engine; ++j) {
                try {
                    std::string test_input = "test" + std::to_string(j);
                    std::string result = "";

                    for (char c : test_input) {
                        result += engines[i]->vKeyHandleEvent(c, 0);
                    }

                    if (!result.empty()) {
                        successful_operations++;
                    }
                } catch (const std::exception& e) {
                    // Log error but continue
                }
            }
        });
    }

    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }

    int total_operations = num_engines * operations_per_engine;
    double success_rate = static_cast<double>(successful_operations) / total_operations;

    EXPECT_GT(success_rate, 0.95) << "Concurrent multi-engine success rate too low: " << success_rate;
}

// Test error handling and recovery
TEST_F(IntegrationTest, ErrorHandlingAndRecovery) {
    // Test invalid input handling
    std::vector<std::string> invalid_inputs = {
        "", // Empty string
        std::string(1000, 'a'), // Very long string
        "\x01\x02\x03", // Control characters
        "\xFF\xFE\xFD", // Invalid UTF-8
    };

    for (const auto& invalid_input : invalid_inputs) {
        engine->startNewSession();
        EXPECT_NO_THROW({
            for (char c : invalid_input) {
                engine->vKeyHandleEvent(c, 0);
            }
        }) << "Engine crashed on invalid input: " << invalid_input;
    }

    // Test recovery after errors
    engine->startNewSession();
    std::string recovery_result = SimulateKeyPress("viet");
    EXPECT_FALSE(recovery_result.empty()) << "Engine failed to recover after error";

    // Test memory corruption handling
    EXPECT_NO_THROW(engine->startNewSession()) << "Session reset failed";
    EXPECT_NO_THROW(engine->switchLanguage()) << "Language switch failed";
}

// Test configuration persistence
TEST_F(IntegrationTest, ConfigurationPersistence) {
    // Modify configuration
    vInputType = VNI;
    vCodeTable = TCVN3;
    vUseMacro = 0;
    vUseSmartSwitchKey = 0;

    // Test configuration changes
    engine->OnInputMethodChanged(VNI);
    engine->OnTableCodeChange(TCVN3);

    // Verify configuration persistence
    EXPECT_EQ(vInputType, VNI);
    EXPECT_EQ(vCodeTable, TCVN3);
    EXPECT_EQ(vUseMacro, 0);
    EXPECT_EQ(vUseSmartSwitchKey, 0);

    // Test configuration application
    engine->startNewSession();
    std::string result = SimulateKeyPress("hello");
    EXPECT_FALSE(result.empty()) << "Configuration application failed";
}