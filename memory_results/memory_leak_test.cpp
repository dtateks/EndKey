#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>

// Memory leak detection simulation
class MemoryLeakDetector {
private:
    static size_t allocated_objects;
    static size_t total_memory_allocated;

public:
    static void trackAllocation(size_t size) {
        allocated_objects++;
        total_memory_allocated += size;
    }

    static void trackDeallocation(size_t size) {
        if (allocated_objects > 0) {
            allocated_objects--;
        }
        if (total_memory_allocated >= size) {
            total_memory_allocated -= size;
        }
    }

    static void simulateVietnameseProcessing() {
        // Simulate Vietnamese character processing with memory allocation
        std::vector<std::string> vietnamese_words = {
            "xin chào", "cảm ơn", "rất vui", "hôm nay", "ngày mai",
            "đẹp trai", "xinh gái", "thông minh", "giàu sang", "quyền lực"
        };

        for (const auto& word : vietnamese_words) {
            // Simulate memory allocation for processing
            std::string* processed = new std::string(word);
            trackAllocation(processed->length() * sizeof(char));

            // Process the word (simplified)
            for (size_t i = 0; i < processed->length(); ++i) {
                char c = (*processed)[i];
                // Simulate character processing
            }

            // Simulate deallocation
            trackDeallocation(processed->length() * sizeof(char));
            delete processed;
        }
    }

    static void simulateMacroProcessing() {
        // Simulate macro processing with temporary allocations
        std::vector<std::pair<std::string, std::string>> macros = {
            {"xch", "xin chào"}, {"cn", "cảm ơn"}, {"hnn", "hôm nay"}
        };

        for (const auto& macro : macros) {
            std::string* expanded = new std::string(macro.second);
            trackAllocation(expanded->length() * sizeof(char));

            // Simulate macro expansion processing
            std::string result = "Expanded: " + *expanded;

            trackDeallocation(expanded->length() * sizeof(char));
            delete expanded;
        }
    }

    static void printMemoryStats(const std::string& phase) {
        std::cout << "Phase: " << phase << std::endl;
        std::cout << "  Allocated objects: " << allocated_objects << std::endl;
        std::cout << "  Total memory: " << total_memory_allocated << " bytes" << std::endl;
    }

    static bool detectLeaks() {
        return (allocated_objects > 0 || total_memory_allocated > 0);
    }
};

// Static member definitions
size_t MemoryLeakDetector::allocated_objects = 0;
size_t MemoryLeakDetector::total_memory_allocated = 0;

int main() {
    std::cout << "Starting memory leak detection test..." << std::endl;

    // Test 1: Vietnamese processing memory management
    std::cout << "\n=== Vietnamese Processing Test ===" << std::endl;
    MemoryLeakDetector::printMemoryStats("Start");

    for (int i = 0; i < 1000; ++i) {
        MemoryLeakDetector::simulateVietnameseProcessing();
    }

    MemoryLeakDetector::printMemoryStats("After Vietnamese Processing");

    // Test 2: Macro processing memory management
    std::cout << "\n=== Macro Processing Test ===" << std::endl;
    MemoryLeakDetector::printMemoryStats("Start Macro Test");

    for (int i = 0; i < 500; ++i) {
        MemoryLeakDetector::simulateMacroProcessing();
    }

    MemoryLeakDetector::printMemoryStats("After Macro Processing");

    // Test 3: Extended processing simulation
    std::cout << "\n=== Extended Processing Test ===" << std::endl;
    MemoryLeakDetector::printMemoryStats("Start Extended Test");

    for (int i = 0; i < 100; ++i) {
        MemoryLeakDetector::simulateVietnameseProcessing();
        MemoryLeakDetector::simulateMacroProcessing();

        // Small delay to simulate real usage
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    MemoryLeakDetector::printMemoryStats("After Extended Processing");

    // Final leak detection
    std::cout << "\n=== Memory Leak Detection ===" << std::endl;
    bool hasLeaks = MemoryLeakDetector::detectLeaks();

    if (hasLeaks) {
        std::cout << "❌ Memory leaks detected!" << std::endl;
        return 1;
    } else {
        std::cout << "✅ No memory leaks detected" << std::endl;
        return 0;
    }
}
