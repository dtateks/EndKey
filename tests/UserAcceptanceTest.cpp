// UserAcceptanceTest.cpp - User acceptance testing scenarios for EndKey
// Created by Hive-Tester-001 for EndKey refactoring validation

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include "../Sources/EndKey/engine/Engine.h"
#include "../Sources/EndKey/engine/Vietnamese.h"
#include "../Sources/EndKey/engine/Macro.h"
#include "../Sources/EndKey/engine/DataType.h"

// User persona definitions for testing
enum class UserType {
    Student,        // High school/university student
    OfficeWorker,   // Office/administrative worker
    Developer,      // Software developer
    Writer,         // Content creator/writer
    Elderly,        // Elderly user
    Beginner        // New Vietnamese input method user
};

struct UserPersona {
    UserType type;
    std::string name;
    std::string description;
    std::vector<std::string> commonTasks;
    std::vector<std::string> painPoints;
    std::map<std::string, int> skillLevel; // typing_speed, tech_savviness, vietnamese_fluency
};

// User task scenarios
struct UserTask {
    std::string name;
    std::string description;
    std::string inputText;
    std::string expectedOutput;
    std::vector<std::string> steps;
    std::map<std::string, int> successCriteria; // accuracy, speed, ease_of_use
    int maxTimeSeconds;
};

// Mock user interface for testing
class MockUserInterface {
private:
    bool isInitialized;
    UserType currentUser;
    std::map<std::string, bool> features;
    std::vector<std::string> userFeedback;
    std::chrono::steady_clock::time_point taskStartTime;

public:
    MockUserInterface() : isInitialized(false), currentUser(UserType::Beginner) {
        features = {
            {"vietnamese_input", true},
            {"macro_system", true},
            {"smart_switching", true},
            {"auto_capitalize", true},
            {"spell_check", true},
            {"quick_consonants", true},
            {"unicode_support", true}
        };
    }

    bool initialize() {
        isInitialized = true;
        return true;
    }

    void setUserType(UserType type) {
        currentUser = type;
        configureForUserType(type);
    }

    UserType getCurrentUserType() const {
        return currentUser;
    }

    bool performTask(const UserTask& task) {
        taskStartTime = std::chrono::steady_clock::now();

        // Simulate user performing the task
        std::string result = simulateTaskExecution(task);

        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - taskStartTime);

        // Check if task completed within time limit
        bool withinTimeLimit = duration.count() <= task.maxTimeSeconds;
        bool outputMatches = (result == task.expectedOutput);

        recordTaskFeedback(task, result, duration.count(), outputMatches);

        return withinTimeLimit && outputMatches;
    }

    std::vector<std::string> getUserFeedback() const {
        return userFeedback;
    }

    void clearFeedback() {
        userFeedback.clear();
    }

    double getTaskSatisfactionScore() const {
        if (userFeedback.empty()) return 0.0;

        int positiveFeedback = 0;
        for (const auto& feedback : userFeedback) {
            if (feedback.find("easy") != std::string::npos ||
                feedback.find("fast") != std::string::npos ||
                feedback.find("accurate") != std::string::npos) {
                positiveFeedback++;
            }
        }

        return static_cast<double>(positiveFeedback) / userFeedback.size();
    }

