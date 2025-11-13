# EndKey Codebase Summary

## Overview

The EndKey codebase is structured as a hybrid C++/Objective-C application with clear separation between the core engine and the macOS user interface. The codebase follows a modular architecture with distinct components handling different aspects of Vietnamese text processing and user interaction.

## High-Level Structure

```
EndKey/
├── engine/                    # Core C++ engine
│   ├── Engine.cpp            # Main event processing logic
│   ├── Engine.h              # Engine API interface
│   ├── Vietnamese.cpp        # Vietnamese character processing
│   ├── Vietnamese.h          # Vietnamese module interface
│   ├── Macro.cpp             # Text expansion system
│   ├── Macro.h               # Macro API
│   ├── SmartSwitchKey.cpp    # Application-aware switching
│   ├── SmartSwitchKey.h      # Smart switch API
│   ├── ConvertTool.cpp       # Text conversion utilities
│   ├── ConvertTool.h         # Convert tool API
│   ├── DataType.h            # Data structures and constants
│   └── platforms/
│       └── mac.h             # Platform-specific definitions
├── macOS/ModernKey/          # macOS application (Objective-C)
│   ├── AppDelegate.h         # Application lifecycle
│   ├── AppDelegate.m         # Main application controller
│   ├── ViewController.m      # Preferences UI
│   ├── EndKeyManager.m       # Event tap management
│   ├── EndKey.mm             # Main entry point
│   ├── ConvertToolViewController.mm  # Convert tool UI
│   ├── MacroViewController.mm        # Macro management UI
│   ├── MJAccessibilityUtils.m        # Permission handling
│   ├── AboutViewController.m         # About dialog
│   ├── MyTextField.m                 # Custom text field
│   ├── Base.lproj/           # Storyboard and resources
│   └── Assets.xcassets/      # App icons and images
└── macOS/EndKeyHelper/       # Helper application
    └── (Helper tool components)
```

## Core Components

### 1. Engine Core (engine/)

The engine directory contains the heart of EndKey's functionality - a pure C++ implementation of Vietnamese text processing logic. This core is platform-agnostic and handles all linguistic processing.

#### Key Modules

**Engine.cpp/h**
- **Size**: ~58KB source (largest single file)
- **Purpose**: Main event loop and coordination
- **Responsibilities**:
  - Receives keyboard/mouse events from platform layer
  - Orchestrates text processing pipeline
  - Manages internal state (typing buffer, current word, etc.)
  - Handles character combination and conversion
  - Coordinates between Vietnamese module, Macro module, and SmartSwitch module
  - Implements session management (word boundaries, backspace handling)

**Vietnamese.cpp/h**
- **Size**: ~23KB source
- **Purpose**: Vietnamese language processing
- **Responsibilities**:
  - Character mapping tables (vowels, consonants, tones)
  - Diphthong and triphthong recognition
  - Tone mark placement algorithms
  - Modern vs. traditional orthography handling
  - Quick Telex combination logic
  - Spell checking validation
  - Unicode normalization and encoding

**Macro.cpp/h**
- **Size**: ~9KB source
- **Purpose**: Text expansion system
- **Responsibilities**:
  - Macro dictionary management (string → expanded text)
  - Case-preserving expansion logic
  - Macro persistence to disk
  - Loading macros from storage
  - Case sensitivity handling (btw vs BTW vs Btw)
  - Macro validation and sanitization

**SmartSwitchKey.cpp/h**
- **Size**: ~2KB source
- **Purpose**: Application-aware language switching
- **Responsibilities**:
  - Maintains per-application language preferences
  - Detects active application via bundle ID
  - Switches input method based on current app
  - Persists application preferences
  - Provides default fallbacks

**ConvertTool.cpp/h**
- **Size**: ~6KB source
- **Purpose**: Text conversion utilities
- **Responsibilities**:
  - Batch convert text between encodings
  - Clipboard integration for quick conversion
  - HTML and plain text conversion support
  - Encoding detection and validation

**DataType.h**
- **Size**: ~5KB source
- **Purpose**: Shared data structures and constants
- **Responsibilities**:
  - Type definitions (Uint8, Uint16, Uint32, etc.)
  - Event type enumerations
  - Internal state structures (vKeyHookState)
  - Bit masks for character attributes
  - Platform-specific macro definitions
  - Vietnamese character encoding constants

**platforms/mac.h**
- **Purpose**: Platform-specific definitions for macOS
- **Contents**: Key code mappings, event type definitions, macOS-specific constants

### 2. macOS Application (macOS/ModernKey/)

The macOS application layer provides the native interface and system integration. Written in Objective-C using Cocoa frameworks.

#### Key Components

