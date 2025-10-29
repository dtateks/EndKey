## Project Overview

EndKey is a Vietnamese typing engine and input method for macOS. It provides multiple Vietnamese input methods (Telex, VNI, Simple Telex) with advanced features like macro support, smart language switching, spelling checking, and custom character encoding conversions.

## Architecture

The project follows a hybrid architecture with three main components:

### 1. Core Engine (`engine/`)
- **C++ engine** that handles all Vietnamese typing logic
- Platform-agnostic core with platform-specific headers in `engine/platforms/`
- Key components:
  - `Engine.cpp/.h` - Main entry point, processes keyboard events
  - `Vietnamese.cpp/.h` & `VietnameseOptimized.cpp/.h` - Vietnamese character processing and tone/mark handling
  - `TypingEngine.cpp/.h` - Core typing logic
  - `SpellingEngine.cpp/.h` - Spelling validation
  - `Macro.cpp/.h` & `MacroManager.cpp/.h` - Macro system for text expansion
  - `SmartSwitchKey.cpp/.h` - Auto language switching based on application
  - `ConvertTool.cpp/.h` - Character encoding conversion (Unicode, TCVN3, VNI-Windows)
  - `EngineBridge.cpp/.h` - Interface between C++ engine and platform-specific code
  - `core/` - Core engine components (ConfigurationManager, MemoryManager, etc.)

### 2. macOS Application (`macOS/ModernKey/`)
- **Objective-C++ wrapper** around the C++ engine
- Main UI and event handling layer
- Key files:
  - `AppDelegate.m` - Main application delegate with global state management
  - `EndKey.mm` - Core event manager bridging macOS events to C++ engine
  - `ViewController.m` - Main preferences UI
  - `UIManager.mm` - UI state management
  - `InputEventManager.mm` - Low-level keyboard/mouse event capture
  - `ConvertToolViewController.mm` - Encoding conversion UI
  - `MacroViewController.mm` - Macro management UI
  - Requires Accessibility permissions for keyboard monitoring

### 3. Helper Application (`macOS/EndKeyHelper/`)
- **Background helper app** for privileged operations
- Handles system-level tasks that require elevated permissions

## Development Workflow

### Complete Build Workflow
```bash
# Kill existing processes
killall -9 EndKey EndKeyHelper 2>/dev/null

# Clean build
xcodebuild -project Sources/EndKey/macOS/EndKey.xcodeproj \
  -target EndKey \
  -configuration Debug \
  CODE_SIGNING_ALLOWED=NO \
  clean build

# Install to Applications
cp -af macOS/build/Debug/EndKey.app /Applications/

# Sign app (required for persistent permissions)
codesign --force --deep --sign - /Applications/EndKey.app
```

### Key Build Settings

The project requires:
- **Accessibility permissions** - App must be authorized to monitor input
- **App Sandbox** - Enabled with specific entitlements
- **Architectures** - x86_64 and arm64 (Universal binary)
- **C++ Standard Library** - libc++

### Development Notes

1. **Cross-Language Bridging**: The engine uses C++ for core logic with Objective-C++ (`.mm`) files for macOS integration. Key files like `EndKey.mm` bridge between these worlds.

2. **Memory Management**: The C++ engine uses raw pointers and manual memory management. The `MemoryManager` class in `engine/core/MemoryManager.cpp` handles memory allocation.

3. **Configuration**: All engine behavior is controlled via global variables defined in `engine/Engine.h` (e.g., `vLanguage`, `vInputType`, `vUseMacro`, etc.).

4. **Testing Approach**: There are no automated tests currently. Manual testing is required after changes to:
   - Vietnamese character processing
   - Tone mark placement
   - Macro expansion
   - Input method switching

5. **Key Code Definitions**: Platform-specific key codes are defined in `engine/platforms/mac.h`, `win32.h`, and `linux.h`.

### Important Files to Understand

- **`engine/DataType.h`** - Core data structures, masks, and constants
- **`engine/Engine.h`** - Public API and global configuration variables
- **`engine/Engine.cpp`** - Main processing logic (line 214: `vKeyHandleEvent()`)
- **`macOS/EndKey.mm`** - Event bridge (line 1: main entry point)
- **`macOS/AppDelegate.m`** - App lifecycle and state management

### Configuration Variables

Key engine configuration (all declared in `engine/Engine.h`):

```cpp
extern int vLanguage;              // 0: English, 1: Vietnamese
extern int vInputType;             // 0: Telex, 1: VNI
extern int vUseMacro;              // Enable/disable macros
extern int vCheckSpelling;         // Enable spelling checking
extern int vUseSmartSwitchKey;     // Auto-switch language per app
extern int vCodeTable;             // 0: Unicode, 1: TCVN3, 2: VNI-Windows
```

### Common Development Tasks

**Adding a new configuration option:**
1. Declare variable in `engine/Engine.h` (around line 48-194)
2. Initialize in `engine/Engine.cpp`
3. Wire up UI in `macOS/ViewController.m`
4. Save/load in `macOS/AppDelegate.m`

**Modifying Vietnamese processing:**
1. Main logic in `engine/VietnameseOptimized.cpp`
2. Tone handling in `engine/Vietnamese.cpp`
3. Spelling validation in `engine/SpellingEngine.cpp`

**Adding a new input method:**
1. Update `vKeyInputType` enum in `engine/DataType.h` (line 33)
2. Add processing keys to `ProcessingChar` array in `engine/Engine.cpp` (line 43)
3. Update `IS_SPECIALKEY` macro in `engine/DataType.h` (line 134)

### Recent Changes

Based on git history, recent work has focused on:
- Removing legacy encoding support
- Improving modular architecture
- Fixing macro buffer clearing logic
- Cleanup and modernization of codebase

### Permissions Required

For development and testing:
1. **Accessibility** - Grant permission in System Preferences → Security & Privacy → Privacy → Accessibility
2. **Input Monitoring** - Grant permission in System Preferences → Security & Privacy → Privacy → Input Monitoring

The app will request these on first run, but they can also be pre-granted for automated testing.

### Debugging Tips

- Set `IS_DEBUG` to 1 in `engine/Engine.h` (line 21) for debug builds
- Use Xcode's debugger to step through both Objective-C++ and C++ code
- Check `macOS/build/` for compiled products
- Test with various input methods (Telex, VNI) and features (macros, smart switching)

### Platform Support

While primarily a macOS app, the core engine supports:
- **macOS** - Primary target (fully implemented)
- **Windows** - Partial support via platform headers
- **Linux** - Partial support via platform headers

Only macOS has a full UI implementation. Other platforms would require additional platform-specific wrappers.
