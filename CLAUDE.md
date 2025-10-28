# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

EndKey là một bộ gõ tiếng Việt cho macOS được refactoring từ kiến trúc monolithic sang modular architecture sử dụng modern C++. Dự án có hai phần chính:

1. **Engine** (C++) - Core Vietnamese typing engine với architecture mới
2. **macOS App** (Objective-C++) - Giao diện người dùng và system integration

## Build Commands

### Complete Build Workflow (推荐)
```bash
# 1. Kill all running instances
killall -9 EndKey EndKeyHelper 2>/dev/null

# 2. Clean build directory
rm -rf Sources/EndKey/macOS/build/

# 3. Build with clean
xcodebuild -project Sources/EndKey/macOS/EndKey.xcodeproj \
  -target EndKey \
  -configuration Debug \
  CODE_SIGNING_ALLOWED=NO \
  clean build

# 4. Install to Applications
rm -rf /Applications/EndKey.app
cp -a Sources/EndKey/macOS/build/Debug/EndKey.app /Applications/

# 5. Sign for permissions persistence
codesign --force --deep --sign - /Applications/EndKey.app
```

### Quick Build (cho development)
```bash
xcodebuild -project Sources/EndKey/macOS/EndKey.xcodeproj \
  -target EndKey \
  -configuration Debug \
  CODE_SIGNING_ALLOWED=NO \
  build
```

### Build Helper App
```bash
xcodebuild -project Sources/EndKey/macOS/EndKey.xcodeproj \
  -target EndKeyHelper \
  -configuration Debug \
  CODE_SIGNING_ALLOWED=NO \
  build
```

## Architecture Overview

### Engine Architecture (C++)

#### Core Layer Pattern
Engine sử dụng **clean architecture** với separation of concerns rõ ràng:

1. **Interface Layer** (`interfaces/`)
   - `IEngineCore.h` - Main engine coordination
   - `IVietnameseProcessor.h` - Vietnamese language processing
   - `IMacroProcessor.h` - Text expansion và macros
   - `IConfigurationManager.h` - Centralized configuration
   - `IInputProcessor.h` - Keyboard event processing

2. **Core Layer** (`core/`)
   - `EngineCore.h/cpp` - Main implementation với dependency injection
   - `ConfigurationManager.h/cpp` - Settings management
   - `VietnameseProcessor.h/cpp` - Vietnamese character processing
   - `MacroProcessor.h/cpp` - Macro expansion
   - `MemoryManager.h/cpp` - Memory optimization

3. **Bridge Layer** (`EngineBridge.h/cpp`)
   - **100% backward compatibility** với legacy API
   - Maps legacy global variables → new configuration system
   - Maintains exact same public interface

#### Key Components
- **TypingEngine** - Core input processing pipeline
- **SpellingEngine** - Vietnamese orthography validation
- **VietnameseOptimized** - Efficient character classes thay hardcoded maps
- **ConvertTool** - Unicode/encoding conversions
- **SmartSwitchKey** - Language switching logic

### macOS App Architecture (Objective-C++)

#### Main Components
- **EndKeyManager** - Core management và event handling
- **InputEventManager** - Global keyboard event monitoring
- **UIManager** - Status window và preferences
- **ModernKey** - Main app interface với Storyboard
- **EndKeyHelper** - Login item cho auto-start

#### Key Files
- `EndKey.mm` - Main entry point
- `InputEventManager.mm` - System-wide event monitoring
- `EndKeyManager.m` - Business logic coordination
- `ViewController.m` - Main UI controller

## Development Patterns

### Memory Management
- Sử dụng **modern C++** với smart pointers (`std::unique_ptr`)
- **RAII pattern** - không cần manual memory management
- **Memory pools** cho frequent allocations
- **LRU caching** cho character lookups (50K entries)

### Configuration System
- Centralized configuration qua `ConfigurationManager`
- Legacy global variables auto-sync với new system
- Real-time configuration updates

### Testing Strategy
- Modular architecture enables isolated component testing
- Mock interfaces cho unit testing
- Backward compatibility ensures existing functionality works

### Performance Optimizations
- **Lazy loading** cho conversion tables
- **Character caching** với 50K LRU cache
- **Hot path optimization** trong typing pipeline
- **Memory-efficient data structures**

## Important Notes

### Backward Compatibility
- EngineBridge maintains **100% API compatibility**
- Legacy code works without changes
- Global variables automatically synchronized

### Code Style
- Modern C++14/17 patterns
- RAII resource management
- Interface-based design for testability
- Dependency injection pattern

### File Organization
```
engine/
├── interfaces/     # Abstract interfaces
├── core/          # Concrete implementations
├── platforms/     # Platform-specific code
└── [legacy files] # Original monolithic code

macOS/
├── ModernKey/     # Main app UI
├── EndKeyHelper/  # Background helper
└── build/         # Build output
```

## Debugging

### Common Issues
- **Permissions**: macOS requires accessibility permissions cho keyboard monitoring
- **Code Signing**: App cần được signed cho persistent permissions
- **Memory Leaks**: Use modern C++ patterns để avoid leaks

### Debug Commands
```bash
# Check running processes
ps aux | grep EndKey

# Monitor memory usage
leaks --atExit -- /Applications/EndKey.app/Contents/MacOS/EndKey

# Check accessibility permissions
sudo tccutil reset Accessibility
```