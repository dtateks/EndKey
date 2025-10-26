#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <chrono>
#include "Vietnamese.h"
#include "Engine.h"

class VietnameseInputTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine = std::make_unique<Engine>();
        engine->startNewSession();

        // Set up test configurations
        vLanguage = VIETNAMESE;
        vInputType = TELEX;
        vCodeTable = UNICODE;
    }

    void TearDown() override {
        engine.reset();
    }

    std::unique_ptr<Engine> engine;

    // Helper method to simulate typing
    std::string SimulateTyping(const std::string& input) {
        std::string result;
        for (char c : input) {
            result += engine->vKeyHandleEvent(c, 0);
        }
        return result;
    }
};

// Test Telex input method
TEST_F(VietnameseInputTest, TelexInputValidation) {
    struct TestCase {
        std::string input;
        std::string expected;
        std::string description;
    };

    std::vector<TestCase> test_cases = {
        {"hello", "hello", "English text should remain unchanged"},
        {"viet", "việt", "Simple Vietnamese with tone mark"},
        {"vien", "viện", "Vietnamese with circumflex"},
        {"toan", "toàn", "Vietnamese with grave accent"},
        {"thoi", "thôi", "Vietnamese with hook above"},
        {"hoi", "hỏi", "Vietnamese with tilde"},
        {"ngoai", "ngoài", "Vietnamese with acute accent"},
        {"duyen", "duyên", "Vietnamese with horn"},
        {"thuynhu", "thuỷnhư", "Vietnamese with dot below"},
        {"nhung", "nhưng", "Vietnamese with tilde"},
        {"quoc", "quốc", "Vietnamese with horn and acute"},
        {"nguoi", "người", "Vietnamese with horn and tilde"},
        {"thu", "thu", "Single Vietnamese word without tone"},
        {"ow", "ớ", "Vowel with horn and acute"},
        {"aw", "ă", "Vowel with breve"},
        {"aa", "â", "Vowel with circumflex"},
        {"dd", "đ", "Vietnamese d with stroke"},
        {"oo", "ô", "Vowel with circumflex"},
        {"ee", "ê", "Vowel with circumflex"},
        {"uw", "ư", "Vowel with horn"},
        {"uwo", "ướ", "Complex vowel combination"},
        {"uow", "ươ", "Complex vowel combination"},
    };

    for (const auto& test_case : test_cases) {
        engine->startNewSession();
        std::string result = SimulateTyping(test_case.input);
        EXPECT_EQ(result, test_case.expected)
            << "Failed: " << test_case.description
            << " - Input: '" << test_case.input
            << "' - Expected: '" << test_case.expected
            << "' - Got: '" << result << "'";
    }
}

// Test VNI input method
TEST_F(VietnameseInputTest, VNIInputValidation) {
    vInputType = VNI;

    struct TestCase {
        std::string input;
        std::string expected;
        std::string description;
    };

    std::vector<TestCase> test_cases = {
        {"ho1c", "hộc", "VNI with tone mark 1"},
        {"ho2c", "hộc", "VNI with tone mark 2"},
        {"ho3c", "hộc", "VNI with tone mark 3"},
        {"ho4c", "hộc", "VNI with tone mark 4"},
        {"ho5c", "hộc", "VNI with tone mark 5"},
        {"ho6c", "hộc", "VNI with tone mark 6"},
        {"ho8c", "hộc", "VNI with tone mark 8"},
        {"aaw", "âu", "VNI with circumflex"},
        {"aww", "âu", "VNI with circumflex variant"},
        {"ow", "ớ", "VNI with horn"},
        {"dd", "đ", "VNI d with stroke"},
    };

    for (const auto& test_case : test_cases) {
        engine->startNewSession();
        std::string result = SimulateTyping(test_case.input);
        EXPECT_EQ(result, test_case.expected)
            << "Failed: " << test_case.description
            << " - Input: '" << test_case.input
            << "' - Expected: '" << test_case.expected
            << "' - Got: '" << result << "'";
    }
}