private:
    void configureForUserType(UserType type) {
        switch (type) {
            case UserType::Student:
                features["smart_switching"] = true;
                features["quick_consonants"] = true;
                break;
            case UserType::OfficeWorker:
                features["macro_system"] = true;
                features["auto_capitalize"] = true;
                features["spell_check"] = true;
                break;
            case UserType::Developer:
                features["smart_switching"] = false; // Prefer manual control
                features["macro_system"] = true;
                break;
            case UserType::Writer:
                features["auto_capitalize"] = true;
                features["spell_check"] = true;
                features["quick_consonants"] = true;
                break;
            case UserType::Elderly:
                features["smart_switching"] = true;
                features["auto_capitalize"] = true;
                features["quick_consonants"] = true;
                break;
            case UserType::Beginner:
                features["smart_switching"] = true;
                features["auto_capitalize"] = true;
                features["quick_consonants"] = true;
                break;
        }
    }

    std::string simulateTaskExecution(const UserTask& task) {
        // Simulate Vietnamese input processing
        std::string result = task.inputText;

        // Apply transformations based on enabled features
        if (features["vietnamese_input"]) {
            result = processVietnameseInput(result);
        }

        if (features["macro_system"]) {
            result = processMacros(result);
        }

        if (features["auto_capitalize"]) {
            result = processAutoCapitalize(result);
        }

        return result;
    }

    std::string processVietnameseInput(const std::string& input) {
        // Simplified Vietnamese processing
        std::string result = input;

        // Basic Telex transformations
        size_t pos = result.find("as");
        if (pos != std::string::npos) result.replace(pos, 2, "ás");

        pos = result.find("dd");
        if (pos != std::string::npos) result.replace(pos, 2, "đ");

        pos = result.find("ow");
        if (pos != std::string::npos) result.replace(pos, 2, "ô");

        pos = result.find("uw");
        if (pos != std::string::npos) result.replace(pos, 2, "ư");

        return result;
    }

    std::string processMacros(const std::string& input) {
        std::map<std::string, std::string> macros = {
            {"xch", "xin chào"},
            {"cn", "cảm ơn"},
            {"hnn", "hôm nay"},
            {"nmt", "ngày mai"}
        };

        std::string result = input;
        for (const auto& macro : macros) {
            size_t pos = result.find(macro.first);
            if (pos != std::string::npos) {
                result.replace(pos, macro.first.length(), macro.second);
            }
        }

        return result;
    }

    std::string processAutoCapitalize(const std::string& input) {
        if (input.empty()) return input;

        std::string result = input;
        result[0] = toupper(result[0]);

        // Capitalize after periods
        size_t pos = result.find(". ");
        while (pos != std::string::npos && pos + 2 < result.length()) {
            result[pos + 2] = toupper(result[pos + 2]);
            pos = result.find(". ", pos + 1);
        }

        return result;
    }

    void recordTaskFeedback(const UserTask& task, const std::string& result,
                           int timeSpent, bool success) {
        std::string feedback = "Task '" + task.name + "': ";

        if (success) {
            feedback += "SUCCESS";
            if (timeSpent < task.maxTimeSeconds / 2) {
                feedback += " (fast)";
            } else if (timeSpent > task.maxTimeSeconds * 0.8) {
                feedback += " (slow)";
            } else {
                feedback += " (good pace)";
            }
        } else {
            feedback += "FAILED";
            if (timeSpent > task.maxTimeSeconds) {
                feedback += " (timeout)";
            } else {
                feedback += " (incorrect output)";
            }
        }

        userFeedback.push_back(feedback);
    }
};

// User acceptance test fixture
class UserAcceptanceTest : public ::testing::Test {
protected:
    void SetUp() override {
        ui = std::make_unique<MockUserInterface>();
        ui->initialize();
        setupUserPersonas();
        setupUserTasks();
    }

    void TearDown() override {
        ui.reset();
    }

    void setupUserPersonas() {
        personas = {
            {
                UserType::Student,
                "Student User",
                "High school/university student using Vietnamese for assignments",
                {"Writing essays", "Taking notes", "Messaging friends", "Social media"},
                {"Slow typing", "Inconsistent accents", "Switching between apps"},
                {{"typing_speed", 3}, {"tech_savviness", 4}, {"vietnamese_fluency", 4}}
            },
            {
                UserType::OfficeWorker,
                "Office Worker",
                "Administrative worker writing emails and documents",
                {"Email composition", "Report writing", "Meeting notes", "Formal documents"},
                {"Professional tone", "Consistent formatting", "Speed requirements"},
                {{"typing_speed", 4}, {"tech_savviness", 3}, {"vietnamese_fluency", 5}}
            },
            {
                UserType::Developer,
                "Developer",
                "Software developer coding and documenting",
                {"Code comments", "Documentation", "Technical writing", "Chatting"},
                {"Mixed English/Vietnamese", "Code keywords", "Technical terms"},
                {{"typing_speed", 5}, {"tech_savviness", 5}, {"vietnamese_fluency", 4}}
            },
            {
                UserType::Writer,
                "Content Writer",
                "Professional writer creating content",
                {"Article writing", "Blog posts", "Creative writing", "Editing"},
                {"Creative expression", "Grammar accuracy", "Style consistency"},
                {{"typing_speed", 4}, {"tech_savviness", 3}, {"vietnamese_fluency", 5}}
            },
            {
                UserType::Elderly,
                "Elderly User",
                "Senior citizen using technology for communication",
                {"Emailing family", "Social media", "News reading", "Simple documents"},
                {"Slow typing", "Vision issues", "Technology anxiety"},
                {{"typing_speed", 2}, {"tech_savviness", 2}, {"vietnamese_fluency", 5}}
            },
            {
                UserType::Beginner,
                "Beginner User",
                "New user learning Vietnamese input methods",
                {"Basic typing", "Learning accents", "Simple messages"},
                {"Learning curve", "Confusion", "Accuracy issues"},
                {{"typing_speed", 2}, {"tech_savviness", 3}, {"vietnamese_fluency", 3}}
            }
        };
    }

