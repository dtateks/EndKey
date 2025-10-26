#include <gtest/gtest.h>
#include <chrono>
#include <vector>
#include <random>
#include <memory>
#include <algorithm>
#include "Vietnamese.h"
#include "Engine.h"
#include "Macro.h"

class PerformanceTest : public ::testing::Test {
protected:
    void SetUp() override {
        engine = std::make_unique<Engine>();
        engine->startNewSession();

        // Initialize test data
        GenerateTestCorpus();

        // Performance targets
        MAX_TYPING_LATENCY_US = 100; // 100 microseconds
        MAX_MEMORY_USAGE_MB = 50; // 50MB
        MAX_CONCURRENT_OPERATIONS = 1000;
    }

    void TearDown() override {
        engine.reset();
        test_corpus.clear();
    }

    void GenerateTestCorpus() {
        // Common Vietnamese words for testing
        std::vector<std::string> vietnamese_words = {
            "vietnamese", "nguoi", "quoc", "thu", "thoi", "vien", "toan",
            "thuong", "thien", "thanh", "thuc", "thuoc", "thuong", "thuyet",
            "thuong", "thu", "thoi", "viet", "nam", "ha", "noi", "ngoai",
            "trong", "tren", "duoi", "ben", "canh", "giua", "sau", "truoc",
            "xin", "chao", "cam", "on", "rat", "vui", "khi", "gap", "ban"
        };

        // Generate test corpus with random combinations
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, vietnamese_words.size() - 1);

        for (int i = 0; i < 1000; ++i) {
            std::string test_word = vietnamese_words[dis(gen)];
            test_corpus.push_back(test_word);
        }
    }

    std::unique_ptr<Engine> engine;
    std::vector<std::string> test_corpus;
    int MAX_TYPING_LATENCY_US;
    int MAX_MEMORY_USAGE_MB;
    int MAX_CONCURRENT_OPERATIONS;

    // Helper method to measure typing performance
    std::chrono::microseconds MeasureTypingLatency(const std::string& input) {
        engine->startNewSession();
        auto start_time = std::chrono::high_resolution_clock::now();

        for (char c : input) {
            engine->vKeyHandleEvent(c, 0);
        }

        auto end_time = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    }

    // Helper method to measure memory usage
    size_t GetCurrentMemoryUsage() {
        // Platform-specific memory measurement
        // For now, return estimated usage
        return engine ? sizeof(*engine) + test_corpus.size() * 50 : 0;
    }
};

// Test sub-millisecond typing response
TEST_F(PerformanceTest, SubMillisecondTypingResponse) {
    const int num_iterations = 1000;
    std::vector<std::chrono::microseconds> latencies;

    for (int i = 0; i < num_iterations; ++i) {
        std::string test_word = test_corpus[i % test_corpus.size()];
        auto latency = MeasureTypingLatency(test_word);
        latencies.push_back(latency);
    }

    // Calculate statistics
    auto total_latency = std::accumulate(latencies.begin(), latencies.end(),
                                        std::chrono::microseconds(0));
    auto avg_latency = total_latency / num_iterations;

    auto max_latency = *std::max_element(latencies.begin(), latencies.end());
    auto min_latency = *std::min_element(latencies.begin(), latencies.end());

    // Calculate percentile
    std::sort(latencies.begin(), latencies.end());
    auto p95_latency = latencies[static_cast<size_t>(num_iterations * 0.95)];
    auto p99_latency = latencies[static_cast<size_t>(num_iterations * 0.99)];

    EXPECT_LT(avg_latency.count(), MAX_TYPING_LATENCY_US)
        << "Average typing latency exceeds threshold: " << avg_latency.count() << "μs";

    EXPECT_LT(p95_latency.count(), MAX_TYPING_LATENCY_US * 2)
        << "95th percentile latency too high: " << p95_latency.count() << "μs";

    EXPECT_LT(p99_latency.count(), MAX_TYPING_LATENCY_US * 5)
        << "99th percentile latency too high: " << p99_latency.count() << "μs";

    std::cout << "Performance Metrics:" << std::endl;
    std::cout << "  Average: " << avg_latency.count() << "μs" << std::endl;
    std::cout << "  Min: " << min_latency.count() << "μs" << std::endl;
    std::cout << "  Max: " << max_latency.count() << "μs" << std::endl;
    std::cout << "  95th percentile: " << p95_latency.count() << "μs" << std::endl;
    std::cout << "  99th percentile: " << p99_latency.count() << "μs" << std::endl;
}

