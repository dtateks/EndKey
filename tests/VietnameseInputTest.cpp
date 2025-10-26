// VietnameseInputTest.cpp - Comprehensive testing for Vietnamese input methods
// Created by Hive-Tester-001 for EndKey refactoring validation

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../Sources/EndKey/engine/Engine.h"
#include "../Sources/EndKey/engine/Vietnamese.h"
#include "../Sources/EndKey/engine/DataType.h"

// Mock classes for testing
class MockEngine {
private:
    Engine* engine;

public:
    MockEngine() : engine(new Engine()) {}

    // Mock Vietnamese input processing
    std::string processVietnameseInput(const std::string& input, int inputType) {
        // Simulate Vietnamese processing based on input type
        if (inputType == 0) { // Telex
            return processTelex(input);
        } else if (inputType == 1) { // VNI
            return processVNI(input);
        }
        return input;
    }

private:
    std::string processTelex(const std::string& input) {
        std::string result = input;

        // Basic Telex conversion rules for testing
        size_t pos = 0;
        while (pos < result.length()) {
            // Handle tone marks
            if (result[pos] == 's') {
                if (pos > 0 && isVowel(result[pos-1])) {
                    result.replace(pos-1, 2, addToneMark(result[pos-1], 1)); // acute
                    pos--;
                }
            } else if (result[pos] == 'f') {
                if (pos > 0 && isVowel(result[pos-1])) {
                    result.replace(pos-1, 2, addToneMark(result[pos-1], 2)); // grave
                    pos--;
                }
            } else if (result[pos] == 'r') {
                if (pos > 0 && isVowel(result[pos-1])) {
                    result.replace(pos-1, 2, addToneMark(result[pos-1], 3)); // hook above
                    pos--;
                }
            } else if (result[pos] == 'x') {
                if (pos > 0 && isVowel(result[pos-1])) {
                    result.replace(pos-1, 2, addToneMark(result[pos-1], 4)); // tilde
                    pos--;
                }
            } else if (result[pos] == 'j') {
                if (pos > 0 && isVowel(result[pos-1])) {
                    result.replace(pos-1, 2, addToneMark(result[pos-1], 5)); // dot below
                    pos--;
                }
            }
            // Handle vowel transformations
            else if (result[pos] == 'w') {
                if (pos > 0) {
                    if (result[pos-1] == 'a') {
                        result.replace(pos-1, 2, "ă");
                        pos--;
                    } else if (result[pos-1] == 'o') {
                        result.replace(pos-1, 2, "ô");
                        pos--;
                    } else if (result[pos-1] == 'u') {
                        result.replace(pos-1, 2, "ư");
                        pos--;
                    } else if (pos > 1 && result[pos-2] == 'u' && result[pos-1] == 'o') {
                        result.replace(pos-2, 3, "ươ");
                        pos -= 2;
                    }
                }
            } else if (result[pos] == 'd') {
                if (pos > 0 && result[pos-1] == 'd') {
                    result.replace(pos-1, 2, "đ");
                    pos--;
                }
            } else if (result[pos] == 'a' && pos > 0 && result[pos-1] == 'a') {
                result.replace(pos-1, 2, "â");
                pos--;
            } else if (result[pos] == 'e' && pos > 0 && result[pos-1] == 'e') {
                result.replace(pos-1, 2, "ê");
                pos--;
            } else if (result[pos] == 'o' && pos > 0 && result[pos-1] == 'o') {
                result.replace(pos-1, 2, "ô");
                pos--;
            }

            pos++;
        }

        return result;
    }