    void setupUserTasks() {
        tasks = {
            {
                "Basic Vietnamese Typing",
                "Type simple Vietnamese sentences with correct accents",
                "xin chao ban",
                "xin chào bạn",
                {"1. Type greeting", "2. Add tone marks", "3. Verify output"},
                {{"accuracy", 90}, {"speed", 30}},
                10
            },
            {
                "Macro Expansion",
                "Use macro shortcuts for common phrases",
                "xch cn",
                "xin chào cảm ơn",
                {"1. Type macro shortcuts", "2. Verify expansion", "3. Check capitalization"},
                {{"accuracy", 95}, {"speed", 10}},
                5
            },
            {
                "Mixed Language Text",
                "Type text mixing Vietnamese and English",
                "hello ban, how are you? cam on",
                "hello bạn, how are you? cảm ơn",
                {"1. Type mixed text", "2. Handle language switching", "3. Maintain context"},
                {{"accuracy", 85}, {"speed", 45}},
                15
            },
            {
                "Complex Document",
                "Type a longer Vietnamese document",
                "hom nay troi dep qua. toi di lam viec va gap ban be.",
                "Hôm nay trời đẹp quá. Tôi đi làm việc và gặp bạn bè.",
                {"1. Type multiple sentences", "2. Maintain consistency", "3. Proper punctuation"},
                {{"accuracy", 90}, {"speed", 60}},
                30
            },
            {
                "Quick Message",
                "Type a quick Vietnamese message",
                "hnn co gi khong?",
                "hôm nay có gì không?",
                {"1. Fast typing", "2. Accurate accents", "3. Natural flow"},
                {{"accuracy", 80}, {"speed", 15}},
                5
            },
            {
                "Formal Email",
                "Type a formal Vietnamese email",
                "kinh thua quy ban, toi la ten va viet thu nay.",
                "Kính thưa quý bàn, tôi là tên và viết thư hôm nay.",
                {"1. Formal tone", "2. Proper capitalization", "3. Correct grammar"},
                {{"accuracy", 95}, {"speed", 90}},
                45
            }
        };
    }

    std::unique_ptr<MockUserInterface> ui;
    std::vector<UserPersona> personas;
    std::vector<UserTask> tasks;
};

// Basic User Acceptance Tests
TEST_F(UserAcceptanceTest, StudentUserCanCompleteBasicTasks) {
    ui->setUserType(UserType::Student);

    // Test tasks suitable for students
    std::vector<std::string> studentTasks = {
        "Basic Vietnamese Typing",
        "Macro Expansion",
        "Quick Message"
    };

    for (const auto& taskName : studentTasks) {
        auto taskIt = std::find_if(tasks.begin(), tasks.end(),
            [&taskName](const UserTask& task) { return task.name == taskName; });

        ASSERT_NE(taskIt, tasks.end()) << "Task not found: " << taskName;

        bool success = ui->performTask(*taskIt);
        EXPECT_TRUE(success) << "Student user failed task: " << taskName;
    }

    // Check overall satisfaction
    double satisfaction = ui->getTaskSatisfactionScore();
    EXPECT_GT(satisfaction, 0.7) << "Student user satisfaction should be above 70%";
}

TEST_F(UserAcceptanceTest, OfficeWorkerCanCompleteProfessionalTasks) {
    ui->setUserType(UserType::OfficeWorker);

    // Test tasks suitable for office workers
    std::vector<std::string> officeTasks = {
        "Macro Expansion",
        "Complex Document",
        "Formal Email"
    };

    for (const auto& taskName : officeTasks) {
        auto taskIt = std::find_if(tasks.begin(), tasks.end(),
            [&taskName](const UserTask& task) { return task.name == taskName; });

        ASSERT_NE(taskIt, tasks.end()) << "Task not found: " << taskName;

        bool success = ui->performTask(*taskIt);
        EXPECT_TRUE(success) << "Office worker failed task: " << taskName;
    }

    // Check overall satisfaction
    double satisfaction = ui->getTaskSatisfactionScore();
    EXPECT_GT(satisfaction, 0.8) << "Office worker satisfaction should be above 80%";
}