**AppDelegate.h/m**
- **Size**: ~25KB source (largest UI file)
- **Purpose**: Application lifecycle and coordination
- **Responsibilities**:
  - Application startup and initialization
  - Main menu handling
  - Status menu management
  - Configuration persistence
  - Auto-startup with system
  - Input method change notifications
  - Window management for preferences
  - Version update checking
  - Global event filtering

**ViewController.m**
- **Size**: ~12KB source
- **Purpose**: Preferences window UI
- **Responsibilities**:
  - Preferences tabbed interface
  - Input method selection
  - Feature toggles (FreeMark, Quick Telex, etc.)
  - Spell checking controls
  - Smart switch configuration
  - Event tap initialization UI
  - Permission status display

**EndKeyManager.m**
- **Size**: ~9KB source
- **Purpose**: Event tap lifecycle management
- **Responsibilities**:
  - CGEventTap creation and management
  - Event callback registration
  - Background thread management for event processing
  - Event mask configuration (keyboard, mouse, flags)
  - Event tap enable/disable
  - Run loop source management
  - Error handling for permission failures

**EndKey.mm**
- **Size**: ~9KB source
- **Purpose**: Application entry point
- **Responsibilities**:
  - Main function entry
  - NSApplication setup
  - Core engine initialization (C++ bridge)
  - Bundle identifier management
  - Command-line argument handling

**ConvertToolViewController.mm**
- **Size**: ~7KB source
- **Purpose**: Convert tool UI
- **Responsibilities**:
  - Convert window interface
  - Clipboard integration
  - Batch conversion processing
  - User input handling
  - Conversion progress display

**MacroViewController.mm**
- **Size**: ~6KB source
- **Purpose**: Macro management interface
- **Responsibilities**:
  - Macro table display
  - Add/edit/delete macros
  - Search and filter macros
  - Import/export functionality
  - Macro validation

**MJAccessibilityUtils.m**
- **Size**: ~2KB source
- **Purpose**: Permission handling utilities
- **Responsibilities**:
  - Check accessibility permission status
  - Display permission request dialogs
  - Guide users through permission granting
  - Detect permission changes

**AboutViewController.m**
- **Size**: <1KB source
- **Purpose**: About dialog
- **Responsibilities**:
  - Display version information
  - Credits and acknowledgments
  - Link to documentation

**MyTextField.m**
- **Size**: ~2KB source
- **Purpose**: Custom text field component
- **Responsibilities**:
  - Custom field behavior for preferences
  - Input validation
  - Field-specific formatting