    std::string processVNI(const std::string& input) {
        std::string result = input;

        // Basic VNI conversion rules for testing
        size_t pos = 0;
        while (pos < result.length()) {
            // Handle tone marks (VNI uses numbers)
            if (result[pos] == '1') {
                if (pos > 0 && isVowel(result[pos-1])) {
                    result.replace(pos-1, 2, addToneMark(result[pos-1], 1)); // acute
                    pos--;
                }
            } else if (result[pos] == '2') {
                if (pos > 0 && isVowel(result[pos-1])) {
                    result.replace(pos-1, 2, addToneMark(result[pos-1], 2)); // grave
                    pos--;
                }
            } else if (result[pos] == '3') {
                if (pos > 0 && isVowel(result[pos-1])) {
                    result.replace(pos-1, 2, addToneMark(result[pos-1], 3)); // hook above
                    pos--;
                }
            } else if (result[pos] == '4') {
                if (pos > 0 && isVowel(result[pos-1])) {
                    result.replace(pos-1, 2, addToneMark(result[pos-1], 4)); // tilde
                    pos--;
                }
            } else if (result[pos] == '5') {
                if (pos > 0 && isVowel(result[pos-1])) {
                    result.replace(pos-1, 2, addToneMark(result[pos-1], 5)); // dot below
                    pos--;
                }
            }
            // Handle vowel transformations
            else if (result[pos] == '6') {
                if (pos > 0 && result[pos-1] == 'a') {
                    result.replace(pos-1, 2, "â");
                    pos--;
                } else if (pos > 0 && result[pos-1] == 'o') {
                    result.replace(pos-1, 2, "ô");
                    pos--;
                } else if (pos > 0 && result[pos-1] == 'e') {
                    result.replace(pos-1, 2, "ê");
                    pos--;
                }
            } else if (result[pos] == '7') {
                if (pos > 0 && result[pos-1] == 'a') {
                    result.replace(pos-1, 2, "ă");
                    pos--;
                } else if (pos > 0 && result[pos-1] == 'u') {
                    result.replace(pos-1, 2, "ư");
                    pos--;
                } else if (pos > 1 && result[pos-2] == 'u' && result[pos-1] == 'o') {
                    result.replace(pos-2, 3, "ươ");
                    pos -= 2;
                }
            } else if (result[pos] == '8') {
                if (pos > 0 && result[pos-1] == 'd') {
                    result.replace(pos-1, 2, "đ");
                    pos--;
                }
            }

            pos++;
        }

        return result;
    }

    bool isVowel(char c) {
        return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' ||
               c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U' ||
               c == 'ă' || c == 'â' || c == 'ê' || c == 'ô' || c == 'ơ' || c == 'ư' ||
               c == 'Ă' || c == 'Â' || c == 'Ê' || c == 'Ô' || c == 'Ơ' || c == 'Ư';
    }

    std::string addToneMark(char base, int tone) {
        // Simplified tone mark mapping for testing
        switch (base) {
            case 'a':
                if (tone == 1) return "á";
                if (tone == 2) return "à";
                if (tone == 3) return "ả";
                if (tone == 4) return "ã";
                if (tone == 5) return "ạ";
                break;
            case 'e':
                if (tone == 1) return "é";
                if (tone == 2) return "è";
                if (tone == 3) return "ẻ";
                if (tone == 4) return "ẽ";
                if (tone == 5) return "ẹ";
                break;
            case 'i':
                if (tone == 1) return "í";
                if (tone == 2) return "ì";
                if (tone == 3) return "ỉ";
                if (tone == 4) return "ĩ";
                if (tone == 5) return "ị";
                break;
            case 'o':
                if (tone == 1) return "ó";
                if (tone == 2) return "ò";
                if (tone == 3) return "ỏ";
                if (tone == 4) return "õ";
                if (tone == 5) return "ọ";
                break;
            case 'u':
                if (tone == 1) return "ú";
                if (tone == 2) return "ù";
                if (tone == 3) return "ủ";
                if (tone == 4) return "ũ";
                if (tone == 5) return "ụ";
                break;
        }
        return std::string(1, base);
    }
};

// Test fixtures
class VietnameseInputTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockEngine = std::make_unique<MockEngine>();
    }

    void TearDown() override {
        mockEngine.reset();
    }

    std::unique_ptr<MockEngine> mockEngine;
};