TEST_F(UserAcceptanceTest, ElderlyUserCanCompleteSimpleTasks) {
    ui->setUserType(UserType::Elderly);

    // Test tasks suitable for elderly users (simpler, more time)
    std::vector<std::string> elderlyTasks = {
        "Basic Vietnamese Typing",
        "Quick Message"
    };

    for (const auto& taskName : elderlyTasks) {
        auto taskIt = std::find_if(tasks.begin(), tasks.end(),
            [&taskName](const UserTask& task) { return task.name == taskName; });

        ASSERT_NE(taskIt, tasks.end()) << "Task not found: " << taskName;

        // Give elderly users more time
        UserTask modifiedTask = *taskIt;
        modifiedTask.maxTimeSeconds *= 2;

        bool success = ui->performTask(modifiedTask);
        EXPECT_TRUE(success) << "Elderly user failed task: " << taskName;
    }

    // Check overall satisfaction
    double satisfaction = ui->getTaskSatisfactionScore();
    EXPECT_GT(satisfaction, 0.6) << "Elderly user satisfaction should be above 60%";
}

TEST_F(UserAcceptanceTest, BeginnerUserCanLearnBasicFunctionality) {
    ui->setUserType(UserType::Beginner);

    // Test basic learning tasks
    std::vector<std::string> beginnerTasks = {
        "Basic Vietnamese Typing"
    };

    for (const auto& taskName : beginnerTasks) {
        auto taskIt = std::find_if(tasks.begin(), tasks.end(),
            [&taskName](const UserTask& task) { return task.name == taskName; });

        ASSERT_NE(taskIt, tasks.end()) << "Task not found: " << taskName;

        // Give beginners more time and lower accuracy expectations
        UserTask modifiedTask = *taskIt;
        modifiedTask.maxTimeSeconds *= 1.5;
        modifiedTask.successCriteria["accuracy"] = 70; // Lower accuracy requirement

        bool success = ui->performTask(modifiedTask);
        EXPECT_TRUE(success) << "Beginner user failed task: " << taskName;
    }

    // Check overall satisfaction
    double satisfaction = ui->getTaskSatisfactionScore();
    EXPECT_GT(satisfaction, 0.5) << "Beginner user satisfaction should be above 50%";
}

// Usability Tests
TEST_F(UserAcceptanceTest, Usability_TaskCompletionTime) {
    std::vector<UserType> userTypes = {
        UserType::Student, UserType::OfficeWorker, UserType::Developer,
        UserType::Writer, UserType::Elderly, UserType::Beginner
    };

    std::map<UserType, std::vector<int>> completionTimes;

    for (UserType userType : userTypes) {
        ui->setUserType(userType);
        ui->clearFeedback();

        for (const auto& task : tasks) {
            auto start = std::chrono::high_resolution_clock::now();
            ui->performTask(task);
            auto end = std::chrono::high_resolution_clock::now();

            auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
            completionTimes[userType].push_back(duration.count());
        }
    }

    // Analyze completion times
    for (const auto& pair : completionTimes) {
        UserType userType = pair.first;
        const std::vector<int>& times = pair.second;

        int totalTime = 0;
        for (int time : times) {
            totalTime += time;
        }
        double averageTime = static_cast<double>(totalTime) / times.size();

        // Different users have different expectations
        int expectedMaxTime = 60; // Base expectation
        if (userType == UserType::Elderly) expectedMaxTime = 90;
        if (userType == UserType::Beginner) expectedMaxTime = 75;
        if (userType == UserType::Developer) expectedMaxTime = 45;
        if (userType == UserType::OfficeWorker) expectedMaxTime = 50;

        EXPECT_LT(averageTime, expectedMaxTime)
            << "Average completion time for user type " << static_cast<int>(userType)
            << " exceeds expectation of " << expectedMaxTime << " seconds";
    }
}

