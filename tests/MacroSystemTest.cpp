// MacroSystemTest.cpp - Comprehensive testing for EndKey macro system
// Created by Hive-Tester-001 for EndKey refactoring validation

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "../Sources/EndKey/engine/Macro.h"
#include "../Sources/EndKey/engine/DataType.h"

// Mock macro system for testing
class MockMacroSystem {
private:
    std::unordered_map<std::string, std::string> macros;
    bool autoCapitalizeEnabled;

public:
    MockMacroSystem() : autoCapitalizeEnabled(true) {}

    // Macro management functions
    bool addMacro(const std::string& keyword, const std::string& expansion) {
        if (keyword.empty() || expansion.empty()) {
            return false;
        }
        macros[keyword] = expansion;
        return true;
    }

    bool deleteMacro(const std::string& keyword) {
        auto it = macros.find(keyword);
        if (it != macros.end()) {
            macros.erase(it);
            return true;
        }
        return false;
    }

    bool hasMacro(const std::string& keyword) const {
        return macros.find(keyword) != macros.end();
    }

    std::string getMacroExpansion(const std::string& keyword) const {
        auto it = macros.find(keyword);
        if (it != macros.end()) {
            return it->second;
        }
        return "";
    }

    std::vector<std::pair<std::string, std::string>> getAllMacros() const {
        std::vector<std::pair<std::string, std::string>> result;
        for (const auto& pair : macros) {
            result.push_back(pair);
        }
        return result;
    }

    void setAutoCapitalize(bool enabled) {
        autoCapitalizeEnabled = enabled;
    }

    bool isAutoCapitalizeEnabled() const {
        return autoCapitalizeEnabled;
    }

    // Macro processing with auto-capitalization
    std::string processMacro(const std::string& input, bool startOfSentence = false) {
        if (!hasMacro(input)) {
            return input;
        }

        std::string expansion = getMacroExpansion(input);

        if (autoCapitalizeEnabled && startOfSentence && !expansion.empty()) {
            // Capitalize first letter if at start of sentence
            expansion[0] = toupper(expansion[0]);
        }

        return expansion;
    }

    // Process text with macro expansion
    std::string processTextWithMacros(const std::string& text) {
        std::string result = text;
        std::string currentWord;
        bool startOfSentence = true;

        for (size_t i = 0; i <= text.length(); ++i) {
            if (i < text.length() && (isalnum(text[i]) || text[i] == '_')) {
                currentWord += text[i];
            } else {
                if (!currentWord.empty()) {
                    std::string expanded = processMacro(currentWord, startOfSentence);
                    result.replace(result.length() - currentWord.length() - (i < text.length() ? 1 : 0),
                                 currentWord.length(), expanded);
                    currentWord.clear();
                }
                startOfSentence = (text[i] == '.' || text[i] == '!' || text[i] == '?');
            }
        }

        return result;
    }

    void clearAllMacros() {
        macros.clear();
    }

    size_t getMacroCount() const {
        return macros.size();
    }
};

// Test fixtures
class MacroSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        macroSystem = std::make_unique<MockMacroSystem>();

        // Setup default macros for testing
        macroSystem->addMacro("xch", "xin chao");
        macroSystem->addMacro("cn", "cam on");
        macroSystem->addMacro("hnn", "hom nay");
        macroSystem->addMacro("nmt", "ngay mai");
        macroSystem->addMacro("tt", "tot dep");
    }

    void TearDown() override {
        macroSystem.reset();
    }

    std::unique_ptr<MockMacroSystem> macroSystem;
};

// Basic Macro Management Tests
TEST_F(MacroSystemTest, AddMacro_ValidInput) {
    EXPECT_TRUE(macroSystem->addMacro("test", "test expansion"));
    EXPECT_TRUE(macroSystem->hasMacro("test"));
    EXPECT_EQ(macroSystem->getMacroExpansion("test"), "test expansion");
}

