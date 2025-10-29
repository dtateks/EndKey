# Project Context

## Purpose
EndKey là bộ gõ tiếng Việt cho macOS được refactoring từ monolithic architecture sang modular architecture sử dụng modern C++. Dự án bao gồm:
- **Engine (C++)**: Core Vietnamese typing engine với architecture modular và clean
- **macOS App (Objective-C++)**: Giao diện người dùng và system integration
- **100% Backward Compatibility**: Đảm bảo tương thích hoàn toàn với legacy API

## Tech Stack

### Core Technologies
- **C++14/17**: Modern C++ cho engine layer với smart pointers, RAII pattern
- **Objective-C++**: macOS application UI và system integration
- **Xcode**: Build system và development IDE
- **CMake**: Cross-platform build configuration (planned)

### Architecture Components
- **Clean Architecture**: Separation of concerns với Interface/Core/Bridge layers
- **Dependency Injection**: Pattern sử dụng trong EngineCore
- **RAII Pattern**: Automatic resource management, không cần manual memory management
- **Interface-Based Design**: Abstract interfaces cho testability

### Performance Optimizations
- **Memory Pools**: Cho frequent allocations
- **LRU Caching**: 50K entries cho character lookups
- **Lazy Loading**: Conversion tables được load khi cần
- **Hot Path Optimization**: Trong typing pipeline

## Project Conventions

### Code Style
- **Modern C++ Standards**: C++14/17 với focus trên:
  - `std::unique_ptr` thay vì raw pointers
  - RAII cho resource management
  - `const` correctness
  - Move semantics where appropriate

- **Naming Conventions**:
  - **Files**: PascalCase cho classes (EngineCore.h), camelCase cho functions
  - **Interfaces**: Prefix 'I' (IEngineCore.h, IVietnameseProcessor.h)
  - **Headers**: .h extension, Implementation: .cpp extension
  - **macOS**: .mm cho Objective-C++, .m cho Objective-C

- **Header Organization**:
  - Include guards hoặc `#pragma once`
  - Interface definitions trong .h files
  - Implementation details trong .cpp files

### Architecture Patterns

#### Layer Separation
```
┌─────────────────────────────────┐
│   Interface Layer (interfaces/) │  ← Abstract contracts
├─────────────────────────────────┤
│      Core Layer (core/)         │  ← Business logic
├─────────────────────────────────┤
│   Bridge Layer (EngineBridge)   │  ← Legacy compatibility
└─────────────────────────────────┘
```

#### Key Patterns
- **Dependency Injection**: EngineCore khởi tạo dependencies
- **Interface Segregation**: Small, focused interfaces
- **Strategy Pattern**: Cho Vietnamese processing và macro expansion
- **Observer Pattern**: Configuration updates (planned)

#### Module Organization
```
engine/
├── interfaces/     # Abstract interfaces (I*.h)
├── core/          # Concrete implementations
├── platforms/     # Platform-specific code
├── utils/         # Utility functions
└── EngineBridge.* # Legacy API compatibility

macOS/
├── ModernKey/     # Main app UI (Storyboard, ViewController)
├── EndKeyHelper/  # Login item helper
└── Support/       # Resources, assets
```

### Testing Strategy
- **Modular Testing**: Isolated component testing nhờ modular architecture
- **Mock Interfaces**: Unit testing với mocked dependencies
- **Backward Compatibility Validation**: Test legacy API để đảm bảo không break
- **Performance Testing**: Memory usage và typing response time
- **Integration Testing**: macOS app integration với engine

### Git Workflow
- **Branch Strategy**: Feature branches từ `main`
- **Commit Messages**: Clear, descriptive messages
- **No Direct Main Commits**: All changes qua pull requests
- **Build Verification**: Build project trước khi commit
- **Atomic Commits**: Single-purpose commits

## Domain Context

### Vietnamese Language Processing
- **Character Classes**: Optimized Vietnamese character classifications
- **Unicode Handling**: Proper Unicode/encoding conversions
- **Spelling Engine**: Vietnamese orthography validation
- **Smart Accent**: Automatic Vietnamese tone marks
- **Telex/VNI Support**: Common Vietnamese input methods

### macOS Integration
- **Global Keyboard Monitoring**: System-wide input event capture
- **Accessibility Permissions**: Required cho keyboard event monitoring
- **Status Menu**: Current input method display
- **Preferences Window**: User configuration management
- **Login Item**: Auto-start với EndKeyHelper

### Typing Engine Pipeline
```
Keyboard Event → InputEventManager → InputProcessor
              → VietnameseProcessor → MacroProcessor
              → EngineCore → Output to active application
```

### Memory Management
- **Modern C++**: `std::unique_ptr`, RAII patterns
- **Memory Pools**: Frequent allocation optimization
- **LRU Cache**: 50K character lookup entries
- **No Manual delete**: Tất cả resources managed tự động

## Important Constraints

### Backward Compatibility
- **100% API Compatibility**: Legacy global variables tự động sync
- **EngineBridge Layer**: Maps legacy API → new configuration
- **No Breaking Changes**: Existing implementations hoạt động không đổi
- **Graceful Migration**: Legacy code hoạt động với new engine

### macOS Requirements
- **Code Signing**: Required cho persistent permissions
- **Accessibility Permissions**: User approval cần thiết cho keyboard monitoring
- **Notarization**: Cần thiết cho distribution
- **Minimum macOS Version**: Check compatibility requirements

### Performance Constraints
- **Real-time Typing**: Low latency cho typing response (< 10ms target)
- **Memory Efficiency**: Desktop app expectations
- **CPU Optimization**: Minimal CPU usage background
- **Battery Impact**: Optimized cho laptop usage

### Security Constraints
- **No Network Access**: Offline Vietnamese typing
- **Local Data Only**: No telemetry hoặc data collection
- **Minimal Permissions**: Chỉ request cần thiết permissions
- **Sandbox Compatibility**: macOS app sandbox requirements

## External Dependencies

### macOS Frameworks
- **AppKit**: macOS UI framework
- **Foundation**: Core services
- **Carbon**: Legacy system integration (migration path needed)
- **Input Method Kit**: Input method registration
- **Accessibility API**: Global event monitoring

### Build Tools
- **Xcode**: Development IDE và build system
- **clang++**: C++ compiler
- **LLVM**: Code generation
- **codesign**: Code signing cho distribution

### Development Dependencies
- **Xcode Command Line Tools**: Build tools
- **CMake**: Cross-platform build (planned adoption)
- **Git**: Version control
- **Debug Tools**: `leaks` tool cho memory analysis

### Runtime Dependencies
- **macOS SDK**: System APIs
- **Standard C++ Library**: C++ runtime
- **Objective-C Runtime**: Runtime support cho macOS app

### Third-Party Dependencies
- **None Currently**: Tự implement tất cả functionality
- **Future Consideration**: Unicode libraries if needed