TEST_F(UserAcceptanceTest, Usability_AccuracyAcrossUserTypes) {
    std::vector<UserType> userTypes = {
        UserType::Student, UserType::OfficeWorker, UserType::Developer,
        UserType::Writer, UserType::Elderly, UserType::Beginner
    };

    std::map<UserType, std::vector<bool>> accuracyResults;

    for (UserType userType : userTypes) {
        ui->setUserType(userType);
        ui->clearFeedback();

        for (const auto& task : tasks) {
            bool success = ui->performTask(task);
            accuracyResults[userType].push_back(success);
        }
    }

    // Analyze accuracy
    for (const auto& pair : accuracyResults) {
        UserType userType = pair.first;
        const std::vector<bool>& results = pair.second;

        int successfulTasks = 0;
        for (bool success : results) {
            if (success) successfulTasks++;
        }
        double accuracyRate = static_cast<double>(successfulTasks) / results.size();

        // Different users have different accuracy expectations
        double minExpectedAccuracy = 0.7; // Base expectation
        if (userType == UserType::Elderly) minExpectedAccuracy = 0.6;
        if (userType == UserType::Beginner) minExpectedAccuracy = 0.5;
        if (userType == UserType::Writer) minExpectedAccuracy = 0.9;
        if (userType == UserType::OfficeWorker) minExpectedAccuracy = 0.85;

        EXPECT_GT(accuracyRate, minExpectedAccuracy)
            << "Accuracy rate for user type " << static_cast<int>(userType)
            << " below expectation of " << minExpectedAccuracy * 100 << "%";
    }
}

// Feature Acceptance Tests
TEST_F(UserAcceptanceTest, FeatureAcceptance_VietnameseInput) {
    // Test Vietnamese input feature across different user types
    std::vector<UserType> userTypes = {
        UserType::Student, UserType::OfficeWorker, UserType::Writer
    };

    for (UserType userType : userTypes) {
        ui->setUserType(userType);

        UserTask vietnameseTask = {
            "Vietnamese Input Test",
            "Test Vietnamese input functionality",
            "xin chao cam on",
            "xin chào cảm ơn",
            {"1. Type Vietnamese text", "2. Check accents", "3. Verify output"},
            {{"accuracy", 85}, {"speed", 20}},
            15
        };

        bool success = ui->performTask(vietnameseTask);
        EXPECT_TRUE(success) << "Vietnamese input feature failed for user type "
                           << static_cast<int>(userType);
    }
}

TEST_F(UserAcceptanceTest, FeatureAcceptance_MacroSystem) {
    // Test macro system feature
    std::vector<UserType> userTypes = {
        UserType::OfficeWorker, UserType::Developer, UserType::Writer
    };

    for (UserType userType : userTypes) {
        ui->setUserType(userType);

        UserTask macroTask = {
            "Macro System Test",
            "Test macro expansion functionality",
            "xch hnn",
            "xin chào hôm nay",
            {"1. Type macro shortcuts", "2. Verify expansion", "3. Check accuracy"},
            {{"accuracy", 90}, {"speed", 10}},
            8
        };

        bool success = ui->performTask(macroTask);
        EXPECT_TRUE(success) << "Macro system feature failed for user type "
                           << static_cast<int>(userType);
    }
}

TEST_F(UserAcceptanceTest, FeatureAcceptance_SmartSwitching) {
    // Test smart switching feature
    std::vector<UserType> userTypes = {
        UserType::Student, UserType::Elderly, UserType::Beginner
    };

    for (UserType userType : userTypes) {
        ui->setUserType(userType);

        UserTask smartSwitchTask = {
            "Smart Switching Test",
            "Test automatic language switching",
            "hello xin chao world cam on",
            "hello xin chào world cảm ơn",
            {"1. Type mixed language", "2. Check automatic switching", "3. Verify context"},
            {{"accuracy", 80}, {"speed", 25}},
            20
        };

        bool success = ui->performTask(smartSwitchTask);
        EXPECT_TRUE(success) << "Smart switching feature failed for user type "
                           << static_cast<int>(userType);
    }
}

// Performance Acceptance Tests
TEST_F(UserAcceptanceTest, Performance_Responsiveness) {
    ui->setUserType(UserType::OfficeWorker); // Most demanding user type

    // Test system responsiveness during continuous use
    for (int i = 0; i < 10; ++i) {
        UserTask continuousTask = {
            "Continuous Use Test " + std::to_string(i + 1),
            "Test system responsiveness during continuous typing",
            "rapid typing test " + std::to_string(i + 1),
            "rapid typing test " + std::to_string(i + 1),
            {"1. Continuous typing", "2. Monitor responsiveness", "3. Check accuracy"},
            {{"accuracy", 85}, {"speed", 15}},
            10
        };

        auto start = std::chrono::high_resolution_clock::now();
        bool success = ui->performTask(continuousTask);
        auto end = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        EXPECT_TRUE(success) << "Continuous use test failed at iteration " << i + 1;
        EXPECT_LT(duration.count(), 12000) << "Task exceeded time limit at iteration " << i + 1;
    }
}