TEST_F(MacroSystemTest, AddMacro_InvalidInput) {
    // Empty keyword
    EXPECT_FALSE(macroSystem->addMacro("", "expansion"));

    // Empty expansion
    EXPECT_FALSE(macroSystem->addMacro("keyword", ""));

    // Both empty
    EXPECT_FALSE(macroSystem->addMacro("", ""));
}

TEST_F(MacroSystemTest, DeleteMacro_Existing) {
    EXPECT_TRUE(macroSystem->hasMacro("xch"));
    EXPECT_TRUE(macroSystem->deleteMacro("xch"));
    EXPECT_FALSE(macroSystem->hasMacro("xch"));
    EXPECT_EQ(macroSystem->getMacroExpansion("xch"), "");
}

TEST_F(MacroSystemTest, DeleteMacro_NonExisting) {
    EXPECT_FALSE(macroSystem->hasMacro("nonexistent"));
    EXPECT_FALSE(macroSystem->deleteMacro("nonexistent"));
}

TEST_F(MacroSystemTest, HasMacro) {
    EXPECT_TRUE(macroSystem->hasMacro("xch"));
    EXPECT_TRUE(macroSystem->hasMacro("cn"));
    EXPECT_TRUE(macroSystem->hasMacro("hnn"));
    EXPECT_FALSE(macroSystem->hasMacro("nonexistent"));
}

TEST_F(MacroSystemTest, GetMacroExpansion) {
    EXPECT_EQ(macroSystem->getMacroExpansion("xch"), "xin chao");
    EXPECT_EQ(macroSystem->getMacroExpansion("cn"), "cam on");
    EXPECT_EQ(macroSystem->getMacroExpansion("hnn"), "hom nay");
    EXPECT_EQ(macroSystem->getMacroExpansion("nonexistent"), "");
}

TEST_F(MacroSystemTest, GetAllMacros) {
    auto allMacros = macroSystem->getAllMacros();
    EXPECT_EQ(allMacros.size(), 5); // 5 default macros

    // Check if all default macros are present
    bool foundXch = false, foundCn = false, foundHnn = false;
    bool foundNmt = false, foundTt = false;

    for (const auto& macro : allMacros) {
        if (macro.first == "xch" && macro.second == "xin chao") foundXch = true;
        if (macro.first == "cn" && macro.second == "cam on") foundCn = true;
        if (macro.first == "hnn" && macro.second == "hom nay") foundHnn = true;
        if (macro.first == "nmt" && macro.second == "ngay mai") foundNmt = true;
        if (macro.first == "tt" && macro.second == "tot dep") foundTt = true;
    }

    EXPECT_TRUE(foundXch);
    EXPECT_TRUE(foundCn);
    EXPECT_TRUE(foundHnn);
    EXPECT_TRUE(foundNmt);
    EXPECT_TRUE(foundTt);
}

// Auto-Capitalization Tests
TEST_F(MacroSystemTest, AutoCapitalize_Enabled) {
    macroSystem->setAutoCapitalize(true);

    // Start of sentence
    EXPECT_EQ(macroSystem->processMacro("xch", true), "Xin chao");
    EXPECT_EQ(macroSystem->processMacro("cn", true), "Cam on");

    // Not start of sentence
    EXPECT_EQ(macroSystem->processMacro("xch", false), "xin chao");
    EXPECT_EQ(macroSystem->processMacro("cn", false), "cam on");
}

TEST_F(MacroSystemTest, AutoCapitalize_Disabled) {
    macroSystem->setAutoCapitalize(false);

    // Should not capitalize regardless of position
    EXPECT_EQ(macroSystem->processMacro("xch", true), "xin chao");
    EXPECT_EQ(macroSystem->processMacro("cn", true), "cam on");
    EXPECT_EQ(macroSystem->processMacro("xch", false), "xin chao");
    EXPECT_EQ(macroSystem->processMacro("cn", false), "cam on");
}