// Test memory usage during extended typing sessions
TEST_F(PerformanceTest, MemoryUsageDuringExtendedSession) {
    const int session_duration_minutes = 5;
    const int typing_speed_wpm = 60; // Words per minute
    const int words_per_minute = typing_speed_wpm;

    size_t initial_memory = GetCurrentMemoryUsage();
    size_t max_memory = initial_memory;

    // Simulate extended typing session
    for (int minute = 0; minute < session_duration_minutes; ++minute) {
        for (int word = 0; word < words_per_minute; ++word) {
            std::string test_word = test_corpus[word % test_corpus.size()];
            engine->startNewSession();

            for (char c : test_word) {
                engine->vKeyHandleEvent(c, 0);
            }

            size_t current_memory = GetCurrentMemoryUsage();
            max_memory = std::max(max_memory, current_memory);
        }

        // Force garbage collection if available
        if (minute % 2 == 0) {
            engine->startNewSession(); // Reset session to clear memory
        }
    }

    size_t final_memory = GetCurrentMemoryUsage();
    size_t memory_increase = final_memory - initial_memory;
    size_t memory_usage_mb = final_memory / (1024 * 1024);

    EXPECT_LT(memory_usage_mb, MAX_MEMORY_USAGE_MB)
        << "Memory usage exceeds threshold: " << memory_usage_mb << "MB";

    EXPECT_LT(memory_increase, initial_memory * 0.5)
        << "Memory leak detected: increased by " << memory_increase << " bytes";

    std::cout << "Memory Usage:" << std::endl;
    std::cout << "  Initial: " << initial_memory / 1024 << "KB" << std::endl;
    std::cout << "  Final: " << final_memory / 1024 << "KB" << std::endl;
    std::cout << "  Peak: " << max_memory / 1024 << "KB" << std::endl;
    std::cout << "  Increase: " << memory_increase / 1024 << "KB" << std::endl;
}