**Resources/**
- Contains app icons, storyboards, nibs, and localized strings
- **Main.storyboard**: Main preferences window UI (32KB+)
- **Info.plist**: App metadata and configuration
- **Assets.xcassets**: App icons and images
- **vi-VN.lproj**: Vietnamese localization resources

## Technology Stack

### Core Technologies

**C++ (Engine)**
- **Standard**: C++11 or later
- **Key Libraries**: Standard library only (no external dependencies)
- **Character Encoding**: UTF-8/UTF-32 support via codecvt
- **Container**: std::vector, std::map, std::string, std::wstring

**Objective-C (UI)**
- **Framework**: Cocoa (AppKit, Foundation)
- **Modern Syntax**: Uses Objective-C 2.0 with ARC (Automatic Reference Counting)
- **Interoperability**: Objective-C++ (.mm files) for C++ bridge

**Platform APIs**
- **Core Graphics**: CGEventTap for event interception
- **Application Services**: NSPasteboard for clipboard access
- **Foundation**: Property lists, notifications, file I/O

### Build System

**Xcode Project**
- **Format**: Xcode 10+ compatible .xcodeproj
- **Targets**:
  - ModernKey (main app)
  - EndKeyHelper (helper tool)
- **Configurations**: Debug, Release
- **Architectures**: x86_64 (Intel), arm64 (Apple Silicon) - Universal binary

**Compilation**
- **C++ Compiler**: Clang with -std=c++11
- **Objective-C Compiler**: Clang
- **Optimization**: Release builds use -O3 optimization
- **Warnings**: Comprehensive warning flags enabled
- **Debug Info**: Debug builds include debug symbols

## Key Dependencies

### System Frameworks (macOS)
- **Cocoa.framework**: Core UI and application framework
- **CoreGraphics.framework**: Event tapping and display handling
- **Foundation.framework**: Core utilities, data types, file I/O
- **ApplicationServices.framework**: High-level system services
- **AppKit.framework**: Complete macOS application framework

### No External Dependencies
The codebase intentionally uses no third-party libraries to minimize dependencies and ensure reliability. All functionality is implemented using system frameworks and standard libraries.

## Data Flow Architecture

```
User Input → CGEventTap → Objective-C Bridge → C++ Engine
                                      ↓
                            [Vietnamese Processing]
                                      ↓
                            [Macro Expansion]
                                      ↓
                            [Smart Switch Check]
                                      ↓
                            [Character Combination]
                                      ↓
                       [State Update & Buffer Management]
                                      ↓
                         Modified Event → Send Back to System
```

## Performance Characteristics

### Engine Efficiency
- **Lightweight**: Pure C++ with minimal allocations
- **Real-time**: <10ms latency target
- **Memory**: Bounded buffer sizes (MAX_BUFF = 32)
- **No allocations**: Static tables for character mappings

### Event Processing
- **Low overhead**: Minimal memory allocations per event
- **Threaded**: Background thread handles event processing
- **Run loop**: CFRunLoop integration for efficient event delivery

## Configuration Management

### Global Variables
The engine uses global variables (prefixed with `v`) for configuration state:
- `vLanguage`: Current language (0=English, 1=Vietnamese)
- `vInputType`: Input method (0=Telex, 1=VNI, etc.)
- `vCodeTable`: Encoding (0=Unicode, 1=TCVN3, 2=VNI-Windows)
- `vSwitchKeyStatus`: Modifier key state
- `vUseMacro`: Macro system enable/disable
- `vUseSmartSwitchKey`: Auto language switching
- Plus 20+ additional feature flags

### Persistence
- **Preferences**: Stored in user defaults (NSUserDefaults)
- **Macros**: Binary format saved to ~/Library/Application Support/EndKey/
- **Smart Switch**: App-specific settings in binary format
- **Auto-save**: Settings persist immediately on change

## Development Workflow

### Code Organization
- **Headers**: Interface definitions (.h files)
- **Implementation**: Implementation files (.cpp, .m, .mm)
- **Separation**: Clear boundary between engine (C++) and UI (Objective-C)

### Naming Conventions
- **C++**: PascalCase for types, camelCase for variables, snake_case for internal
- **Objective-C**: camelCase with prefix, descriptive names
- **Global Variables**: All prefixed with `v` (e.g., `vLanguage`)

### Build Process
1. Compile C++ engine to object files
2. Link with Objective-C objects
3. Create bundle structure with resources
4. Code sign (if distribution build)
5. Package .app for distribution

## File Statistics

**Largest Files by Token Count:**
1. `macOS/ModernKey/Base.lproj/Main.storyboard` (32,410 tokens) - UI definition
2. `engine/Engine.cpp` (16,277 tokens) - Core processing logic
3. `macOS/EndKey.xcodeproj/project.pbxproj` (11,876 tokens) - Project config
4. `engine/Vietnamese.cpp` (10,255 tokens) - Language processing
5. `macOS/ModernKey/EndKey.mm` (9,004 tokens) - Entry point

**Total Metrics:**
- **Total Files**: 47 source files (excluding binaries)
- **Total Tokens**: 121,217 tokens in text files
- **Total Characters**: 483,906 characters
- **Languages**: C++ (~60%), Objective-C (~35%), XML/Plist (~5%)

## Integration Points

### C++/Objective-C Bridge
- **External C functions**: Engine exports C-compatible functions
- **Objective-C++**: Uses .mm extension for mixed files
- **Type translation**: UTF-8 strings passed across boundary
- **Memory management**: C++ objects owned by engine, no cross-boundary ownership

### Event Tap Integration
- **CGEventTap**: System-level event interception
- **Callback function**: C function receives all input events
- **Event mask**: Specifies which events to intercept
- **Threading**: Events processed on dedicated background thread

### Persistence Layer
- **Property List**: Native format for simple preferences
- **Custom Binary**: Macro and SmartSwitch data in custom format
- **File I/O**: NSFileManager for platform-appropriate file handling

## Extensibility Points

### Adding New Input Methods
1. Add to `vKeyInputType` enum in DataType.h
2. Define key mappings in Vietnamese.cpp
3. Update IS_SPECIALKEY macro
4. Add UI option in ViewController.m

### Adding New Features
1. Add global variable in Engine.h
2. Implement logic in Engine.cpp
3. Add UI control in ViewController.m
4. Persist to NSUserDefaults in AppDelegate.m

### Platform Portability
- Engine is already platform-agnostic
- Only macOS-specific parts are in Objective-C layer
- Windows/Linux port would require rewriting UI layer
- Platform-specific files limited to platforms/ directory

## Conclusion

The EndKey codebase demonstrates a well-structured architecture with clear separation of concerns. The C++ engine provides a robust, performant foundation for Vietnamese text processing, while the Objective-C layer handles macOS integration and user interface. The modular design allows for relatively easy maintenance and extension, with well-defined interfaces between components.