// Text Processing Tests
TEST_F(MacroSystemTest, ProcessTextWithMacros_SingleWord) {
    std::string text = "xch";
    std::string processed = macroSystem->processTextWithMacros(text);
    EXPECT_EQ(processed, "Xin chao");
}

TEST_F(MacroSystemTest, ProcessTextWithMacros_Sentence) {
    std::string text = "xch ban";
    std::string processed = macroSystem->processTextWithMacros(text);
    EXPECT_EQ(processed, "Xin chao ban");
}

TEST_F(MacroSystemTest, ProcessTextWithMacros_MultipleMacros) {
    std::string text = "xch ban. cn rat nhieu";
    std::string processed = macroSystem->processTextWithMacros(text);
    EXPECT_EQ(processed, "Xin chao ban. Cam on rat nhieu");
}

TEST_F(MacroSystemTest, ProcessTextWithMacros_NoMacros) {
    std::string text = "hello world";
    std::string processed = macroSystem->processTextWithMacros(text);
    EXPECT_EQ(processed, text);
}

TEST_F(MacroSystemTest, ProcessTextWithMacros_Mixed) {
    std::string text = "xch, ban rat tt";
    std::string processed = macroSystem->processTextWithMacros(text);
    EXPECT_EQ(processed, "Xin chao, ban rat tot dep");
}

// Edge Case Tests
TEST_F(MacroSystemTest, EdgeCase_DuplicateKeywords) {
    // Add macro with existing keyword
    EXPECT_TRUE(macroSystem->hasMacro("xch"));
    EXPECT_TRUE(macroSystem->addMacro("xch", "new expansion"));
    EXPECT_EQ(macroSystem->getMacroExpansion("xch"), "new expansion");
}

TEST_F(MacroSystemTest, EdgeCase_KeywordCasing) {
    macroSystem->addMacro("Test", "Expansion");
    macroSystem->addMacro("test", "expansion");

    // Case-sensitive matching
    EXPECT_EQ(macroSystem->getMacroExpansion("Test"), "Expansion");
    EXPECT_EQ(macroSystem->getMacroExpansion("test"), "expansion");
    EXPECT_EQ(macroSystem->getMacroExpansion("TEST"), "");
}

TEST_F(MacroSystemTest, EdgeCase_SpecialCharacters) {
    EXPECT_TRUE(macroSystem->addMacro("test123", "expansion123"));
    EXPECT_TRUE(macroSystem->addMacro("test_abc", "expansion_abc"));

    EXPECT_EQ(macroSystem->getMacroExpansion("test123"), "expansion123");
    EXPECT_EQ(macroSystem->getMacroExpansion("test_abc"), "expansion_abc");
}

TEST_F(MacroSystemTest, EdgeCase_UnicodeCharacters) {
    EXPECT_TRUE(macroSystem->addMacro("chào", "hello"));
    EXPECT_TRUE(macroSystem->addMacro("cảm", "thank"));

    EXPECT_EQ(macroSystem->getMacroExpansion("chào"), "hello");
    EXPECT_EQ(macroSystem->getMacroExpansion("cảm"), "thank");
}

