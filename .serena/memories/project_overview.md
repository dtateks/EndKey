# EndKey Project Overview

**EndKey** là một bộ gõ tiếng Việt cho macOS được xây dựng với architecture kết hợp giữa native macOS app (Objective-C++) và cross-platform C++ engine, cùng với AI-powered orchestration system.

## Architecture Components

### Core Structure
- **Engine**: Cross-platform C++ engine xử lý tiếng Việt
  - Engine.h/cpp: Main engine interface (56KB)
  - Vietnamese.h/cpp: Vietnamese input processing (22KB)
  - Macro.h/cpp: Macro functionality (9KB)
  - SmartSwitchKey.h/cpp: Smart app switching
  - ConvertTool.h/cpp: Unicode conversion (6KB)
  - DataType.h: Data types and constants
  - platforms/: Platform-specific headers (mac.h, win32.h, linux.h)

- **macOS App**: Native macOS application
  - ModernKey/: Main app with UI components
  - EndKeyHelper/: Helper app for permissions
  - EndKeyUpdate/: Auto-updater

### Technology Stack
- **Core Engine**: Cross-platform C++ (Windows/Linux/macOS)
- **macOS App**: Objective-C++ with Cocoa framework
- **Build System**: Xcode project with native compilation
- **Development**: Claude-Flow SPARC methodology với 54+ agents
- **Coordination**: MCP servers (claude-flow@alpha, ruv-swarm)

### Key Features
- **Input Methods**: Telex, VNI, Simple Telex
- **Smart Switching**: Automatic language detection per application
- **Macro System**: Text expansion với auto-capitalization
- **Code Tables**: Unicode, TCVN3, VNI-Windows encoding support
- **Spelling Correction**: Modern Vietnamese orthography
- **Quick Consonants**: f→ph, j→gi, w→qu replacements

### Development Patterns
- **Bridge Pattern**: EndKey.mm bridges C++ engine to Objective-C UI
- **Strategy Pattern**: Multiple input methods với interchangeable algorithms
- **Factory Pattern**: Engine creation với platform-specific initialization
- **Observer Pattern**: Event-driven keyboard/mouse input handling

### Memory Management
- **ARC**: Automatic Reference Counting cho Objective-C
- **Smart Pointers**: C++14 smart pointers cho engine components
- **Session Persistence**: SQLite-based memory storage cho agent coordination

### Integration Points
- **Carbon Framework**: Low-level keyboard event handling
- **Cocoa AppKit**: Modern macOS UI components
- **Core Foundation**: System-level integration cho permissions và updates