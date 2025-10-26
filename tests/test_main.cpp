#include <gtest/gtest.h>
#include <chrono>
#include <iostream>
#include "Engine.h"

class EndKeyTestEnvironment : public ::testing::Environment {
public:
    void SetUp() override {
        std::cout << "\n=== EndKey Test Suite Initialization ===" << std::endl;
        std::cout << "Starting Vietnamese Input Method Testing..." << std::endl;

        // Initialize performance monitoring
        start_time = std::chrono::high_resolution_clock::now();
    }

    void TearDown() override {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

        std::cout << "\n=== Test Suite Completed ===" << std::endl;
        std::cout << "Total execution time: " << duration.count() << " ms" << std::endl;

        // Generate test summary
        GenerateTestSummary();
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;

    void GenerateTestSummary() {
        std::cout << "\n=== Test Summary ===" << std::endl;
        std::cout << "Total tests: " << ::testing::UnitTest::GetInstance()->total_test_count() << std::endl;
        std::cout << "Successful: " << ::testing::UnitTest::GetInstance()->successful_test_count() << std::endl;
        std::cout << "Failed: " << ::testing::UnitTest::GetInstance()->failed_test_count() << std::endl;
        std::cout << "Skipped: " << ::testing::UnitTest::GetInstance()->disabled_test_count() << std::endl;
    }
};

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    // Add test environment
    ::testing::AddGlobalTestEnvironment(new EndKeyTestEnvironment);

    // Parse custom arguments
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--performance") {
            std::cout << "Running performance-focused tests..." << std::endl;
        } else if (std::string(argv[i]) == "--vietnamese") {
            std::cout << "Running Vietnamese input validation tests..." << std::endl;
        }
    }

    return RUN_ALL_TESTS();
}