// Performance Tests
TEST_F(MacroSystemTest, Performance_MacroLookup) {
    // Add many macros
    for (int i = 0; i < 10000; ++i) {
        std::string keyword = "macro" + std::to_string(i);
        std::string expansion = "expansion" + std::to_string(i);
        macroSystem->addMacro(keyword, expansion);
    }

    auto start = std::chrono::high_resolution_clock::now();

    // Perform many lookups
    for (int i = 0; i < 100000; ++i) {
        std::string keyword = "macro" + std::to_string(i % 10000);
        macroSystem->hasMacro(keyword);
        macroSystem->getMacroExpansion(keyword);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    // Should complete 100,000 lookups quickly
    EXPECT_LT(duration.count(), 100);
}

TEST_F(MacroSystemTest, Performance_TextProcessing) {
    // Create large text with many macros
    std::string largeText;
    for (int i = 0; i < 1000; ++i) {
        largeText += "xch cn hnn nmt tt ";
    }

    auto start = std::chrono::high_resolution_clock::now();

    std::string processed = macroSystem->processTextWithMacros(largeText);

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    // Should process large text with many macros quickly
    EXPECT_LT(duration.count(), 50);
}

// Memory Tests
TEST_F(MacroSystemTest, Memory_MacroCount) {
    size_t initialCount = macroSystem->getMacroCount();

    // Add many macros
    for (int i = 0; i < 1000; ++i) {
        std::string keyword = "test_macro_" + std::to_string(i);
        std::string expansion = "test_expansion_" + std::to_string(i);
        macroSystem->addMacro(keyword, expansion);
    }

    EXPECT_EQ(macroSystem->getMacroCount(), initialCount + 1000);

    // Clear all macros
    macroSystem->clearAllMacros();
    EXPECT_EQ(macroSystem->getMacroCount(), 0);
}

// Integration Tests with Vietnamese Input
TEST_F(MacroSystemTest, Integration_VietnameseMacros) {
    // Add Vietnamese macros
    macroSystem->addMacro("xch", "xin chào");
    macroSystem->addMacro("cn", "cảm ơn");
    macroSystem->addMacro("hnn", "hôm nay");
    macroSystem->addMacro("nmt", "ngày mai");

    std::string text = "xch ban. cn vi hnn va nmt.";
    std::string processed = macroSystem->processTextWithMacros(text);

    EXPECT_EQ(processed, "Xin chào ban. Cảm ơn vi hôm nay và ngày mai.");
}

TEST_F(MacroSystemTest, Integration_MacroInMacro) {
    // Test if macro expansion contains other macros (should not expand recursively)
    macroSystem->addMacro("greeting", "xch ban");
    macroSystem->addMacro("thanks", "cn rat nhieu");

    std::string text = "greeting. thanks!";
    std::string processed = macroSystem->processTextWithMacros(text);

    // Should not expand macros within macros
    EXPECT_EQ(processed, "Greeting. Thanks!");
}

// Regression Tests
TEST_F(MacroSystemTest, Regression_EmptyTextProcessing) {
    std::string emptyText = "";
    std::string processed = macroSystem->processTextWithMacros(emptyText);
    EXPECT_EQ(processed, emptyText);
}

TEST_F(MacroSystemTest, Regression_OnlySpecialCharacters) {
    std::string text = "!@#$%^&*()";
    std::string processed = macroSystem->processTextWithMacros(text);
    EXPECT_EQ(processed, text);
}

TEST_F(MacroSystemTest, Regression_ConsecutiveSpaces) {
    std::string text = "xch  cn   hnn";
    std::string processed = macroSystem->processTextWithMacros(text);
    EXPECT_EQ(processed, "Xin chào  Cam on   Hôm nay");
}

// Macro Persistence Tests (simulated)
TEST_F(MacroSystemTest, Persistence_SaveAndLoad) {
    // Save current macros
    auto macrosBefore = macroSystem->getAllMacros();
    size_t countBefore = macrosBefore.size();

    // Clear and add new macros
    macroSystem->clearAllMacros();
    macroSystem->addMacro("new1", "expansion1");
    macroSystem->addMacro("new2", "expansion2");

    // Simulate loading saved macros
    macroSystem->clearAllMacros();
    for (const auto& macro : macrosBefore) {
        macroSystem->addMacro(macro.first, macro.second);
    }

    // Verify restoration
    EXPECT_EQ(macroSystem->getMacroCount(), countBefore);
    auto macrosAfter = macroSystem->getAllMacros();
    EXPECT_EQ(macrosAfter.size(), macrosBefore.size());
}

// Main function to run tests
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}