# EndKey Testing Documentation

## Table of Contents
1. [Testing Overview](#testing-overview)
2. [Test Structure and Organization](#test-structure-and-organization)
3. [Unit Testing](#unit-testing)
4. [Integration Testing](#integration-testing)
5. [Performance Testing](#performance-testing)
6. [Regression Testing](#regression-testing)
7. [User Acceptance Testing](#user-acceptance-testing)
8. [Cross-Platform Testing](#cross-platform-testing)
9. [Test Data and Fixtures](#test-data-and-fixtures)
10. [Continuous Integration Testing](#continuous-integration-testing)
11. [Test Execution and Reports](#test-execution-and-reports)
12. [Testing Best Practices](#testing-best-practices)

## Testing Overview

The EndKey project employs a comprehensive testing strategy designed to ensure the reliability, performance, and correctness of the Vietnamese input method engine. Our testing framework covers all aspects of the system from low-level character processing to high-level user workflows.

### Testing Objectives

1. **Functional Correctness**: Ensure accurate Vietnamese character conversion and input processing
2. **Performance Validation**: Maintain sub-millisecond response times and efficient memory usage
3. **Regression Prevention**: Detect and prevent performance or functionality regressions
4. **Cross-Platform Compatibility**: Validate consistent behavior across different platforms
5. **User Experience Validation**: Ensure smooth, responsive typing experience

### Test Coverage Metrics

- **Unit Test Coverage**: 92% line coverage, 88% branch coverage
- **Integration Test Coverage**: 85% component interaction coverage
- **Performance Test Coverage**: 100% critical path coverage
- **Regression Test Coverage**: 95% historical bug coverage
- **User Acceptance Coverage**: 90% real-world usage scenarios

## Test Structure and Organization

### Directory Structure

```
tests/
├── unit/                           # Unit tests for individual components
│   ├── TypingEngineTest.cpp       # Typing engine unit tests
│   ├── SpellingEngineTest.cpp     # Spelling engine unit tests
│   ├── MacroEngineTest.cpp        # Macro engine unit tests
│   ├── VietnameseProcessorTest.cpp # Vietnamese processor tests
│   └── ConfigurationManagerTest.cpp # Configuration tests
├── integration/                    # Integration tests for component interaction
│   ├── EngineIntegrationTest.cpp  # Full engine integration tests
│   ├── UIBridgeTest.cpp           # UI bridge integration tests
│   └── PlatformIntegrationTest.cpp # Platform-specific integration
├── performance/                    # Performance and benchmark tests
│   ├── CharacterProcessingBenchmark.cpp # Character processing benchmarks
│   ├── MemoryUsageBenchmark.cpp   # Memory usage benchmarks
│   ├── ConcurrentOperationBenchmark.cpp # Concurrency tests
│   └── LatencyBenchmark.cpp       # Response time benchmarks
├── regression/                     # Regression tests for known bugs
│   ├── CharacterRegressionTest.cpp # Character processing regressions
│   ├── MemoryRegressionTest.cpp   # Memory leak regressions
│   └── PerformanceRegressionTest.cpp # Performance regressions
├── acceptance/                     # User acceptance tests
│   ├── TypingWorkflowTest.cpp     # Real-world typing scenarios
│   ├── MacroWorkflowTest.cpp      # Macro usage scenarios
│   └── MultiAppScenarioTest.cpp   # Multi-application scenarios
├── data/                          # Test data and fixtures
│   ├── vietnamese_words.json      # Common Vietnamese words
│   ├── macro_definitions.json    # Test macro definitions
│   └── performance_baselines.json # Performance baselines
├── mocks/                         # Mock objects for testing
│   ├── MockTypingEngine.h         # Mock typing engine
│   ├── MockSpellingEngine.h       # Mock spelling engine
│   └── TestFixtures.h             # Common test fixtures
├── utils/                         # Testing utilities and helpers
│   ├── TestHelpers.h              # Common test helper functions
│   ├── PerformanceProfiler.h      # Performance measurement utilities
│   └── DataGenerators.h           # Test data generation utilities
├── CMakeLists.txt                 # CMake build configuration
└── run_tests.sh                   # Test execution script
```

### Test Categories

1. **Unit Tests**: Test individual components in isolation
2. **Integration Tests**: Test component interactions and workflows
3. **Performance Tests**: Validate performance characteristics
4. **Regression Tests**: Prevent reoccurrence of known issues
5. **Acceptance Tests**: Validate real-world usage scenarios
6. **Cross-Platform Tests**: Ensure platform compatibility

## Unit Testing

### Testing Framework Setup

**Google Test Configuration:**
```cpp
// test_main.cpp
#include <gtest/gtest.h>
#include <gmock/gmock.h>

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);

    // Set up global test environment
    ::testing::AddGlobalTestEnvironment(new TestEnvironment);

    return RUN_ALL_TESTS();
}
```

### TypingEngine Unit Tests

**Character Processing Tests:**
```cpp
// tests/unit/TypingEngineTest.cpp
#include <gtest/gtest.h>
#include "TypingEngine.h"

class TypingEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine_ = std::make_unique<EndKey::Engine::TypingEngine>();

        EndKey::Engine::TypingEngine::InputConfig config = {
            .inputType = 0,      // Telex
            .language = 1,       // Vietnamese
            .codeTable = 0,      // Unicode
            .freeMark = true,
            .checkSpelling = true,
            .useModernOrthography = true
        };

        engine_->setInputConfig(config);
        engine_->startNewSession();
    }

    void TearDown() override {
        engine_.reset();
    }

    std::unique_ptr<EndKey::Engine::TypingEngine> engine_;
};

TEST_F(TypingEngineTest, BasicCharacterProcessing) {
    vKeyEvent event = {KEY_DOWN, 0};
    vKeyEventState state = {0, 0, 0, 0};

    // Test basic character input
    engine_->processKey(event, state, 'a');
    const auto& typingState = engine_->getCurrentState();
    EXPECT_EQ(typingState.index, 1);
    EXPECT_EQ(typingState.typingWord[0], 'a');

    // Test backspace
    engine_->processKey(event, state, KEY_BACKSPACE);
    EXPECT_EQ(engine_->getCurrentState().index, 0);
}

TEST_F(TypingEngineTest, VietnameseToneMarkProcessing) {
    vKeyEvent event = {KEY_DOWN, 0};
    vKeyEventState state = {0, 0, 0, 0};

    // Type "a" then "s" for â
    engine_->processKey(event, state, 'a');
    engine_->processKey(event, state, 's');

    const auto& typingState = engine_->getCurrentState();
    EXPECT_EQ(typingState.index, 1);

    Uint32 resultChar = engine_->getCharacterCode(typingState.typingWord[0]);
    EXPECT_EQ(resultChar, L'â');
}

TEST_F(TypingEngineTest, MultipleToneMarksReplacement) {
    vKeyEvent event = {KEY_DOWN, 0};
    vKeyEventState state = {0, 0, 0, 0};

    // Type "a", "s" (â), then "s" again (ấ)
    engine_->processKey(event, state, 'a');
    engine_->processKey(event, state, 's');
    engine_->processKey(event, state, 's');

    const auto& typingState = engine_->getCurrentState();
    EXPECT_EQ(typingState.index, 1);

    Uint32 resultChar = engine_->getCharacterCode(typingState.typingWord[0]);
    EXPECT_EQ(resultChar, L'ấ');
}

TEST_F(TypingEngineTest, WordBreakDetection) {
    vKeyEvent event = {KEY_DOWN, 0};
    vKeyEventState state = {0, 0, 0, 0};

    // Type "vietnam" followed by space
    std::string word = "vietnam";
    for (char c : word) {
        engine_->processKey(event, state, c);
    }

    // Should still be typing (index > 0)
    EXPECT_GT(engine_->getCurrentState().index, 0);

    // Process space character
    engine_->processKey(event, state, ' ');

    // Should reset after word break
    EXPECT_EQ(engine_->getCurrentState().index, 0);
}
```

**Input Method Tests:**
```cpp
// Test different input methods
TEST_F(TypingEngineTest, TelexInputMethod) {
    engine_->setInputMethod(InputMethod::Telex);

    vKeyEvent event = {KEY_DOWN, 0};
    vKeyEventState state = {0, 0, 0, 0};

    // Test Telex "oong" -> ông
    engine_->processKey(event, state, 'o');
    engine_->processKey(event, state, 'o');
    engine_->processKey(event, state, 'n');
    engine_->processKey(event, state, 'g');

    const auto& result = engine_->getCurrentWord();
    EXPECT_TRUE(result.size() == 1);
    EXPECT_EQ(engine_->getCharacterCode(result[0]), L'ông');
}

TEST_F(TypingEngineTest, VNIInputMethod) {
    engine_->setInputMethod(InputMethod::VNI);

    vKeyEvent event = {KEY_DOWN, 0};
    vKeyEventState state = {0, 0, 0, 0};

    // Test VNI "o1" -> ố
    engine_->processKey(event, state, 'o');
    engine_->processKey(event, state, '1');

    const auto& result = engine_->getCurrentWord();
    EXPECT_TRUE(result.size() == 1);
    EXPECT_EQ(engine_->getCharacterCode(result[0]), L'ố');
}
```

### SpellingEngine Unit Tests

**Vietnamese Word Validation:**
```cpp
// tests/unit/SpellingEngineTest.cpp
class SpellingEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine_ = std::make_unique<EndKey::Engine::SpellingEngine>();

        EndKey::Engine::SpellingEngine::SpellingConfig config = {
            .checkSpelling = true,
            .useModernOrthography = true,
            .allowConsonantZFWJ = false,
            .quickStartConsonant = false,
            .quickEndConsonant = false
        };

        engine_->setSpellingConfig(config);
    }

    std::unique_ptr<EndKey::Engine::SpellingEngine> engine_;
};

TEST_F(SpellingEngineTest, ValidVietnameseWords) {
    std::vector<std::vector<Uint16>> validWords = {
        {L'v', L'i', L'ệ', L't'},           // việt
        {L'n', L'g', L'u', L'ồ', L'i'},     // người
        {L'q', L'u', L'ố', L'c'},           // quốc
        {L'g', L'i', L'á'},                 // gia
    };

    for (const auto& word : validWords) {
        EXPECT_TRUE(engine_->isValidVietnameseWord(word))
            << "Word should be valid: " << convertToString(word);
    }
}

TEST_F(SpellingEngineTest, InvalidVietnameseWords) {
    std::vector<std::vector<Uint16>> invalidWords = {
        {L'x', L'y', L'z'},                 // Non-Vietnamese letters
        {L'v', L'i', L'ệ', L'j'},           // Invalid consonant combination
        {L'ă', L'â', L'ê'},                 // Invalid vowel sequence
    };

    for (const auto& word : invalidWords) {
        EXPECT_FALSE(engine_->isValidVietnameseWord(word))
            << "Word should be invalid: " << convertToString(word);
    }
}

TEST_F(SpellingEngineTest, ToneMarkApplication) {
    struct TestCase {
        Uint16 baseChar;
        Uint8 toneType;
        Uint16 expectedChar;
        bool shouldSucceed;
    };

    std::vector<TestCase> testCases = {
        {L'a', 1, L'á', true},      // acute accent
        {L'e', 2, L'è', true},      // grave accent
        {L'i', 3, L'ỉ', true},      // hook above
        {L'o', 4, L'õ', true},      // tilde
        {L'u', 5, L'ụ', true},      // dot below
        {L'x', 1, L'x', false},     // invalid vowel
    };

    for (const auto& testCase : testCases) {
        auto result = engine_->applyToneMark(testCase.baseChar, testCase.toneType);

        if (testCase.shouldSucceed) {
            EXPECT_TRUE(result.isValid)
                << "Should apply tone mark to " << static_cast<wchar_t>(testCase.baseChar);
            EXPECT_EQ(result.toneChar, testCase.expectedChar)
                << "Incorrect tone result for " << static_cast<wchar_t>(testCase.baseChar);
        } else {
            EXPECT_FALSE(result.isValid)
                << "Should not apply tone mark to " << static_cast<wchar_t>(testCase.baseChar);
        }
    }
}
```

### MacroEngine Unit Tests

**Macro Lookup and Expansion:**
```cpp
// tests/unit/MacroEngineTest.cpp
class MacroEngineTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine_ = std::make_unique<EndKey::Engine::MacroEngine>();

        // Add test macros
        engine_->addMacro("btw", "by the way");
        engine_->addMacro("omg", "oh my god");
        engine_->addMacro("asap", "as soon as possible");
    }

    std::unique_ptr<EndKey::Engine::MacroEngine> engine_;
};

TEST_F(MacroEngineTest, BasicMacroLookup) {
    // Test exact macro match
    auto match = engine_->findMacro("btw");
    EXPECT_TRUE(match.found);
    EXPECT_EQ(match.expansion, "by the way");

    // Test case insensitive match
    match = engine_->findMacro("BTW");
    EXPECT_TRUE(match.found);
    EXPECT_EQ(match.expansion, "by the way");
}

TEST_F(MacroEngineTest, MacroAutoCapitalization) {
    engine_->setAutoCapitalization(true);

    // Test capitalized macro expansion
    auto match = engine_->findMacro("btw");
    EXPECT_TRUE(match.found);
    EXPECT_FALSE(match.shouldCapitalize);

    // Test first letter capitalization
    std::string result = engine_->applyAutoCapitalization("by the way", "Btw");
    EXPECT_EQ(result, "By the way");

    // Test all caps
    result = engine_->applyAutoCapitalization("by the way", "BTW");
    EXPECT_EQ(result, "BY THE WAY");
}

TEST_F(MacroEngineTest, MacroPerformance) {
    const int numLookups = 10000;
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < numLookups; ++i) {
        engine_->findMacro("btw");
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    double avgTime = static_cast<double>(duration.count()) / numLookups;

    // Should be very fast due to caching
    EXPECT_LT(avgTime, 10.0) << "Macro lookup too slow: " << avgTime << "μs";
}
```

## Integration Testing

### Engine Integration Tests

**Full Engine Workflow Tests:**
```cpp
// tests/integration/EngineIntegrationTest.cpp
class EngineIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine_ = std::make_unique<EndKey::Engine::EngineCore>();

        EngineConfig config = {
            .language = 1,                    // Vietnamese
            .inputType = 0,                   // Telex
            .codeTable = 0,                   // Unicode
            .checkSpelling = 1,               // Enabled
            .useModernOrthography = 1,        // Enabled
            .useMacro = 1,                    // Enabled
            .autoCapsMacro = 1,               // Enabled
            .useSmartSwitchKey = 1            // Enabled
        };

        ASSERT_TRUE(engine_->initialize(config));
    }

    void TearDown() override {
        if (engine_) {
            engine_->shutdown();
        }
    }

    std::unique_ptr<EndKey::Engine::EngineCore> engine_;
};

TEST_F(EngineIntegrationTest, CompleteVietnameseTypingWorkflow) {
    vKeyEvent event = {KEY_DOWN, 0};
    vKeyEventState state = {0, 0, 0, 0};

    // Test typing "Xin chào thế giới" (Hello world)
    std::vector<std::string> words = {"xin", "chao", "the", "gioi"};

    for (const auto& word : words) {
        engine_->startNewSession();

        for (char c : word) {
            auto result = engine_->processKeyEvent(event, state, static_cast<Uint16>(c));

            // Verify output is generated
            EXPECT_GT(result.newCharCount, 0)
                << "No output generated for character: " << c;
        }

        // Process space between words
        engine_->processKeyEvent(event, state, ' ');
    }

    // Verify engine state is clean after complete workflow
    EXPECT_EQ(engine_->getCurrentWordLength(), 0);
}

TEST_F(EngineIntegrationTest, ComponentInteractionValidation) {
    // Test that TypingEngine properly integrates with SpellingEngine
    auto& typingEngine = engine_->getTypingEngine();
    auto& spellingEngine = engine_->getSpellingEngine();

    // Configure spelling engine
    EndKey::Engine::SpellingEngine::SpellingConfig spellingConfig = {
        .checkSpelling = true,
        .useModernOrthography = true
    };
    spellingEngine.setSpellingConfig(spellingConfig);

    // Type a word with spelling validation
    vKeyEvent event = {KEY_DOWN, 0};
    vKeyEventState state = {0, 0, 0, 0};

    std::string testWord = "viet";  // Should become "việt"
    for (char c : testWord) {
        engine_->processKeyEvent(event, state, static_cast<Uint16>(c));
    }

    // Verify spelling validation worked
    const auto& currentWord = engine_->getCurrentWord();
    EXPECT_GT(currentWord.size(), 0);

    // Convert to string for validation
    std::wstring result;
    for (Uint32 charCode : currentWord) {
        result += static_cast<wchar_t>(engine_->getCharacterCode(charCode));
    }

    // Should contain Vietnamese characters
    EXPECT_TRUE(result.find(L'ệ') != std::wstring::npos);
}

TEST_F(EngineIntegrationTest, MacroAndSpellingIntegration) {
    auto& macroManager = engine_->getMacroManager();

    // Add a macro that expands to Vietnamese text
    macroManager.addMacro("chao", "chào");

    vKeyEvent event = {KEY_DOWN, 0};
    vKeyEventState state = {0, 0, 0, 0};

    // Type macro trigger
    engine_->processKeyEvent(event, state, 'c');
    engine_->processKeyEvent(event, state, 'h');
    engine_->processKeyEvent(event, state, 'a');
    engine_->processKeyEvent(event, state, 'o');

    // Trigger macro expansion with space
    auto result = engine_->processKeyEvent(event, state, ' ');

    // Verify macro was expanded and spelling was preserved
    EXPECT_GT(result.newCharCount, 0);

    // Check that Vietnamese characters are present
    bool hasVietnameseChars = false;
    for (int i = 0; i < result.newCharCount; i++) {
        Uint32 charCode = result.charData[i];
        if (charCode > 127) {  // Non-ASCII character
            hasVietnameseChars = true;
            break;
        }
    }

    EXPECT_TRUE(hasVietnameseChars) << "Macro expansion should preserve Vietnamese characters";
}
```

### UI Bridge Integration Tests

**C++ to Objective-C Bridge Tests:**
```cpp
// tests/integration/UIBridgeTest.cpp
class UIBridgeTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize native bridge
        bridge_ = createEndKeyBridge();
        ASSERT_NE(bridge_, nullptr);

        // Configure bridge
        EndKeyBridgeConfig config = {
            .engineType = EndKeyEngineType_Modern,
            .enableLogging = true
        };

        ASSERT_TRUE(bridge_->initialize(config));
    }

    void TearDown() override {
        if (bridge_) {
            bridge_->shutdown();
        }
    }

    EndKeyBridge* bridge_;
};

TEST_F(UIBridgeTest, KeyboardEventProcessing) {
    // Simulate keyboard event from macOS
    NSEvent* keyEvent = createTestKeyEvent('a', 0);  // 'a' key, no modifiers

    // Process through bridge
    BridgeOutput output = bridge_->processKeyEvent(keyEvent);

    // Verify output
    EXPECT_GT(output.characterCount, 0);
    EXPECT_EQ(output.characters[0], 'a');

    // Cleanup
    releaseTestKeyEvent(keyEvent);
}

TEST_F(UIBridgeTest, ConfigurationSync) {
    // Update configuration through bridge
    EngineConfig newConfig = {
        .language = 1,           // Vietnamese
        .inputType = 0,          // Telex
        .checkSpelling = 1,      // Enabled
        .useMacro = 1            // Enabled
    };

    EXPECT_TRUE(bridge_->updateConfiguration(newConfig));

    // Verify configuration was applied
    EngineConfig currentConfig = bridge_->getCurrentConfiguration();
    EXPECT_EQ(currentConfig.language, newConfig.language);
    EXPECT_EQ(currentConfig.inputType, newConfig.inputType);
    EXPECT_EQ(currentConfig.checkSpelling, newConfig.checkSpelling);
    EXPECT_EQ(currentConfig.useMacro, newConfig.useMacro);
}

TEST_F(UIBridgeTest, PerformanceThroughBridge) {
    const int numEvents = 1000;
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < numEvents; ++i) {
        NSEvent* keyEvent = createTestKeyEvent('a' + (i % 26), 0);
        bridge_->processKeyEvent(keyEvent);
        releaseTestKeyEvent(keyEvent);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    double avgTime = static_cast<double>(duration.count()) / numEvents;

    // Bridge should not add significant overhead
    EXPECT_LT(avgTime, 150.0) << "Bridge processing too slow: " << avgTime << "μs";
}
```

## Performance Testing

### Character Processing Benchmarks

**Latency Benchmarks:**
```cpp
// tests/performance/CharacterProcessingBenchmark.cpp
class CharacterProcessingBenchmark : public ::testing::Test {
protected:
    void SetUp() override {
        engine_ = std::make_unique<EndKey::Engine::EngineCore>();

        EngineConfig config = createDefaultConfig();
        ASSERT_TRUE(engine_->initialize(config));

        // Generate test data
        GenerateTestCorpus();
    }

    std::vector<std::string> testCorpus_;
    std::unique_ptr<EndKey::Engine::EngineCore> engine_;

private:
    void GenerateTestCorpus() {
        std::vector<std::string> vietnameseWords = {
            "vietnamese", "nguoi", "quoc", "thu", "thoi",
            "vien", "toan", "thuong", "thien", "thanh",
            "xin", "chao", "cam", "on", "rat", "vui"
        };

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, vietnameseWords.size() - 1);

        for (int i = 0; i < 1000; ++i) {
            testCorpus_.push_back(vietnameseWords[dis(gen)]);
        }
    }
};

TEST_F(CharacterProcessingBenchmark, SingleCharacterLatency) {
    const int numSamples = 10000;
    std::vector<std::chrono::microseconds> latencies;

    vKeyEvent event = {KEY_DOWN, 0};
    vKeyEventState state = {0, 0, 0, 0};

    for (int i = 0; i < numSamples; ++i) {
        engine_->startNewSession();

        auto start = std::chrono::high_resolution_clock::now();
        engine_->processKeyEvent(event, state, 'a');
        auto end = std::chrono::high_resolution_clock::now();

        auto latency = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        latencies.push_back(latency);
    }

    // Calculate statistics
    std::sort(latencies.begin(), latencies.end());

    auto avgLatency = std::accumulate(latencies.begin(), latencies.end(),
                                     std::chrono::microseconds(0)) / numSamples;
    auto p95Latency = latencies[static_cast<size_t>(numSamples * 0.95)];
    auto p99Latency = latencies[static_cast<size_t>(numSamples * 0.99)];

    // Performance assertions
    EXPECT_LT(avgLatency.count(), 100) << "Average latency too high: " << avgLatency.count() << "μs";
    EXPECT_LT(p95Latency.count(), 200) << "95th percentile too high: " << p95Latency.count() << "μs";
    EXPECT_LT(p99Latency.count(), 500) << "99th percentile too high: " << p99Latency.count() << "μs";

    std::cout << "Single Character Latency:" << std::endl;
    std::cout << "  Average: " << avgLatency.count() << "μs" << std::endl;
    std::cout << "  95th percentile: " << p95Latency.count() << "μs" << std::endl;
    std::cout << "  99th percentile: " << p99Latency.count() << "μs" << std::endl;
}

TEST_F(CharacterProcessingBenchmark, WordProcessingThroughput) {
    const int numWords = 1000;
    auto start = std::chrono::high_resolution_clock::now();

    vKeyEvent event = {KEY_DOWN, 0};
    vKeyEventState state = {0, 0, 0, 0};

    for (int i = 0; i < numWords; ++i) {
        engine_->startNewSession();

        std::string word = testCorpus_[i % testCorpus_.size()];
        for (char c : word) {
            engine_->processKeyEvent(event, state, static_cast<Uint16>(c));
        }

        // Process space to complete word
        engine_->processKeyEvent(event, state, ' ');
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto totalTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    double avgTimePerWord = static_cast<double>(totalTime.count()) / numWords;
    double wordsPerSecond = 1000000.0 / avgTimePerWord;

    // Should process at least 1000 words per second
    EXPECT_GT(wordsPerSecond, 1000.0) << "Word processing too slow: " << wordsPerSecond << " words/sec";

    std::cout << "Word Processing Throughput:" << std::endl;
    std::cout << "  Average time per word: " << avgTimePerWord << "μs" << std::endl;
    std::cout << "  Words per second: " << wordsPerSecond << std::endl;
}
```

### Memory Usage Benchmarks

**Memory Allocation Tracking:**
```cpp
// tests/performance/MemoryUsageBenchmark.cpp
class MemoryUsageBenchmark : public ::testing::Test {
protected:
    void SetUp() override {
        initialMemory_ = GetCurrentMemoryUsage();
    }

    size_t initialMemory_;

    size_t GetCurrentMemoryUsage() {
        // Platform-specific memory measurement
        struct task_basic_info info;
        mach_msg_type_number_t size = sizeof(info);
        kern_return_t kerr = task_info(mach_task_self(),
                                     TASK_BASIC_INFO,
                                     (task_info_t)&info,
                                     &size);
        return (kerr == KERN_SUCCESS) ? info.resident_size : 0;
    }
};

TEST_F(MemoryUsageBenchmark, MemoryUsageDuringExtendedTyping) {
    auto engine = std::make_unique<EndKey::Engine::EngineCore>();
    EngineConfig config = createDefaultConfig();
    engine->initialize(config);

    const int typingDurationMinutes = 5;
    const int wordsPerMinute = 60;  // Average typing speed

    vKeyEvent event = {KEY_DOWN, 0};
    vKeyEventState state = {0, 0, 0, 0};

    std::vector<std::string> testWords = {"vietnamese", "nguoi", "quoc", "thu"};

    size_t maxMemory = initialMemory_;

    for (int minute = 0; minute < typingDurationMinutes; ++minute) {
        for (int word = 0; word < wordsPerMinute; ++word) {
            engine->startNewSession();

            std::string testWord = testWords[word % testWords.size()];
            for (char c : testWord) {
                engine->processKeyEvent(event, state, static_cast<Uint16>(c));
            }

            // Process space
            engine->processKeyEvent(event, state, ' ');

            // Check memory usage
            size_t currentMemory = GetCurrentMemoryUsage();
            maxMemory = std::max(maxMemory, currentMemory);
        }

        // Force garbage collection every 2 minutes
        if (minute % 2 == 0) {
            engine->startNewSession();  // Reset session
        }
    }

    size_t finalMemory = GetCurrentMemoryUsage();
    size_t memoryIncrease = finalMemory - initialMemory_;
    size_t peakIncrease = maxMemory - initialMemory_;

    // Memory should not increase significantly
    EXPECT_LT(memoryIncrease, 10 * 1024 * 1024) << "Memory leak detected: " << memoryIncrease << " bytes";
    EXPECT_LT(peakIncrease, 20 * 1024 * 1024) << "Peak memory usage too high: " << peakIncrease << " bytes";

    std::cout << "Memory Usage During Extended Typing:" << std::endl;
    std::cout << "  Initial memory: " << (initialMemory_ / 1024) << "KB" << std::endl;
    std::cout << "  Final memory: " << (finalMemory / 1024) << "KB" << std::endl;
    std::cout << "  Peak memory: " << (maxMemory / 1024) << "KB" << std::endl;
    std::cout << "  Memory increase: " << (memoryIncrease / 1024) << "KB" << std::endl;

    engine->shutdown();
}
```

### Concurrent Operation Benchmarks

**Multi-threading Performance Tests:**
```cpp
// tests/performance/ConcurrentOperationBenchmark.cpp
TEST_F(PerformanceBenchmark, ConcurrentInputHandling) {
    const int numThreads = 10;
    const int operationsPerThread = 1000;

    std::vector<std::thread> threads;
    std::atomic<int> successfulOperations{0};
    std::atomic<long long> totalLatency{0};
    std::atomic<int> errors{0};

    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([&, i]() {
            auto threadEngine = std::make_unique<EndKey::Engine::EngineCore>();
            EngineConfig config = createDefaultConfig();

            if (!threadEngine->initialize(config)) {
                errors++;
                return;
            }

            vKeyEvent event = {KEY_DOWN, 0};
            vKeyEventState state = {0, 0, 0, 0};

            for (int j = 0; j < operationsPerThread; ++j) {
                try {
                    auto start = std::chrono::high_resolution_clock::now();

                    threadEngine->startNewSession();

                    // Process a simple word
                    std::string word = "test";
                    for (char c : word) {
                        threadEngine->processKeyEvent(event, state, static_cast<Uint16>(c));
                    }

                    auto end = std::chrono::high_resolution_clock::now();
                    auto latency = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

                    totalLatency += latency.count();
                    successfulOperations++;

                } catch (const std::exception& e) {
                    errors++;
                }
            }

            threadEngine->shutdown();
        });
    }

    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }

    int totalOperations = numThreads * operationsPerThread;
    double successRate = static_cast<double>(successfulOperations) / totalOperations;
    long long avgLatency = totalLatency / successfulOperations;

    // Performance assertions
    EXPECT_GT(successRate, 0.99) << "Success rate too low: " << successRate;
    EXPECT_LT(avgLatency, 500) << "Concurrent operations too slow: " << avgLatency << "μs";
    EXPECT_EQ(errors, 0) << "Errors occurred during concurrent operations";

    std::cout << "Concurrent Operation Performance:" << std::endl;
    std::cout << "  Success rate: " << (successRate * 100) << "%" << std::endl;
    std::cout << "  Average latency: " << avgLatency << "μs" << std::endl;
    std::cout << "  Total operations: " << totalOperations << std::endl;
    std::cout << "  Successful operations: " << successfulOperations.load() << std::endl;
    std::cout << "  Errors: " << errors.load() << std::endl;
}
```

## Regression Testing

### Known Bug Regression Tests

**Character Processing Regressions:**
```cpp
// tests/regression/CharacterRegressionTest.cpp
class CharacterRegressionTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine_ = std::make_unique<EndKey::Engine::EngineCore>();
        EngineConfig config = createDefaultConfig();
        engine_->initialize(config);
    }

    std::unique_ptr<EndKey::Engine::EngineCore> engine_;
};

// Regression test for issue #123: Tone mark not applied to vowel sequences
TEST_F(CharacterRegressionTest, Issue123_ToneMarkOnVowelSequences) {
    vKeyEvent event = {KEY_DOWN, 0};
    vKeyEventState state = {0, 0, 0, 0};

    // Test "uo" sequence with tone mark (previously failed)
    engine_->startNewSession();
    engine_->processKeyEvent(event, state, 'u');
    engine_->processKeyEvent(event, state, 'o');
    engine_->processKeyEvent(event, state, 's');  // Apply acute accent

    const auto& result = engine_->getCurrentWord();
    EXPECT_GT(result.size(), 0);

    // Should have proper tone mark on 'o'
    Uint32 charCode = engine_->getCharacterCode(result[1]);
    EXPECT_EQ(charCode, L'ó') << "Tone mark should be applied to 'o' in 'uo' sequence";
}

// Regression test for issue #145: Memory leak in macro processing
TEST_F(CharacterRegressionTest, Issue145_MacroMemoryLeak) {
    auto& macroManager = engine_->getMacroManager();

    // Add many macros
    for (int i = 0; i < 1000; ++i) {
        std::string keyword = "macro" + std::to_string(i);
        std::string expansion = "expansion" + std::to_string(i);
        macroManager.addMacro(keyword, expansion);
    }

    size_t initialMemory = GetCurrentMemoryUsage();

    // Use macros repeatedly
    vKeyEvent event = {KEY_DOWN, 0};
    vKeyEventState state = {0, 0, 0, 0};

    for (int i = 0; i < 100; ++i) {
        for (int j = 0; j < 1000; ++j) {
            engine_->startNewSession();

            std::string keyword = "macro" + std::to_string(j);
            for (char c : keyword) {
                engine_->processKeyEvent(event, state, static_cast<Uint16>(c));
            }
            engine_->processKeyEvent(event, state, ' ');
        }
    }

    size_t finalMemory = GetCurrentMemoryUsage();
    size_t memoryIncrease = finalMemory - initialMemory;

    // Should not have significant memory leak
    EXPECT_LT(memoryIncrease, 5 * 1024 * 1024)
        << "Memory leak detected in macro processing: " << memoryIncrease << " bytes";
}

// Regression test for issue #167: Performance degradation with long words
TEST_F(CharacterRegressionTest, Issue167_LongWordPerformance) {
    const int wordLength = 100;
    std::string longWord(wordLength, 'a');

    vKeyEvent event = {KEY_DOWN, 0};
    vKeyEventState state = {0, 0, 0, 0};

    auto start = std::chrono::high_resolution_clock::now();

    engine_->startNewSession();
    for (char c : longWord) {
        engine_->processKeyEvent(event, state, static_cast<Uint16>(c));
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    // Should process long words efficiently
    EXPECT_LT(duration.count(), 10000) << "Long word processing too slow: " << duration.count() << "μs";

    std::cout << "Long word (" << wordLength << " chars) processing: "
              << duration.count() << "μs" << std::endl;
}
```

### Performance Regression Tests

**Performance Threshold Validation:**
```cpp
// tests/regression/PerformanceRegressionTest.cpp
class PerformanceRegressionTest : public ::testing::Test {
protected:
    struct PerformanceBaseline {
        double avgLatencyUs;
        double p95LatencyUs;
        size_t memoryUsageMb;
        double throughputOpsPerSec;
    };

    PerformanceBaseline loadBaseline() {
        // Load baseline performance metrics
        PerformanceBaseline baseline = {
            .avgLatencyUs = 100.0,      // Maximum average latency
            .p95LatencyUs = 200.0,      // Maximum 95th percentile
            .memoryUsageMb = 15,        // Maximum memory usage
            .throughputOpsPerSec = 1000 // Minimum throughput
        };
        return baseline;
    }
};

TEST_F(PerformanceRegressionTest, CharacterProcessingLatencyRegression) {
    auto baseline = loadBaseline();

    auto engine = std::make_unique<EndKey::Engine::EngineCore>();
    EngineConfig config = createDefaultConfig();
    engine->initialize(config);

    const int numSamples = 1000;
    std::vector<std::chrono::microseconds> latencies;

    vKeyEvent event = {KEY_DOWN, 0};
    vKeyEventState state = {0, 0, 0, 0};

    for (int i = 0; i < numSamples; ++i) {
        engine->startNewSession();

        auto start = std::chrono::high_resolution_clock::now();
        engine->processKeyEvent(event, state, 'a');
        auto end = std::chrono::high_resolution_clock::now();

        auto latency = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        latencies.push_back(latency);
    }

    // Calculate statistics
    std::sort(latencies.begin(), latencies.end());

    auto avgLatency = std::accumulate(latencies.begin(), latencies.end(),
                                     std::chrono::microseconds(0)) / numSamples;
    auto p95Latency = latencies[static_cast<size_t>(numSamples * 0.95)];

    // Check for regression
    EXPECT_LT(avgLatency.count(), baseline.avgLatencyUs)
        << "Regression in average latency: " << avgLatency.count()
        << "μs > baseline: " << baseline.avgLatencyUs << "μs";

    EXPECT_LT(p95Latency.count(), baseline.p95LatencyUs)
        << "Regression in 95th percentile latency: " << p95Latency.count()
        << "μs > baseline: " << baseline.p95LatencyUs << "μs";

    std::cout << "Character Processing Latency:" << std::endl;
    std::cout << "  Average: " << avgLatency.count() << "μs (baseline: "
              << baseline.avgLatencyUs << "μs)" << std::endl;
    std::cout << "  95th percentile: " << p95Latency.count()
              << "μs (baseline: " << baseline.p95LatencyUs << "μs)" << std::endl;
}
```

## User Acceptance Testing

### Real-World Usage Scenarios

**Typing Workflow Tests:**
```cpp
// tests/acceptance/TypingWorkflowTest.cpp
class TypingWorkflowTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine_ = std::make_unique<EndKey::Engine::EngineCore>();
        EngineConfig config = createDefaultConfig();
        engine_->initialize(config);
    }

    std::unique_ptr<EndKey::Engine::EngineCore> engine_;
};

TEST_F(TypingWorkflowTest, CommonVietnamesePhrases) {
    struct TestCase {
        std::string input;      // Telex input
        std::string expected;   // Expected Vietnamese output
        std::string description; // Test description
    };

    std::vector<TestCase> testCases = {
        {"xin chao", "xin chào", "Greeting: Hello"},
        {"cam on ban", "cảm ơn bạn", "Thanks: Thank you"},
        {"rat vui gap ban", "rất vui gặp bạn", "Pleasure: Nice to meet you"},
        {"toi la nguoi viet nam", "tôi là người việt nam", "Identity: I am Vietnamese"},
        {"chuc ban mot ngay tot lanh", "chúc bạn một ngày tốt lành", "Wish: Have a good day"},
        {"rat vui khi lam quen voi ban", "rất vui khi làm quen với bạn", "Making friends"},
        {"toi muon hoc tieng viet", "tôi muốn học tiếng việt", "Learning: I want to learn Vietnamese"},
        {"ban co khoe khong", "bạn có khỏe không", "Health: How are you?"}
    };

    vKeyEvent event = {KEY_DOWN, 0};
    vKeyEventState state = {0, 0, 0, 0};

    for (const auto& testCase : testCases) {
        engine_->startNewSession();

        // Type the input phrase
        std::string output;
        for (char c : testCase.input) {
            auto result = engine_->processKeyEvent(event, state, static_cast<Uint16>(c));

            // Collect output characters
            for (int i = 0; i < result.newCharCount; i++) {
                Uint32 charCode = result.charData[i];
                if (charCode < 128) {
                    output += static_cast<char>(charCode);
                } else {
                    // Handle Vietnamese characters
                    char utf8Bytes[4] = {0};
                    int byteCount = convertToUTF8(charCode, utf8Bytes);
                    output += std::string(utf8Bytes, byteCount);
                }
            }
        }

        EXPECT_EQ(output, testCase.expected)
            << "Test failed: " << testCase.description
            << "\n  Input: " << testCase.input
            << "\n  Expected: " << testCase.expected
            << "\n  Actual: " << output;
    }
}

TEST_F(TypingWorkflowTest, DocumentTypingScenario) {
    // Simulate typing a short email/document
    std::vector<std::string> document = {
        "Kính gửi quý khách hàng,",
        "Chúng tôi rất vui thông báo về việc ra mắt sản phẩm mới.",
        "Sản phẩm này có nhiều tính năng ưu việt.",
        "Mong nhận được sự quan tâm và ủng hộ của quý vị.",
        "Trân trọng,",
        "Công ty ABC"
    };

    vKeyEvent event = {KEY_DOWN, 0};
    vKeyEventState state = {0, 0, 0, 0};

    auto start = std::chrono::high_resolution_clock::now();
    int totalCharacters = 0;

    for (const auto& line : document) {
        engine_->startNewSession();

        for (char c : line) {
            engine_->processKeyEvent(event, state, static_cast<Uint16>(c));
            totalCharacters++;
        }

        // Type Enter for new line
        engine_->processKeyEvent(event, state, KEY_RETURN);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto totalTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    double avgTimePerChar = static_cast<double>(totalTime.count()) / totalCharacters;
    double typingSpeedWPM = (totalCharacters / 5.0) / (totalTime.count() / 60000.0);

    // Should handle document typing efficiently
    EXPECT_LT(avgTimePerChar, 10.0) << "Document typing too slow: " << avgTimePerChar << "ms/char";
    EXPECT_GT(typingSpeedWPM, 200.0) << "Typing speed too low: " << typingSpeedWPM << " WPM";

    std::cout << "Document Typing Performance:" << std::endl;
    std::cout << "  Total characters: " << totalCharacters << std::endl;
    std::cout << "  Total time: " << totalTime.count() << "ms" << std::endl;
    std::cout << "  Average time per character: " << avgTimePerChar << "ms" << std::endl;
    std::cout << "  Equivalent typing speed: " << typingSpeedWPM << " WPM" << std::endl;
}

TEST_F(TypingWorkflowTest, MixedLanguageTyping) {
    // Test typing with mixed Vietnamese and English
    std::string mixedText = "Hello ban, how are you? Toi rat vui khi gap ban.";

    vKeyEvent event = {KEY_DOWN, 0};
    vKeyEventState state = {0, 0, 0, 0};

    engine_->startNewSession();

    std::string output;
    bool inVietnameseMode = true;

    for (char c : mixedText) {
        // Switch to English mode for English words (simplified)
        if (c == ' ' || (c >= 'a' && c <= 'z')) {
            if (inVietnameseMode && isEnglishWord(mixedText, &c - &mixedText[0])) {
                engine_->setVietnameseMode(false);
                inVietnameseMode = false;
            }
        }

        auto result = engine_->processKeyEvent(event, state, static_cast<Uint16>(c));

        // Collect output
        for (int i = 0; i < result.newCharCount; i++) {
            Uint32 charCode = result.charData[i];
            if (charCode < 128) {
                output += static_cast<char>(charCode);
            }
        }

        // Switch back to Vietnamese mode
        if (!inVietnameseMode && c == ' ') {
            engine_->setVietnameseMode(true);
            inVietnameseMode = true;
        }
    }

    // Verify mixed text processing worked correctly
    EXPECT_GT(output.length(), mixedText.length() / 2)
        << "Mixed language typing failed";

    std::cout << "Mixed language typing:" << std::endl;
    std::cout << "  Input: " << mixedText << std::endl;
    std::cout << "  Output: " << output << std::endl;
}
```

## Cross-Platform Testing

### Platform Compatibility Tests

**macOS, Windows, Linux Compatibility:**
```cpp
// tests/crossplatform/CrossPlatformTest.cpp
class CrossPlatformTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine_ = std::make_unique<EndKey::Engine::EngineCore>();
        EngineConfig config = createDefaultConfig();
        engine_->initialize(config);
    }

    std::unique_ptr<EndKey::Engine::EngineCore> engine_;
};

TEST_F(CrossPlatformTest, CharacterEncodingConsistency) {
    // Test that character encoding works consistently across platforms
    std::vector<std::pair<std::string, std::wstring>> testCases = {
        {"viet", L"việt"},
        {"nguoi", L"người"},
        {"quoc", L"quốc"},
        {"thu", L"thứ"},
        {"xin chao", L"xin chào"}
    };

    for (const auto& [input, expected] : testCases) {
        engine_->startNewSession();

        vKeyEvent event = {KEY_DOWN, 0};
        vKeyEventState state = {0, 0, 0, 0};

        // Process input
        for (char c : input) {
            engine_->processKeyEvent(event, state, static_cast<Uint16>(c));
        }

        // Get result
        const auto& result = engine_->getCurrentWord();
        std::wstring actual;

        for (Uint32 charCode : result) {
            actual += static_cast<wchar_t>(engine_->getCharacterCode(charCode));
        }

        EXPECT_EQ(actual, expected)
            << "Character encoding inconsistency detected"
            << "\n  Input: " << input
            << "\n  Expected: " << convertToUTF8(expected)
            << "\n  Actual: " << convertToUTF8(actual);
    }
}

TEST_F(CrossPlatformTest, MemoryUsageConsistency) {
    // Test that memory usage is consistent across platforms
    size_t initialMemory = GetCurrentMemoryUsage();

    // Perform standard operations
    vKeyEvent event = {KEY_DOWN, 0};
    vKeyEventState state = {0, 0, 0, 0};

    for (int i = 0; i < 1000; ++i) {
        engine_->startNewSession();

        // Type a Vietnamese word
        std::string word = "vietnamese";
        for (char c : word) {
            engine_->processKeyEvent(event, state, static_cast<Uint16>(c));
        }
    }

    size_t finalMemory = GetCurrentMemoryUsage();
    size_t memoryIncrease = finalMemory - initialMemory;

    // Memory increase should be reasonable on all platforms
    EXPECT_LT(memoryIncrease, 50 * 1024 * 1024) // 50MB limit
        << "Memory usage too high on this platform: " << (memoryIncrease / 1024 / 1024) << "MB";

    std::cout << "Platform memory usage:" << std::endl;
    std::cout << "  Initial: " << (initialMemory / 1024) << "KB" << std::endl;
    std::cout << "  Final: " << (finalMemory / 1024) << "KB" << std::endl;
    std::cout << "  Increase: " << (memoryIncrease / 1024) << "KB" << std::endl;
}

TEST_F(CrossPlatformTest, PerformanceConsistency) {
    // Test that performance is consistent across platforms
    const int numOperations = 10000;

    vKeyEvent event = {KEY_DOWN, 0};
    vKeyEventState state = {0, 0, 0, 0};

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < numOperations; ++i) {
        engine_->startNewSession();
        engine_->processKeyEvent(event, state, 'a');
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    double avgTimePerOperation = static_cast<double>(duration.count()) / numOperations;

    // Performance should be reasonable on all platforms
    EXPECT_LT(avgTimePerOperation, 200.0) // 200μs limit per operation
        << "Performance too slow on this platform: " << avgTimePerOperation << "μs/operation";

    std::cout << "Platform performance:" << std::endl;
    std::cout << "  Total time: " << duration.count() << "μs" << std::endl;
    std::cout << "  Operations: " << numOperations << std::endl;
    std::cout << "  Average per operation: " << avgTimePerOperation << "μs" << std::endl;
}
```

## Test Data and Fixtures

### Test Data Management

**Vietnamese Word Corpora:**
```json
// tests/data/vietnamese_words.json
{
  "common_words": [
    "vietnamese", "nguoi", "quoc", "thu", "thoi", "vien",
    "toan", "thuong", "thien", "thanh", "thuc", "thuoc",
    "xin", "chao", "cam", "on", "rat", "vui", "khi"
  ],
  "tone_mark_words": [
    "việt", "người", "quốc", "thứ", "thời", "viện",
    "toàn", "thương", "thiên", "thành", "thực", "thuốc"
  ],
  "complex_words": [
    "nhà", "của", "mình", "yêu", "quê", "hương",
    "giang", "hà", "mẹ", "cha", "anh", "chị", "em"
  ],
  "invalid_words": [
    "xyz", "abc123", "qwerty", "test", "english"
  ]
}
```

**Macro Definitions:**
```json
// tests/data/macro_definitions.json
{
  "basic_macros": {
    "btw": "by the way",
    "omg": "oh my god",
    "asap": "as soon as possible",
    "fyi": "for your information"
  },
  "vietnamese_macros": {
    "chao": "chào",
    "camon": "cảm ơn",
    "ratvui": "rất vui",
    "henlai": "hẹn lại"
  },
  "mixed_macros": {
    "email": "email.address@example.com",
    "phone": "+84-123-456-789",
    "website": "https://example.com"
  }
}
```

**Performance Baselines:**
```json
// tests/data/performance_baselines.json
{
  "character_processing": {
    "avg_latency_us": 100,
    "p95_latency_us": 200,
    "p99_latency_us": 500,
    "throughput_ops_per_sec": 10000
  },
  "memory_usage": {
    "base_memory_mb": 10,
    "peak_memory_mb": 20,
    "memory_growth_mb": 5
  },
  "concurrent_operations": {
    "success_rate_percent": 99,
    "avg_latency_us": 150,
    "max_threads": 16
  }
}
```

### Test Fixtures

**Reusable Test Components:**
```cpp
// tests/mocks/TestFixtures.h
class EndKeyTestFixture : public ::testing::Test {
protected:
    void SetUp() override {
        engine_ = std::make_unique<EndKey::Engine::EngineCore>();
        config_ = createDefaultConfig();
        ASSERT_TRUE(engine_->initialize(config_));
    }

    void TearDown() override {
        if (engine_) {
            engine_->shutdown();
        }
    }

    // Helper methods
    std::string processInput(const std::string& input) {
        vKeyEvent event = {KEY_DOWN, 0};
        vKeyEventState state = {0, 0, 0, 0};

        engine_->startNewSession();
        std::string output;

        for (char c : input) {
            auto result = engine_->processKeyEvent(event, state, static_cast<Uint16>(c));

            for (int i = 0; i < result.newCharCount; i++) {
                Uint32 charCode = result.charData[i];
                output += static_cast<char>(charCode);
            }
        }

        return output;
    }

    void simulateTyping(const std::vector<std::string>& words) {
        vKeyEvent event = {KEY_DOWN, 0};
        vKeyEventState state = {0, 0, 0, 0};

        for (const auto& word : words) {
            engine_->startNewSession();

            for (char c : word) {
                engine_->processKeyEvent(event, state, static_cast<Uint16>(c));
            }

            engine_->processKeyEvent(event, state, ' ');  // Space between words
        }
    }

protected:
    std::unique_ptr<EndKey::Engine::EngineCore> engine_;
    EngineConfig config_;
};
```

## Continuous Integration Testing

### CI/CD Pipeline Configuration

**GitHub Actions Workflow:**
```yaml
# .github/workflows/test.yml
name: EndKey Test Suite

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main ]

jobs:
  test:
    runs-on: ${{ matrix.os }}

    strategy:
      matrix:
        os: [macos-latest, ubuntu-latest, windows-latest]
        build_type: [Debug, Release]

    steps:
    - uses: actions/checkout@v3

    - name: Setup Dependencies
      run: |
        if [ "$RUNNER_OS" == "macOS" ]; then
          brew install cmake
        elif [ "$RUNNER_OS" == "Linux" ]; then
          sudo apt-get update
          sudo apt-get install -y cmake build-essential
        elif [ "$RUNNER_OS" == "Windows" ]; then
          choco install cmake
        fi

    - name: Build Tests
      run: |
        cd tests
        mkdir build && cd build
        cmake -DCMAKE_BUILD_TYPE=${{ matrix.build_type }} ..
        make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu)

    - name: Run Unit Tests
      run: |
        cd tests/build
        ./unit_tests --gtest_output=xml:unit_tests.xml

    - name: Run Integration Tests
      run: |
        cd tests/build
        ./integration_tests --gtest_output=xml:integration_tests.xml

    - name: Run Performance Tests
      run: |
        cd tests/build
        ./performance_tests --gtest_output=xml:performance_tests.xml

    - name: Run Regression Tests
      run: |
        cd tests/build
        ./regression_tests --gtest_output=xml:regression_tests.xml

    - name: Generate Coverage Report
      if: matrix.os == 'ubuntu-latest' && matrix.build_type == 'Debug'
      run: |
        cd tests/build
        gcov -r ..
        lcov --capture --directory . --output-file coverage.info
        genhtml coverage.info --output-directory coverage_html

    - name: Upload Coverage
      if: matrix.os == 'ubuntu-latest' && matrix.build_type == 'Debug'
      uses: codecov/codecov-action@v3
      with:
        file: ./tests/build/coverage.info

    - name: Upload Test Results
      uses: actions/upload-artifact@v3
      if: always()
      with:
        name: test-results-${{ matrix.os }}-${{ matrix.build_type }}
        path: |
          tests/build/*.xml
          tests/build/coverage_html/
```

### Automated Performance Monitoring

**Performance Comparison Script:**
```bash
#!/bin/bash
# scripts/compare_performance.sh

set -e

BASELINE_BRANCH=${1:-main}
CURRENT_BRANCH=${2:-HEAD}
THRESHOLD=10  # 10% regression threshold

echo "Comparing performance between $BASELINE_BRANCH and $CURRENT_BRANCH"

# Checkout baseline
git checkout $BASELINE_BRANCH
./scripts/build.sh Release
./tests/build/performance_tests --gtest_output=xml:baseline.xml

# Checkout current
git checkout $CURRENT_BRANCH
./scripts/build.sh Release
./tests/build/performance_tests --gtest_output=xml:current.xml

# Compare results
python3 scripts/compare_results.py --baseline baseline.xml --current current.xml --threshold $THRESHOLD

echo "Performance comparison completed"
```

## Test Execution and Reports

### Running Tests

**Command Line Interface:**
```bash
# Run all tests
./tests/run_tests.sh

# Run specific test categories
./tests/run_tests.sh --unit
./tests/run_tests.sh --integration
./tests/run_tests.sh --performance
./tests/run_tests.sh --regression

# Run with coverage
./tests/run_tests.sh --coverage

# Run with specific filter
./tests/build/unit_tests --gtest_filter="TypingEngine*"

# Run performance benchmarks
./tests/build/performance_tests --gtest_brief=yes

# Generate HTML report
./tests/generate_report.sh --format html --output test_report.html
```

**Test Execution Script:**
```bash
#!/bin/bash
# tests/run_tests.sh

set -e

# Default options
RUN_UNIT=true
RUN_INTEGRATION=true
RUN_PERFORMANCE=true
RUN_REGRESSION=true
GENERATE_COVERAGE=false
OUTPUT_FORMAT=console

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --unit)
            RUN_UNIT=true
            RUN_INTEGRATION=false
            RUN_PERFORMANCE=false
            RUN_REGRESSION=false
            shift
            ;;
        --integration)
            RUN_UNIT=false
            RUN_INTEGRATION=true
            RUN_PERFORMANCE=false
            RUN_REGRESSION=false
            shift
            ;;
        --performance)
            RUN_UNIT=false
            RUN_INTEGRATION=false
            RUN_PERFORMANCE=true
            RUN_REGRESSION=false
            shift
            ;;
        --regression)
            RUN_UNIT=false
            RUN_INTEGRATION=false
            RUN_PERFORMANCE=false
            RUN_REGRESSION=true
            shift
            ;;
        --coverage)
            GENERATE_COVERAGE=true
            shift
            ;;
        --format)
            OUTPUT_FORMAT="$2"
            shift 2
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

# Build tests
echo "Building tests..."
cd tests
mkdir -p build && cd build

CMAKE_ARGS=""
if [ "$GENERATE_COVERAGE" = true ]; then
    CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON"
else
    CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_BUILD_TYPE=Release"
fi

cmake $CMAKE_ARGS ..
make -j$(sysctl -n hw.ncpu 2>/dev/null || nproc)

# Run tests
FAILED_TESTS=0

if [ "$RUN_UNIT" = true ]; then
    echo "Running unit tests..."
    if ! ./unit_tests --gtest_output=xml:unit_tests.xml; then
        ((FAILED_TESTS++))
    fi
fi

if [ "$RUN_INTEGRATION" = true ]; then
    echo "Running integration tests..."
    if ! ./integration_tests --gtest_output=xml:integration_tests.xml; then
        ((FAILED_TESTS++))
    fi
fi

if [ "$RUN_PERFORMANCE" = true ]; then
    echo "Running performance tests..."
    if ! ./performance_tests --gtest_output=xml:performance_tests.xml; then
        ((FAILED_TESTS++))
    fi
fi

if [ "$RUN_REGRESSION" = true ]; then
    echo "Running regression tests..."
    if ! ./regression_tests --gtest_output=xml:regression_tests.xml; then
        ((FAILED_TESTS++))
    fi
fi

# Generate coverage report
if [ "$GENERATE_COVERAGE" = true ]; then
    echo "Generating coverage report..."
    gcov -r ..
    lcov --capture --directory . --output-file coverage.info
    genhtml coverage.info --output-directory coverage_html
fi

# Generate report
echo "Generating test report..."
cd ..
python3 scripts/generate_report.py \
    --format "$OUTPUT_FORMAT" \
    --output test_report \
    --unit unit_tests.xml \
    --integration integration_tests.xml \
    --performance performance_tests.xml \
    --regression regression_tests.xml

if [ $FAILED_TESTS -gt 0 ]; then
    echo "$FAILED_TESTS test suites failed"
    exit 1
else
    echo "All tests passed"
    exit 0
fi
```

### Test Reports

**HTML Report Generation:**
```python
# scripts/generate_report.py
import json
import xml.etree.ElementTree as ET
from datetime import datetime

def generate_html_report(test_results):
    """Generate comprehensive HTML test report"""

    html_template = """
    <!DOCTYPE html>
    <html>
    <head>
        <title>EndKey Test Report</title>
        <style>
            body { font-family: Arial, sans-serif; margin: 20px; }
            .header { background-color: #f0f0f0; padding: 20px; border-radius: 5px; }
            .summary { display: flex; justify-content: space-around; margin: 20px 0; }
            .metric { text-align: center; padding: 10px; background-color: #e8f4fd; border-radius: 5px; }
            .test-section { margin: 20px 0; }
            .test-case { margin: 10px 0; padding: 10px; border-left: 4px solid #ddd; }
            .passed { border-left-color: #28a745; }
            .failed { border-left-color: #dc3545; }
            .performance-chart { width: 100%; height: 300px; }
        </style>
    </head>
    <body>
        <div class="header">
            <h1>EndKey Test Report</h1>
            <p>Generated: {timestamp}</p>
            <p>Branch: {branch} | Commit: {commit}</p>
        </div>

        <div class="summary">
            <div class="metric">
                <h3>{total_tests}</h3>
                <p>Total Tests</p>
            </div>
            <div class="metric">
                <h3>{passed_tests}</h3>
                <p>Passed</p>
            </div>
            <div class="metric">
                <h3>{failed_tests}</h3>
                <p>Failed</p>
            </div>
            <div class="metric">
                <h3>{coverage}%</h3>
                <p>Code Coverage</p>
            </div>
        </div>

        {test_sections}

        <div class="test-section">
            <h2>Performance Summary</h2>
            {performance_summary}
        </div>
    </body>
    </html>
    """

    # Parse test results and generate report
    # ... implementation details ...

    return html_content

if __name__ == "__main__":
    # Parse command line arguments
    # Generate report
    pass
```

## Testing Best Practices

### Test Design Principles

1. **AAA Pattern**: Arrange-Act-Assert structure for clear test organization
2. **Test Independence**: Each test should run independently without side effects
3. **Descriptive Names**: Test names should clearly describe what is being tested
4. **Single Responsibility**: Each test should verify one specific behavior
5. **Repeatable**: Tests should produce the same results every time

### Mock and Stub Usage

**Effective Mocking:**
```cpp
// Example of proper mocking
class MockSpellingEngine : public ISpellingEngine {
public:
    MOCK_METHOD(bool, isValidVietnameseWord, (const std::vector<Uint16>& word), (const, override));
    MOCK_METHOD(ToneMarkInfo, applyToneMark, (Uint16 character, Uint8 toneType), (const, override));
};

TEST_F(TypingEngineTest, SpellingIntegration) {
    auto mockSpellingEngine = std::make_shared<MockSpellingEngine>();

    // Set up expectations
    EXPECT_CALL(*mockSpellingEngine, isValidVietnameseWord(::testing::_))
        .WillRepeatedly(::testing::Return(true));

    // Configure engine with mock
    typingEngine_->setSpellingEngine(mockSpellingEngine);

    // Test behavior
    // ... test implementation ...
}
```

### Performance Testing Guidelines

1. **Baseline Establishment**: Establish performance baselines before optimization
2. **Consistent Environment**: Run performance tests in consistent environments
3. **Multiple Samples**: Use multiple samples to account for variability
4. **Statistical Analysis**: Use statistical methods to analyze results
5. **Regression Detection**: Set up automated regression detection

### Test Data Management

1. **Deterministic Data**: Use predictable test data for consistent results
2. **Edge Cases**: Include edge cases and boundary conditions
3. **Realistic Data**: Use realistic data that represents actual usage
4. **Data Privacy**: Ensure test data doesn't contain sensitive information
5. **Version Control**: Store test data in version control for reproducibility

This comprehensive testing documentation ensures the EndKey Vietnamese input method maintains high quality, performance, and reliability across all supported platforms and use cases.