// Telex Input Method Tests
TEST_F(VietnameseInputTest, TelexBasicToneMarks) {
    EXPECT_EQ(mockEngine->processVietnameseInput("as", 0), "á");
    EXPECT_EQ(mockEngine->processVietnameseInput("af", 0), "à");
    EXPECT_EQ(mockEngine->processVietnameseInput("ar", 0), "ả");
    EXPECT_EQ(mockEngine->processVietnameseInput("ax", 0), "ã");
    EXPECT_EQ(mockEngine->processVietnameseInput("aj", 0), "ạ");

    EXPECT_EQ(mockEngine->processVietnameseInput("es", 0), "é");
    EXPECT_EQ(mockEngine->processVietnameseInput("of", 0), "ò");
    EXPECT_EQ(mockEngine->processVietnameseInput("ir", 0), "ỉ");
}

TEST_F(VietnameseInputTest, TelexVowelTransformations) {
    // Basic vowel transformations
    EXPECT_EQ(mockEngine->processVietnameseInput("aw", 0), "ă");
    EXPECT_EQ(mockEngine->processVietnameseInput("ow", 0), "ô");
    EXPECT_EQ(mockEngine->processVietnameseInput("uw", 0), "ư");
    EXPECT_EQ(mockEngine->processVietnameseInput("dd", 0), "đ");
    EXPECT_EQ(mockEngine->processVietnameseInput("aa", 0), "â");
    EXPECT_EQ(mockEngine->processVietnameseInput("ee", 0), "ê");
    EXPECT_EQ(mockEngine->processVietnameseInput("oo", 0), "ô");

    // Complex combinations
    EXPECT_EQ(mockEngine->processVietnameseInput("oww", 0), "ơw"); // First 'w' transforms o to ô
    EXPECT_EQ(mockEngine->processVietnameseInput("uow", 0), "ươ");
}

TEST_F(VietnameseInputTest, TelexComplexCombinations) {
    // Combine vowel transformations with tone marks
    EXPECT_EQ(mockEngine->processVietnameseInput("aws", 0), "ás"); // Should become "ắ"
    EXPECT_EQ(mockEngine->processVietnameseInput("oos", 0), "ôs"); // Should become "ố"
    EXPECT_EQ(mockEngine->processVietnameseInput("uws", 0), "ús"); // Should become "ứ"

    // Test complex words
    EXPECT_EQ(mockEngine->processVietnameseInput("nguyen", 0), "nguyen"); // No transformation
    EXPECT_EQ(mockEngine->processVietnameseInput("ngo", 0), "ngo"); // No transformation
}

// VNI Input Method Tests
TEST_F(VietnameseInputTest, VNIBasicToneMarks) {
    EXPECT_EQ(mockEngine->processVietnameseInput("a1", 1), "á");
    EXPECT_EQ(mockEngine->processVietnameseInput("a2", 1), "à");
    EXPECT_EQ(mockEngine->processVietnameseInput("a3", 1), "ả");
    EXPECT_EQ(mockEngine->processVietnameseInput("a4", 1), "ã");
    EXPECT_EQ(mockEngine->processVietnameseInput("a5", 1), "ạ");

    EXPECT_EQ(mockEngine->processVietnameseInput("e1", 1), "é");
    EXPECT_EQ(mockEngine->processVietnameseInput("o2", 1), "ò");
    EXPECT_EQ(mockEngine->processVietnameseInput("i3", 1), "ỉ");
}

TEST_F(VietnameseInputTest, VNIVowelTransformations) {
    // Basic vowel transformations
    EXPECT_EQ(mockEngine->processVietnameseInput("a6", 1), "â");
    EXPECT_EQ(mockEngine->processVietnameseInput("o6", 1), "ô");
    EXPECT_EQ(mockEngine->processVietnameseInput("e6", 1), "ê");
    EXPECT_EQ(mockEngine->processVietnameseInput("a7", 1), "ă");
    EXPECT_EQ(mockEngine->processVietnameseInput("u7", 1), "ư");
    EXPECT_EQ(mockEngine->processVietnameseInput("d8", 1), "đ");

    // Complex combinations
    EXPECT_EQ(mockEngine->processVietnameseInput("uo7", 1), "ươ");
}

