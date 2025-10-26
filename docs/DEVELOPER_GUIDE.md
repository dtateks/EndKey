# EndKey Developer Guide

## Table of Contents
1. [Getting Started](#getting-started)
2. [Development Environment Setup](#development-environment-setup)
3. [Coding Standards](#coding-standards)
4. [Architecture Guidelines](#architecture-guidelines)
5. [Component Development](#component-development)
6. [Testing Strategies](#testing-strategies)
7. [Build and Deployment](#build-and-deployment)
8. [Debugging and Troubleshooting](#debugging-and-troubleshooting)
9. [Contribution Guidelines](#contribution-guidelines)
10. [Best Practices](#best-practices)

## Getting Started

### Prerequisites

**Required Tools:**
- Xcode 12+ (for macOS development)
- CMake 3.15+ (for cross-platform testing)
- Git (version control)
- Google Test (testing framework)

**Development Environment:**
- macOS 10.15+ (Catalina or later)
- Xcode Command Line Tools
- C++14/17 compatible compiler

### Repository Structure

```
EndKey/
├── Sources/EndKey/
│   ├── engine/                    # Core engine components
│   │   ├── core/                 # Core engine infrastructure
│   │   ├── interfaces/           # Component interfaces
│   │   ├── platforms/            # Platform-specific code
│   │   ├── EngineCore.h/cpp      # Main engine coordinator
│   │   ├── TypingEngine.h/cpp    # Input processing
│   │   ├── SpellingEngine.h/cpp  # Vietnamese orthography
│   │   ├── MacroEngine.h/cpp     # Text expansion
│   │   └── ...                   # Other engine components
│   ├── macOS/                    # macOS-specific implementation
│   │   ├── ModernKey/           # Main application
│   │   ├── EndKeyHelper/        # Helper utilities
│   │   └── EndKeyUpdate/        # Auto-updater
│   └── EndKey.xcodeproj         # Xcode project
├── tests/                        # Test suite
│   ├── unit/                    # Unit tests
│   ├── integration/             # Integration tests
│   ├── performance/             # Performance benchmarks
│   └── regression/              # Regression tests
├── docs/                        # Documentation
├── scripts/                     # Build and utility scripts
└── tools/                       # Development tools
```

### Quick Start

1. **Clone the repository:**
```bash
git clone https://github.com/username/EndKey.git
cd EndKey
```

2. **Initialize submodules:**
```bash
git submodule update --init --recursive
```

3. **Build the project:**
```bash
# Build with Xcode
xcodebuild -project Sources/EndKey/macOS/EndKey.xcodeproj \
  -target EndKey \
  -configuration Debug \
  CODE_SIGNING_ALLOWED=NO \
  build

# Or use the convenience script
./scripts/build.sh
```

4. **Run tests:**
```bash
./scripts/run_tests.sh
```

## Development Environment Setup

### Xcode Configuration

1. **Install Xcode:**
   - Download from Mac App Store
   - Install Command Line Tools: `xcode-select --install`

2. **Configure Project:**
   - Open `Sources/EndKey/macOS/EndKey.xcodeproj`
   - Set team signing in project settings
   - Configure build settings

3. **Enable Sandbox:**
   - Go to Build Settings → Capabilities
   - Enable App Sandbox
   - Add required entitlements:
     - Accessibility
     - Input Monitoring
     - File System Read/Write

### CMake Setup (Cross-Platform Testing)

1. **Install CMake:**
```bash
brew install cmake
```

2. **Configure Test Build:**
```bash
cd tests
mkdir build && cd build
cmake ..
make
```

3. **Run Test Suite:**
```bash
./run_tests
```

### Development Tools

**Recommended Extensions/Tools:**
- CLion (optional, for C++ development)
- GitKraken (Git GUI)
- Instruments (performance analysis)
- Xcode Instruments (profiling)

## Coding Standards

### Naming Conventions

**Classes and Structs:**
```cpp
// PascalCase for classes and structs
class TypingEngine;
class VietnameseProcessor;
struct EngineConfig;
struct MacroEntry;
```

**Functions and Methods:**
```cpp
// camelCase for functions
void processKeyEvent();
const std::string& getConfiguration() const;
bool isVietnameseMode() const;
```

**Variables:**
```cpp
// camelCase for local and member variables
int currentIndex;
bool isInitialized;
std::vector<Uint32> typingWord;
```

**Constants:**
```cpp
// UPPER_SNAKE_CASE for constants
static constexpr size_t MAX_BUFFER_SIZE = 1024;
static constexpr Uint8 TONE_ACUTE = 1;
static const std::string DEFAULT_CONFIG_PATH = "/etc/endkey/config.json";
```

**Namespaces:**
```cpp
namespace EndKey {
    namespace Engine {
        class TypingEngine;
        namespace Internal {
            // Internal implementation details
        }
    }
}
```

### Code Formatting

**Indentation:**
- Use 4 spaces (no tabs)
- Align braces consistently

**Line Length:**
- Maximum 100 characters per line
- Break long function calls logically

**Brace Style:**
```cpp
// Allman style for classes and functions
class EngineCore
{
public:
    EngineCore();
    ~EngineCore();

    void processKeyEvent()
    {
        // Implementation
    }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// K&R style for control structures
if (condition) {
    // Do something
} else {
    // Do something else
}
```

### Documentation Standards

**Doxygen Comments:**
```cpp
/**
 * @brief Processes a keyboard event and converts it to Vietnamese characters
 *
 * This method handles the core input processing for Vietnamese text input,
 * applying tone marks, checking spelling, and expanding macros as needed.
 *
 * @param event The keyboard event information
 * @param state Current keyboard modifier state
 * @param data Raw key code from the system
 * @param capsStatus Caps lock status (optional)
 * @param otherControlKey Other control key status (optional)
 * @return EngineHookState Structure containing processed output and metadata
 *
 * @example
 * vKeyEvent event = {KEY_DOWN, 0};
 * vKeyEventState state = {0, 0, 0, 0};
 * EngineHookState result = engine.processKeyEvent(event, state, 'a');
 */
EngineHookState processKeyEvent(const vKeyEvent& event,
                               const vKeyEventState& state,
                               const Uint16& data,
                               const Uint8& capsStatus = 0,
                               const bool& otherControlKey = false);
```

**File Headers:**
```cpp
//
//  TypingEngine.h
//  EndKey
//
//  Created by EndKey Refactoring on 10/26/25.
//  Copyright © 2025 Tuyen Mai. All rights reserved.
//

#ifndef TypingEngine_h
#define TypingEngine_h

// ... implementation
#endif /* TypingEngine_h */
```

### Error Handling

**Exception Safety:**
```cpp
class EngineCore {
public:
    // Strong exception guarantee
    bool initialize(const EngineConfig& config) noexcept {
        try {
            return initializeImpl(config);
        } catch (const std::exception& e) {
            logError("Initialization failed: " + std::string(e.what()));
            return false;
        } catch (...) {
            logError("Unknown initialization error");
            return false;
        }
    }

private:
    bool initializeImpl(const EngineConfig& config);
};
```

**RAII Pattern:**
```cpp
class ResourceManager {
private:
    std::unique_ptr<uint8_t[]> buffer_;
    size_t bufferSize_;

public:
    ResourceManager(size_t size) : bufferSize_(size) {
        buffer_ = std::make_unique<uint8_t[]>(size);
        if (!buffer_) {
            throw std::bad_alloc();
        }
    }

    ~ResourceManager() {
        // Automatic cleanup via unique_ptr
    }

    // Prevent copying
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    // Allow moving
    ResourceManager(ResourceManager&& other) noexcept
        : buffer_(std::move(other.buffer_)), bufferSize_(other.bufferSize_) {
        other.bufferSize_ = 0;
    }
};
```

## Architecture Guidelines

### Component Design Principles

**Single Responsibility:**
- Each class should have one, and only one, reason to change
- Example: `TypingEngine` only handles input processing, not configuration

**Interface Segregation:**
- Create focused interfaces that clients depend on
- Example: `IInputProcessor` for input handling, `ISpellChecker` for spelling

**Dependency Inversion:**
- High-level modules should not depend on low-level modules
- Both should depend on abstractions

### Design Patterns Used

**Pimpl Pattern:**
```cpp
class EngineCore {
public:
    EngineCore();
    ~EngineCore();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;  // Hide implementation details
};
```

**Factory Pattern:**
```cpp
class EngineFactory {
public:
    static std::unique_ptr<IInputProcessor> createTypingEngine(InputType type) {
        switch (type) {
            case InputType::Telex:
                return std::make_unique<TelexTypingEngine>();
            case InputType::VNI:
                return std::make_unique<VNITypingEngine>();
            default:
                return nullptr;
        }
    }
};
```

**Observer Pattern:**
```cpp
class ConfigurationObserver {
public:
    virtual void onConfigurationChanged(const EngineConfig& newConfig) = 0;
};

class ConfigurationManager {
private:
    std::vector<ConfigurationObserver*> observers_;

public:
    void addObserver(ConfigurationObserver* observer) {
        observers_.push_back(observer);
    }

    void updateConfiguration(const EngineConfig& config) {
        currentConfig_ = config;
        for (auto* observer : observers_) {
            observer->onConfigurationChanged(config);
        }
    }
};
```

### Memory Management Guidelines

**Smart Pointers:**
```cpp
// Use unique_ptr for exclusive ownership
std::unique_ptr<TypingEngine> typingEngine_ = std::make_unique<TypingEngine>();

// Use shared_ptr for shared ownership
std::shared_ptr<ConfigurationManager> configManager_ = std::make_shared<ConfigurationManager>();

// Use weak_ptr to break circular dependencies
std::weak_ptr<ParentClass> parentWeakPtr_;
```

**Resource Management:**
```cpp
class BufferManager {
private:
    uint8_t* buffer_;
    size_t size_;

public:
    BufferManager(size_t size) : size_(size) {
        buffer_ = static_cast<uint8_t*>(std::malloc(size));
        if (!buffer_) {
            throw std::bad_alloc();
        }
    }

    ~BufferManager() {
        if (buffer_) {
            std::free(buffer_);
        }
    }

    // Rule of Five
    BufferManager(const BufferManager&) = delete;
    BufferManager& operator=(const BufferManager&) = delete;
    BufferManager(BufferManager&& other) noexcept;
    BufferManager& operator=(BufferManager&& other) noexcept;
};
```

## Component Development

### Creating New Components

1. **Define Interface:**
```cpp
// interfaces/INewComponent.h
class INewComponent {
public:
    virtual ~INewComponent() = default;
    virtual bool initialize(const ComponentConfig& config) = 0;
    virtual void process(const InputData& data) = 0;
    virtual void shutdown() = 0;
    virtual bool isInitialized() const = 0;
};
```

2. **Implement Component:**
```cpp
// NewComponent.h
class NewComponent : public INewComponent {
public:
    NewComponent();
    ~NewComponent() override;

    bool initialize(const ComponentConfig& config) override;
    void process(const InputData& data) override;
    void shutdown() override;
    bool isInitialized() const override;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

3. **Register with EngineCore:**
```cpp
// EngineCore.h
class EngineCore {
private:
    std::unique_ptr<INewComponent> newComponent_;

public:
    bool initialize(const EngineConfig& config) override {
        // Initialize new component
        newComponent_ = std::make_unique<NewComponent>();
        return newComponent_->initialize(config.newComponentConfig);
    }
};
```

### Component Lifecycle

**Initialization:**
```cpp
bool Component::initialize(const ComponentConfig& config) {
    try {
        // Validate configuration
        if (!validateConfig(config)) {
            return false;
        }

        // Allocate resources
        allocateResources(config);

        // Initialize state
        resetState();

        // Register with observers
        registerObservers();

        isInitialized_ = true;
        return true;
    } catch (const std::exception& e) {
        logError("Component initialization failed: " + std::string(e.what()));
        cleanup();
        return false;
    }
}
```

**Shutdown:**
```cpp
void Component::shutdown() {
    if (!isInitialized_) {
        return;
    }

    try {
        // Unregister from observers
        unregisterObservers();

        // Save state if needed
        saveState();

        // Release resources
        releaseResources();

        isInitialized_ = false;
    } catch (const std::exception& e) {
        logError("Component shutdown error: " + std::string(e.what()));
    }
}
```

### Configuration Integration

**Configuration Structure:**
```cpp
struct ComponentConfig {
    int parameter1;
    bool parameter2;
    std::string parameter3;
    std::vector<std::string> parameterList;
};

// Add to main EngineConfig
struct EngineConfig {
    // Existing fields...
    ComponentConfig newComponentConfig;
};
```

**Configuration Validation:**
```cpp
bool validateComponentConfig(const ComponentConfig& config) {
    if (config.parameter1 < 0 || config.parameter1 > MAX_VALUE) {
        return false;
    }

    if (config.parameter3.empty()) {
        return false;
    }

    return true;
}
```

## Testing Strategies

### Test Structure

**Unit Tests:**
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
    }

    void TearDown() override {
        engine_.reset();
    }

    std::unique_ptr<EndKey::Engine::TypingEngine> engine_;
};

TEST_F(TypingEngineTest, BasicCharacterProcessing) {
    vKeyEvent event = {KEY_DOWN, 0};
    vKeyEventState state = {0, 0, 0, 0};

    engine_->processKey(event, state, 'a');
    engine_->processKey(event, state, 's');  // Should produce â

    const auto& typingState = engine_->getCurrentState();
    EXPECT_EQ(typingState.index, 1);

    Uint32 resultChar = engine_->getCharacterCode(typingState.typingWord[0]);
    EXPECT_EQ(resultChar, L'â');
}
```

**Integration Tests:**
```cpp
// tests/integration/EngineIntegrationTest.cpp
TEST(EngineIntegrationTest, FullVietnameseProcessing) {
    EndKey::Engine::EngineCore engine;

    EngineConfig config = {
        .language = 1,
        .inputType = 0,
        .checkSpelling = 1,
        .useMacro = 1
    };

    ASSERT_TRUE(engine.initialize(config));

    // Test full word processing
    vKeyEvent event = {KEY_DOWN, 0};
    vKeyEventState state = {0, 0, 0, 0};

    std::vector<Uint16> keys = {'v', 'i', 'e', 'e', 't', 's'};  // việt
    for (Uint16 key : keys) {
        engine.processKeyEvent(event, state, key);
    }

    // Verify result
    const auto& currentWord = engine.getCurrentWord();
    EXPECT_GT(currentWord.size(), 0);

    engine.shutdown();
}
```

**Performance Tests:**
```cpp
// tests/performance/PerformanceBenchmark.cpp
TEST(PerformanceBenchmark, CharacterProcessingSpeed) {
    EndKey::Engine::TypingEngine engine;

    auto start = std::chrono::high_resolution_clock::now();

    // Process 1000 characters
    for (int i = 0; i < 1000; i++) {
        vKeyEvent event = {KEY_DOWN, 0};
        vKeyEventState state = {0, 0, 0, 0};
        engine.processKey(event, state, 'a' + (i % 26));
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    // Should process at least 1000 characters per millisecond
    EXPECT_LT(duration.count(), 1000);  // Less than 1ms
}
```

### Test Utilities

**Mock Components:**
```cpp
// tests/mocks/MockSpellingEngine.h
class MockSpellingEngine : public ISpellingEngine {
public:
    MOCK_METHOD(bool, isValidVietnameseWord, (const std::vector<Uint16>& word), (const, override));
    MOCK_METHOD(ToneMarkInfo, applyToneMark, (Uint16 character, Uint8 toneType), (const, override));
    MOCK_METHOD(void, setSpellingConfig, (const SpellingConfig& config), (override));
};
```

**Test Fixtures:**
```cpp
// tests/fixtures/EngineTestFixture.h
class EngineTestFixture {
public:
    static EngineConfig createDefaultConfig() {
        return {
            .language = 1,
            .inputType = 0,
            .codeTable = 0,
            .checkSpelling = 1,
            .useModernOrthography = 1
        };
    }

    static std::vector<Uint16> createTestWord(const std::string& word) {
        return std::vector<Uint16>(word.begin(), word.end());
    }
};
```

### Running Tests

**Command Line:**
```bash
# Run all tests
./scripts/run_tests.sh

# Run specific test suite
./tests/build/unit_tests --gtest_filter="TypingEngineTest.*"

# Run with coverage
./scripts/run_coverage.sh
```

**Xcode Integration:**
1. Add test target to Xcode project
2. Configure scheme for testing
3. Use Xcode's test navigator

## Build and Deployment

### Build Scripts

**Main Build Script:**
```bash
#!/bin/bash
# scripts/build.sh

set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_TYPE="${1:-Debug}"

echo "Building EndKey ($BUILD_TYPE)..."

# Kill existing instances
killall -9 EndKey EndKeyHelper 2>/dev/null || true

# Clean build directory
rm -rf "$PROJECT_ROOT/Sources/EndKey/macOS/build/"

# Build with Xcode
xcodebuild -project "$PROJECT_ROOT/Sources/EndKey/macOS/EndKey.xcodeproj" \
  -target EndKey \
  -configuration "$BUILD_TYPE" \
  CODE_SIGNING_ALLOWED=NO \
  clean build

# Install to Applications
if [ "$BUILD_TYPE" = "Release" ]; then
    rm -rf /Applications/EndKey.app
    cp -a "$PROJECT_ROOT/Sources/EndKey/macOS/build/Release/EndKey.app" /Applications/
    codesign --force --deep --sign - /Applications/EndKey.app
    echo "EndKey installed to /Applications/EndKey.app"
fi

echo "Build completed successfully!"
```

**Test Build Script:**
```bash
#!/bin/bash
# scripts/build_tests.sh

set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
TEST_BUILD_DIR="$PROJECT_ROOT/tests/build"

echo "Building tests..."

mkdir -p "$TEST_BUILD_DIR"
cd "$TEST_BUILD_DIR"

cmake ..
make -j$(sysctl -n hw.ncpu)

echo "Tests built successfully!"
echo "Run with: $TEST_BUILD_DIR/run_tests"
```

### Configuration Management

**Debug Configuration:**
```cpp
#ifdef DEBUG
    #define LOG_DEBUG(msg) std::cout << "[DEBUG] " << msg << std::endl
    #define ASSERT(condition) assert(condition)
#else
    #define LOG_DEBUG(msg)
    #define ASSERT(condition)
#endif
```

**Release Configuration:**
```cpp
#ifndef IS_DEBUG
    #define IS_DEBUG 0
#endif

#if IS_DEBUG
    void debugLog(const std::string& message);
#else
    #define debugLog(message)
#endif
```

### Continuous Integration

**GitHub Actions Workflow:**
```yaml
# .github/workflows/build.yml
name: Build and Test

on: [push, pull_request]

jobs:
  build:
    runs-on: macos-latest

    steps:
    - uses: actions/checkout@v2

    - name: Build
      run: ./scripts/build.sh

    - name: Run Tests
      run: ./scripts/run_tests.sh

    - name: Run Coverage
      run: ./scripts/run_coverage.sh

    - name: Upload Coverage
      uses: codecov/codecov-action@v1
```

## Debugging and Troubleshooting

### Debugging Techniques

**Logging System:**
```cpp
// Logger.h
enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error
};

class Logger {
public:
    static void log(LogLevel level, const std::string& message);
    static void setLogLevel(LogLevel level);
    static void setLogFile(const std::string& filename);
};

// Usage
Logger::log(LogLevel::Debug, "Initializing typing engine");
Logger::log(LogLevel::Error, "Failed to load configuration");
```

**Memory Debugging:**
```cpp
// MemoryTracker.h
class MemoryTracker {
private:
    std::unordered_map<void*, size_t> allocations_;
    std::mutex mutex_;

public:
    void trackAllocation(void* ptr, size_t size);
    void trackDeallocation(void* ptr);
    void printLeakReport();
};

#define TRACKED_NEW(type) new type()
#define TRACKED_DELETE(ptr) delete ptr
```

**Performance Profiling:**
```cpp
// Profiler.h
class Profiler {
private:
    std::unordered_map<std::string, std::chrono::nanoseconds> measurements_;

public:
    class ScopedTimer {
    public:
        ScopedTimer(const std::string& name);
        ~ScopedTimer();
    };

    static void startMeasurement(const std::string& name);
    static void endMeasurement(const std::string& name);
    static void printReport();
};

#define PROFILE_SCOPE(name) Profiler::ScopedTimer timer(name)
```

### Common Issues

**Memory Leaks:**
1. Use Address Sanitizer: `-fsanitize=address`
2. Enable memory tracking in debug builds
3. Run leak detection tools

**Performance Issues:**
1. Profile with Instruments Time Profiler
2. Check for unnecessary allocations
3. Optimize cache usage

**Build Errors:**
1. Clean build directory: `rm -rf build/`
2. Update Xcode and command line tools
3. Check for missing dependencies

### Debugging Tools

**Xcode Debugger:**
- Set breakpoints in engine code
- Use LLDB commands for inspection
- Monitor memory usage

**Command Line Tools:**
```bash
# Valgrind for memory checking (Linux)
valgrind --leak-check=full ./endkey_test

# Address Sanitizer (macOS)
export ASAN_OPTIONS=detect_leaks=1
./endkey_test

# Performance profiling
instruments -t "Time Profiler" ./EndKey.app
```

## Contribution Guidelines

### Code Review Process

1. **Create Feature Branch:**
```bash
git checkout -b feature/new-component
```

2. **Make Changes:**
   - Follow coding standards
   - Add comprehensive tests
   - Update documentation

3. **Submit Pull Request:**
   - Clear description of changes
   - Link to related issues
   - Include test results

4. **Review Requirements:**
   - Code style compliance
   - Test coverage > 80%
   - Documentation updated
   - Performance benchmarks passed

### Commit Message Format

```
<type>(<scope>): <description>

[optional body]

[optional footer]
```

**Types:**
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `style`: Code style changes
- `refactor`: Code refactoring
- `test`: Test additions/changes
- `chore`: Build process or auxiliary changes

**Examples:**
```
feat(typing): Add support for VNI input method

- Implement VNI character processing
- Add configuration options
- Update tests and documentation

Closes #123
```

```
fix(spelling): Correct tone mark application for combined vowels

Previously, tone marks were incorrectly applied when processing
vowel combinations like 'uo' and 'ie'. This fix ensures proper
tone placement according to Vietnamese orthography rules.
```

### Testing Requirements

**Before Submitting:**
1. All tests pass: `./scripts/run_tests.sh`
2. Code coverage > 80%: `./scripts/run_coverage.sh`
3. Performance benchmarks pass: `./scripts/run_benchmarks.sh`
4. Code style check passes: `./scripts/check_style.sh`

**Test Categories:**
- Unit tests for all new functions
- Integration tests for component interactions
- Performance tests for critical paths
- Regression tests for bug fixes

## Best Practices

### Performance Optimization

**Cache-Friendly Design:**
```cpp
class OptimizedProcessor {
private:
    // Align to cache line boundaries
    alignas(64) std::array<Uint32, 256> lookupTable_;

    // Group frequently accessed data together
    struct HotData {
        std::vector<Uint32> currentWord;
        size_t currentIndex;
        bool isTyping;
    } hotData_;

public:
    void processCharacter(Uint16 character) {
        // Use lookup table for O(1) operations
        Uint32 converted = lookupTable_[character];

        // Minimize cache misses by working with local data
        hotData_.currentWord[hotData_.currentIndex++] = converted;
    }
};
```

**Memory Optimization:**
```cpp
class MemoryEfficientEngine {
private:
    // Use object pools for frequent allocations
    std::vector<std::unique_ptr<WordBuffer>> bufferPool_;
    size_t nextAvailableBuffer_ = 0;

public:
    WordBuffer* getBuffer() {
        if (nextAvailableBuffer_ < bufferPool_.size()) {
            return bufferPool_[nextAvailableBuffer_++].get();
        }

        // Create new buffer if pool is exhausted
        bufferPool_.emplace_back(std::make_unique<WordBuffer>());
        return bufferPool_.back().get();
    }

    void returnBuffer(WordBuffer* buffer) {
        // Reset buffer and return to pool
        buffer->reset();
        if (nextAvailableBuffer_ > 0) {
            nextAvailableBuffer_--;
        }
    }
};
```

### Error Handling

**Exception Safety:**
```cpp
class SafeComponent {
public:
    void process(const InputData& data) noexcept {
        try {
            processImpl(data);
        } catch (const std::exception& e) {
            handleError(e);
        } catch (...) {
            handleUnknownError();
        }
    }

private:
    void processImpl(const InputData& data);
    void handleError(const std::exception& e);
    void handleUnknownError();
};
```

**Resource Management:**
```cpp
class ResourceGuard {
private:
    std::function<void()> cleanup_;

public:
    template<typename T>
    ResourceGuard(T* resource, void (*deleter)(T*))
        : cleanup_([resource, deleter]() { deleter(resource); }) {}

    ~ResourceGuard() {
        if (cleanup_) {
            cleanup_();
        }
    }

    // Prevent copying
    ResourceGuard(const ResourceGuard&) = delete;
    ResourceGuard& operator=(const ResourceGuard&) = delete;
};
```

### Security Considerations

**Input Validation:**
```cpp
class SecureInputProcessor {
public:
    bool validateInput(const std::string& input) const {
        // Check for buffer overflow attempts
        if (input.length() > MAX_INPUT_LENGTH) {
            return false;
        }

        // Check for malicious characters
        for (char c : input) {
            if (!isValidCharacter(c)) {
                return false;
            }
        }

        return true;
    }

private:
    bool isValidCharacter(char c) const {
        return std::isprint(c) && c != '\0';
    }
};
```

**Configuration Security:**
```cpp
class SecureConfigManager {
public:
    bool loadConfig(const std::string& path) {
        // Validate file path
        if (!isValidPath(path)) {
            return false;
        }

        // Validate file size
        std::ifstream file(path);
        if (!file.is_open()) {
            return false;
        }

        file.seekg(0, std::ios::end);
        if (file.tellg() > MAX_CONFIG_SIZE) {
            return false;
        }

        // Parse and validate configuration
        return parseAndValidate(file);
    }

private:
    bool isValidPath(const std::string& path) const;
    bool parseAndValidate(std::ifstream& file);
};
```

This developer guide provides comprehensive information for contributing to the EndKey project, following modern C++ best practices and maintaining high code quality standards.