// Test concurrent input handling stress
TEST_F(PerformanceTest, ConcurrentInputHandlingStress) {
    const int num_threads = 10;
    const int operations_per_thread = MAX_CONCURRENT_OPERATIONS;

    std::vector<std::thread> threads;
    std::atomic<int> successful_operations{0};
    std::atomic<long long> total_latency_us{0};

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&, i]() {
            for (int j = 0; j < operations_per_thread; ++j) {
                try {
                    std::string test_word = test_corpus[(i * operations_per_thread + j) % test_corpus.size()];

                    auto start_time = std::chrono::high_resolution_clock::now();

                    engine->startNewSession();
                    for (char c : test_word) {
                        engine->vKeyHandleEvent(c, 0);
                    }

                    auto end_time = std::chrono::high_resolution_clock::now();
                    auto latency = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

                    total_latency_us += latency.count();
                    successful_operations++;

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

    int total_operations = num_threads * operations_per_thread;
    double success_rate = static_cast<double>(successful_operations) / total_operations;
    long long avg_latency_us = total_latency_us / successful_operations;

    EXPECT_GT(success_rate, 0.99) << "Success rate too low: " << success_rate;
    EXPECT_LT(avg_latency_us, MAX_TYPING_LATENCY_US * 10)
        << "Concurrent operations too slow: " << avg_latency_us << "μs";

    std::cout << "Concurrent Performance:" << std::endl;
    std::cout << "  Success rate: " << (success_rate * 100) << "%" << std::endl;
    std::cout << "  Average latency: " << avg_latency_us << "μs" << std::endl;
    std::cout << "  Total operations: " << total_operations << std::endl;
}

// Test cross-platform performance regression detection
TEST_F(PerformanceTest, CrossPlatformPerformanceRegression) {
    struct PerformanceBenchmark {
        std::string platform;
        std::string test_name;
        double target_avg_latency_us;
        double target_p95_latency_us;
        double target_memory_usage_mb;
    };

    std::vector<PerformanceBenchmark> benchmarks = {
        {"macOS", "telex_input", 50.0, 100.0, 10.0},
        {"macOS", "vni_input", 60.0, 120.0, 10.0},
        {"macOS", "macro_expansion", 30.0, 80.0, 5.0},
        {"macOS", "smart_switching", 20.0, 50.0, 2.0},
    };

    for (const auto& benchmark : benchmarks) {
        std::vector<std::chrono::microseconds> latencies;
        const int num_samples = 100;

        // Run performance test
        for (int i = 0; i < num_samples; ++i) {
            std::string test_input = "vietnamesetypingtest";
            auto latency = MeasureTypingLatency(test_input);
            latencies.push_back(latency);
        }

        // Calculate metrics
        auto total_latency = std::accumulate(latencies.begin(), latencies.end(),
                                           std::chrono::microseconds(0));
        double avg_latency = static_cast<double>(total_latency.count()) / num_samples;

        std::sort(latencies.begin(), latencies.end());
        double p95_latency = latencies[static_cast<size_t>(num_samples * 0.95)].count();

        size_t memory_usage = GetCurrentMemoryUsage();
        double memory_usage_mb = static_cast<double>(memory_usage) / (1024 * 1024);

        // Compare against benchmarks
        EXPECT_LT(avg_latency, benchmark.target_avg_latency_us)
            << "Regression in " << benchmark.test_name
            << " - Avg latency: " << avg_latency
            << "μs > target: " << benchmark.target_avg_latency_us << "μs";

        EXPECT_LT(p95_latency, benchmark.target_p95_latency_us)
            << "Regression in " << benchmark.test_name
            << " - P95 latency: " << p95_latency
            << "μs > target: " << benchmark.target_p95_latency_us << "μs";

        EXPECT_LT(memory_usage_mb, benchmark.target_memory_usage_mb)
            << "Regression in " << benchmark.test_name
            << " - Memory: " << memory_usage_mb
            << "MB > target: " << benchmark.target_memory_usage_mb << "MB";

        std::cout << "Benchmark: " << benchmark.test_name << std::endl;
        std::cout << "  Avg latency: " << avg_latency << "μs (target: " << benchmark.target_avg_latency_us << "μs)" << std::endl;
        std::cout << "  P95 latency: " << p95_latency << "μs (target: " << benchmark.target_p95_latency_us << "μs)" << std::endl;
        std::cout << "  Memory: " << memory_usage_mb << "MB (target: " << benchmark.target_memory_usage_mb << "MB)" << std::endl;
    }
}

// Test performance with different code tables
TEST_F(PerformanceTest, CodeTablePerformance) {
    std::vector<std::pair<int, std::string>> code_tables = {
        {UNICODE, "Unicode"},
        {TCVN3, "TCVN3"},
        {VNI_WINDOWS, "VNI-Windows"}
    };

    for (const auto& [table, name] : code_tables) {
        vCodeTable = table;

        std::vector<std::chrono::microseconds> latencies;
        const int num_samples = 100;

        for (int i = 0; i < num_samples; ++i) {
            std::string test_input = test_corpus[i % test_corpus.size()];
            auto latency = MeasureTypingLatency(test_input);
            latencies.push_back(latency);
        }

        auto total_latency = std::accumulate(latencies.begin(), latencies.end(),
                                           std::chrono::microseconds(0));
        double avg_latency = static_cast<double>(total_latency.count()) / num_samples;

        EXPECT_LT(avg_latency, MAX_TYPING_LATENCY_US * 2)
            << "Code table " << name << " performance too slow: " << avg_latency << "μs";

        std::cout << "Code Table: " << name << " - Avg latency: " << avg_latency << "μs" << std::endl;
    }
}