// Test tone mark combinations
TEST_F(VietnameseInputTest, ToneMarkCombinations) {
    struct TestCase {
        std::string input;
        std::string expected;
        std::string description;
    };

    std::vector<TestCase> test_cases = {
        {"thuws", "thứ", "Tone mark with horn"},
        {"quocs", "quốc", "Tone mark with horn on complex vowel"},
        {"ngoais", "ngoài", "Tone mark with horn and complex vowel"},
        {"thuyens", "thuyên", "Tone mark with horn on y"},
        {"duyens", "duyên", "Tone mark with horn on u"},
        {"thuongs", "thượng", "Tone mark with circumflex"},
        {"thuoings", "thướng", "Tone mark with circumflex variant"},
    };

    for (const auto& test_case : test_cases) {
        engine->startNewSession();
        std::string result = SimulateTyping(test_case.input);
        EXPECT_EQ(result, test_case.expected)
            << "Failed: " << test_case.description
            << " - Input: '" << test_case.input
            << "' - Expected: '" << test_case.expected
            << "' - Got: '" << result << "'";
    }
}

// Test character conversion accuracy
TEST_F(VietnameseInputTest, CharacterConversionAccuracy) {
    // Test all Vietnamese characters with all tone marks
    std::vector<std::string> vowel_bases = {"a", "e", "i", "o", "u", "y"};
    std::vector<std::string> vowel_modifications = {
        "a", "ă", "â", "e", "ê", "i", "o", "ô", "ơ", "u", "ư", "y"
    };
    std::vector<std::string> tone_marks = {"", "s", "f", "r", "x", "j", "as", "af", "ar", "ax", "aj"};

    for (const auto& base : vowel_modifications) {
        for (const auto& tone : tone_marks) {
            std::string input = base + tone;
            engine->startNewSession();
            std::string result = SimulateTyping(input);

            // The result should be a valid Vietnamese character
            EXPECT_FALSE(result.empty())
                << "Empty result for input: " << input;

            // The result should be different from input if tone mark is applied
            if (!tone.empty()) {
                EXPECT_NE(result, input)
                    << "No conversion for input: " << input;
            }
        }
    }
}

// Test smart switching functionality
TEST_F(VietnameseInputTest, SmartSwitchingValidation) {
    vUseSmartSwitchKey = 1;

    // Simulate different applications
    std::vector<std::string> applications = {
        "TextEdit",
        "Chrome",
        "Safari",
        "Terminal",
        "Microsoft Word"
    };

    for (const auto& app : applications) {
        engine->OnActiveAppChanged(app);

        // Test typing in Vietnamese mode
        engine->startNewSession();
        std::string viet_result = SimulateTyping("viet");

        // Test typing in English mode
        engine->switchLanguage(); // Switch to English
        engine->startNewSession();
        std::string eng_result = SimulateTyping("viet");

        EXPECT_NE(viet_result, eng_result)
            << "Language switching failed for app: " << app;
    }
}

// Test boundary conditions
TEST_F(VietnameseInputTest, BoundaryConditions) {
    // Test empty input
    engine->startNewSession();
    std::string empty_result = SimulateTyping("");
    EXPECT_EQ(empty_result, "");

    // Test very long input
    std::string long_input = "vietnamesetypingtest" + std::string(1000, 'a');
    engine->startNewSession();
    auto start_time = std::chrono::high_resolution_clock::now();
    std::string long_result = SimulateTyping(long_input);
    auto end_time = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    EXPECT_LT(duration.count(), 1000) << "Long input processing took too long: " << duration.count() << "μs";

    // Test special characters
    std::vector<std::string> special_chars = {"123", "!@#", "\n\t", " "};
    for (const auto& special : special_chars) {
        engine->startNewSession();
        std::string result = SimulateTyping(special);
        EXPECT_EQ(result, special) << "Special characters should pass through: " << special;
    }
}

// Test concurrent input handling
TEST_F(VietnameseInputTest, ConcurrentInputHandling) {
    const int num_threads = 10;
    const int operations_per_thread = 100;

    std::vector<std::thread> threads;
    std::atomic<int> successful_operations{0};

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&successful_operations, operations_per_thread, this]() {
            for (int j = 0; j < operations_per_thread; ++j) {
                try {
                    engine->startNewSession();
                    std::string result = SimulateTyping("viet");
                    if (!result.empty()) {
                        successful_operations++;
                    }
                } catch (const std::exception& e) {
                    // Log error but continue
                }
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    EXPECT_EQ(successful_operations, num_threads * operations_per_thread)
        << "Not all concurrent operations were successful";
}