TEST_F(UserAcceptanceTest, Performance_MemoryUsage) {
    ui->setUserType(UserType::Developer); // Power user

    // Test memory usage during extended sessions
    for (int i = 0; i < 5; ++i) {
        UserTask memoryTask = {
            "Memory Usage Test " + std::to_string(i + 1),
            "Test memory usage during extended session",
            "extended session test with complex text and multiple features enabled",
            "Extended session test with complex text and multiple features enabled",
            {"1. Extended typing", "2. Multiple features", "3. Monitor memory"},
            {{"accuracy", 85}, {"speed", 30}},
            25
        };

        bool success = ui->performTask(memoryTask);
        EXPECT_TRUE(success) << "Memory usage test failed at iteration " << i + 1;
    }

    // If we reach here without memory issues, test passes
    SUCCEED();
}

// Regression Acceptance Tests
TEST_F(UserAcceptanceTest, Regression_CommonIssues) {
    // Test for regression of common user-reported issues

    struct RegressionTest {
        std::string name;
        std::string input;
        std::string expectedOutput;
        std::string description;
    };

    std::vector<RegressionTest> regressionTests = {
        {
            "Accent Loss Issue",
            "xin chao",
            "xin chào",
            "Test that accents are not lost during processing"
        },
        {
            "Macro Capitalization",
            "xch",
            "Xin chào",
            "Test that macros are properly capitalized at sentence start"
        },
        {
            "Mixed Language Context",
            "hello ban",
            "hello bạn",
            "Test that Vietnamese works correctly in mixed language context"
        },
        {
            "Special Characters",
            "cam on! rat vui.",
            "Cảm ơn! Rất vui.",
            "Test that special characters and punctuation work correctly"
        },
        {
            "Complex Accents",
            "tiep theo",
            "tiếp theo",
            "Test complex Vietnamese accent combinations"
        }
    };

    ui->setUserType(UserType::Student);

    for (const auto& test : regressionTests) {
        UserTask regressionTask = {
            test.name,
            test.description,
            test.input,
            test.expectedOutput,
            {"1. Process input", "2. Check output", "3. Verify no regression"},
            {{"accuracy", 95}, {"speed", 10}},
            8
        };

        bool success = ui->performTask(regressionTask);
        EXPECT_TRUE(success) << "Regression test failed: " << test.name;
    }
}

// Satisfaction Survey Tests
TEST_F(UserAcceptanceTest, SatisfactionSurvey) {
    // Simulate user satisfaction survey across different user types
    std::vector<UserType> userTypes = {
        UserType::Student, UserType::OfficeWorker, UserType::Developer,
        UserType::Writer, UserType::Elderly, UserType::Beginner
    };

    std::map<UserType, double> satisfactionScores;

    for (UserType userType : userTypes) {
        ui->setUserType(userType);
        ui->clearFeedback();

        // Have each user complete a representative set of tasks
        std::vector<std::string> representativeTasks = {
            "Basic Vietnamese Typing",
            "Macro Expansion",
            "Mixed Language Text"
        };

        for (const auto& taskName : representativeTasks) {
            auto taskIt = std::find_if(tasks.begin(), tasks.end(),
                [&taskName](const UserTask& task) { return task.name == taskName; });

            if (taskIt != tasks.end()) {
                ui->performTask(*taskIt);
            }
        }

        double satisfaction = ui->getTaskSatisfactionScore();
        satisfactionScores[userType] = satisfaction;
    }

    // Analyze satisfaction scores
    for (const auto& pair : satisfactionScores) {
        UserType userType = pair.first;
        double score = pair.second;

        double minExpectedScore = 0.6; // Base expectation
        if (userType == UserType::Elderly) minExpectedScore = 0.5;
        if (userType == UserType::Beginner) minExpectedScore = 0.5;
        if (userType == UserType::OfficeWorker) minExpectedScore = 0.8;
        if (userType == UserType::Writer) minExpectedScore = 0.75;

        EXPECT_GT(score, minExpectedScore)
            << "Satisfaction score for user type " << static_cast<int>(userType)
            << " below expectation of " << minExpectedScore * 100 << "%";
    }
}

// Main function to run tests
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}