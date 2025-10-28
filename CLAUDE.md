# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

EndKey là một bộ gõ tiếng Việt cho macOS với kiến trúc động cơ đã được tái cấu trúc theo nguyên tắc SOLID. Gồm hai phần chính:

1. **Engine** (C++) - Động cơ xử lý ngôn ngữ và gõ phím
2. **macOS App** (Objective-C/C++) - Giao diện người dùng và tích hợp hệ thống

## Development Commands

### Complete rebuild workflow - use this for most development**:
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

## Architecture

### Engine Structure (C++)
```
engine/
├── interfaces/           # Abstract interfaces
│   ├── IEngineCore.h
│   ├── IInputProcessor.h
│   ├── IVietnameseProcessor.h
│   ├── IMacroProcessor.h
│   └── IConfigurationManager.h
├── core/                # Implementations
│   ├── EngineCore.h
│   ├── ConfigurationManager.h/cpp
│   ├── InputProcessor.h
│   ├── VietnameseProcessor.h
│   ├── MacroProcessor.h
│   └── MemoryManager.h/cpp
├── EngineBridge.h/cpp   # Backward compatibility
└── [legacy files...]    # Original files
```

### macOS App Structure
```
macOS/
├── ModernKey/           # Main application
│   ├── EndKey.mm        # Main entry point
│   ├── EndKeyManager.h/m
│   ├── InputEventManager.h/mm
│   ├── UIManager.h/mm
│   └── [UI controllers...]
├── EndKey.xcodeproj/    # Xcode project
└── EndKeyHelper/        # Helper app
```

## Design Patterns

### Clean Architecture
- **Dependency Injection**: Components receive dependencies through constructors
- **Interface Segregation**: Small, focused interfaces
- **Single Responsibility**: Mỗi class có một trách nhiệm rõ ràng
- **Factory Pattern**: EngineFactory tạo tất cả components

### Backward Compatibility
Bridge layer (`EngineBridge.h/cpp`) cung cấp tương thích 100% với API cũ:
```cpp
// Legacy API vẫn hoạt động
void* vKeyInit();
void vKeyHandleEvent(const vKeyEvent& event, ...);
Uint32 getCharacterCode(const Uint32& data);
```

## Key Components

### Engine Core Components
- **EngineCore**: Điều phối chính của động cơ
- **VietnameseProcessor**: Xử lý chuyển đổi tiếng Việt (Telex, VNI)
- **MacroProcessor**: Xử lý macro và text expansion
- **ConfigurationManager**: Quản lý settings và preferences
- **MemoryManager**: Tối ưu memory và caching

### macOS Integration
- **EndKeyManager**: Quản lý trạng thái ứng dụng
- **InputEventManager**: Xử lý keyboard events và hotkeys
- **UIManager**: Quản lý giao diện người dùng
- **MJAccessibilityUtils**: Accessibility utilities

## Development Workflow

### Thêm Feature Mới
1. Nếu là engine logic: Thêm vào `core/` hoặc extend interfaces
2. Nếu là UI: Thêm vào `macOS/ModernKey/`
3. Đảm bảo backward compatibility qua Bridge layer
4. Test với cả legacy API và new API

### Testing
Không có automated test suite hiện tại. Testing được thực hiện thủ công qua:
- Manual testing trong macOS app
- Unit testing cho engine functions (nếu cần)

### Memory Management
- Sử dụng smart pointers (`std::unique_ptr`)
- RAII pattern
- Memory pools cho frequent allocations
- LRU caches cho performance

## Common Tasks

### Debug Engine Issues
1. Kiểm tra logic trong `engine/core/`
2. Xem legacy behavior trong `engine/Engine.cpp`
3. Verify bridge layer compatibility

### Debug macOS Integration
1. Kiểm tra `InputEventManager.mm` cho keyboard events
2. Xem `EndKeyManager.m` cho app state
3. Verify accessibility permissions

### Add New Input Method
1. Extend `IVietnameseProcessor.h`
2. Implement trong `VietnameseProcessor.h`
3. Update `ConfigurationManager` với new type
4. Add UI controls trong `ViewController.m`

## File Types & Extensions

- `.h/.cpp` - C++ engine files
- `.h/.m` - Objective-C headers/implementation
- `.h/.mm` - Objective-C++ files (khi mix C++ và Objective-C)
- `.pbxproj` - Xcode project file
- `.plist` - Property lists (app configuration)
- `.storyboard` - Interface Builder files
- `.xib` - Legacy Interface Builder files

## Important Notes

- Engine đã được refactor từ monolithic sang modular architecture
- 100% backward compatibility được maintain qua Bridge layer
- Smart pointers và RAII được sử dụng cho memory safety
- macOS app là LSUIElement (system tray app, không có dock icon)
- Yêu cầu accessibility permissions cho keyboard events