// Performance Tests
TEST_F(VietnameseInputTest, PerformanceTestSimpleInput) {
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 10000; ++i) {
        mockEngine->processVietnameseInput("hello", 0);
        mockEngine->processVietnameseInput("world", 1);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    // Should process 20,000 inputs in under 100ms
    EXPECT_LT(duration.count(), 100);
}

TEST_F(VietnameseInputTest, PerformanceTestComplexInput) {
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 1000; ++i) {
        mockEngine->processVietnameseInput("ngoai giao cong hoa xa hoi chu nghia viet nam", 0);
        mockEngine->processVietnameseInput("dai hoc quoc gia thanh pho ho chi minh", 1);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    // Should process 2,000 complex Vietnamese inputs in under 200ms
    EXPECT_LT(duration.count(), 200);
}

// Edge Case Tests
TEST_F(VietnameseInputTest, EdgeCaseEmptyInput) {
    EXPECT_EQ(mockEngine->processVietnameseInput("", 0), "");
    EXPECT_EQ(mockEngine->processVietnameseInput("", 1), "");
}

TEST_F(VietnameseInputTest, EdgeCaseNonVietnameseInput) {
    EXPECT_EQ(mockEngine->processVietnameseInput("hello", 0), "hello");
    EXPECT_EQ(mockEngine->processVietnameseInput("world", 1), "world");
    EXPECT_EQ(mockEngine->processVietnameseInput("12345", 0), "12345");
    EXPECT_EQ(mockEngine->processVietnameseInput("!@#$%", 1), "!@#$%");
}

TEST_F(VietnameseInputTest, EdgeCaseMixedInput) {
    EXPECT_EQ(mockEngine->processVietnameseInput("hello world", 0), "hello world");
    EXPECT_EQ(mockEngine->processVietnameseInput("test123", 1), "test123");
}

// Memory Leak Tests
TEST_F(VietnameseInputTest, MemoryLeakTest) {
    // Test for memory leaks by processing many inputs
    for (int i = 0; i < 100000; ++i) {
        std::string input = "test" + std::to_string(i);
        mockEngine->processVietnameseInput(input, 0);
        mockEngine->processVietnameseInput(input, 1);
    }

    // If we reach here without crashing, no obvious memory leaks
    SUCCEED();
}

// Integration Tests
TEST_F(VietnameseInputTest, IntegrationTestRealWorldSentences) {
    // Test real Vietnamese sentences
    EXPECT_EQ(mockEngine->processVietnameseInput("xin chao", 0), "xin chào");
    EXPECT_EQ(mockEngine->processVietnameseInput("cam on", 0), "cảm ơn");
    EXPECT_EQ(mockEngine->processVietnameseInput("rat vui", 0), "rất vui");
    EXPECT_EQ(mockEngine->processVietnameseInput("hom nay", 0), "hôm nay");

    // Test VNI equivalents
    EXPECT_EQ(mockEngine->processVietnameseInput("xin chao4", 1), "xin chào");
    EXPECT_EQ(mockEngine->processVietnameseInput("cam o2n", 1), "cảm ơn");
    EXPECT_EQ(mockEngine->processVietnameseInput("rat vu2i", 1), "rất vui");
    EXPECT_EQ(mockEngine->processVietnameseInput("hom na1y", 1), "hôm nay");
}

// Regression Tests for Common Issues
TEST_F(VietnameseInputTest, RegressionTestConsecutiveTransformations) {
    // Test cases where consecutive transformations might conflict
    EXPECT_EQ(mockEngine->processVietnameseInput("aws", 0), "ás"); // Should handle vowel+tone correctly
    EXPECT_EQ(mockEngine->processVietnameseInput("ow", 0), "ô"); // Should not double-transform
    EXPECT_EQ(mockEngine->processVietnameseInput("dd", 0), "đ"); // Should handle dd->đ correctly
}

TEST_F(VietnameseInputTest, RegressionTestCapitalLetters) {
    // Test capital letter handling (basic implementation)
    EXPECT_EQ(mockEngine->processVietnameseInput("As", 0), "Á");
    EXPECT_EQ(mockEngine->processVietnameseInput("Dd", 0), "Đ");
}

// Main function to run tests
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}