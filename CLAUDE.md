# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 📋 Project Overview

**EndKey** là bộ gõ tiếng Việt nguồn mở cross-platform, sử dụng kỹ thuật Backspace để loại bỏ lỗi gạch chân khó chịu của bộ gõ mặc định trên macOS.

- **License**: GPL (GNU General Public License)
- **Platform chính**: macOS
- **Hỗ trợ**: Windows, Linux (đang phát triển)
- **Ngôn ngữ**: C++, Objective-C
- **Version hiện tại**: macOS: v2.0.3 (build 47), Windows: v2.0.2 (build 131074)
- **Repository**: https://github.com/tuyenvm/EndKey
- **Website**: http://open-key.org

## 🏗️ Architecture

### Core Components

```
EndKey/
├── Sources/EndKey/
│   ├── engine/              # Core C++ Engine (cross-platform)
│   │   ├── Engine.h/cpp     # Main entry point, API definitions
│   │   ├── Vietnamese.h/cpp # Vietnamese language logic, character mapping
│   │   ├── DataType.h       # Data structures, enums, masks
│   │   ├── Macro.h/cpp      # Macro/shortcut functionality
│   │   ├── ConvertTool.h/cpp # Code table conversion (Unicode, VNI, TCVN3)
│   │   ├── SmartSwitchKey.h/cpp # Smart language switching (DISABLED)
│   │   └── platforms/       # Platform-specific headers
│   │       ├── mac.h
│   │       ├── win32.h
│   │       └── linux.h
│   ├── macOS/              # macOS UI (Objective-C)
│   │   ├── ModernKey/      # Main application
│   │   │   ├── AppDelegate.m
│   │   │   ├── EndKeyManager.m
│   │   │   ├── ViewController.m
│   │   │   ├── MacroViewController.mm
│   │   │   └── ConvertToolViewController.mm
│   │   └── EndKeyHelper/  # Helper app for auto-launch
│   ├── win32/              # Windows UI (C++)
│   └── linux/              # Linux (in development)
```

### Architecture Principles

1. **Event-Driven Architecture**: Main entry point `vKeyHandleEvent()` xử lý tất cả keyboard/mouse events
2. **Backspace Technique**: Sử dụng backspace để sửa ký tự thay vì blocking, tránh lỗi gạch chân
3. **Cross-Platform Core**: Engine C++ shared giữa các platforms
4. **State Management**: `vKeyHookState` structure quản lý trạng thái typing
5. **Platform Abstraction**: Platform-specific code tách biệt trong platform headers

## ⚙️ Build Commands

### macOS Build

**Development Build:**
```bash
# Build without signing
xcodebuild -project Sources/EndKey/macOS/EndKey.xcodeproj -target EndKey -configuration Debug CODE_SIGNING_ALLOWED=NO

# Copy to Applications
cp -a Sources/EndKey/macOS/build/Debug/EndKey.app /Applications/

# Sign for permissions persistence
codesign --force --deep --sign - /Applications/EndKey.app
```

**Production Build:**
```bash
# Open project and use Xcode UI
open Sources/EndKey/macOS/EndKey.xcodeproj
# Product → Archive → Distribute App
```

### Helper App Build
```bash
# Build helper app (separate target)
xcodebuild -project Sources/EndKey/macOS/EndKey.xcodeproj -target EndKeyHelper -configuration Debug CODE_SIGNING_ALLOWED=NO

# Or manual compilation
cd Sources/EndKey/macOS
mkdir -p EndKeyHelper.app/Contents/MacOS
clang -fobjc-arc -framework Foundation -framework AppKit -o EndKeyHelper.app/Contents/MacOS/EndKeyHelper EndKeyHelper/AppDelegate.m EndKeyHelper/main.m
```

### Windows Build
```bash
# Open Visual Studio solution
open Sources/EndKey/win32/EndKey/EndKey.sln
# Build solution
```

### Debug Commands
```bash
# Kill all EndKey processes
killall EndKey EndKeyHelper 2>/dev/null

# Restart EndKey
open /Applications/EndKey.app

# Manual launch (for debugging)
/Applications/EndKey.app/Contents/MacOS/EndKey &
```

## 🔑 Core APIs

### Main Entry Point
```cpp
void vKeyHandleEvent(const vKeyEvent& event,
                     const vKeyEventState& state,
                     const Uint16& data,
                     const Uint8& capsStatus=0,
                     const bool& otherControlKey=false);
```

### Key Global Variables
```cpp
extern int vLanguage;              // 0=English, 1=Vietnamese
extern int vInputType;             // 0=Telex, 1=VNI, 2=SimpleTelex1, 3=SimpleTelex2
extern int vCodeTable;             // 0=Unicode, 1=TCVN3, 2=VNI-Windows, 3=Unicode Compound
extern int vCheckSpelling;         // Enable spelling check
extern int vUseModernOrthography;  // Modern orthography
extern int vQuickTelex;            // Quick Telex mode
extern int vUseMacro;              // Enable macro
extern int vUpperCaseFirstChar;    // Auto capitalize
extern int vTempOffSpelling;       // Temp disable with Ctrl
extern int vQuickStartConsonant;   // f→ph, j→gi, w→qu
extern int vQuickEndConsonant;     // g→ng, h→nh, k→ch
extern int vRememberCode;          // Remember code table per app
extern int vTempOffEndKey;        // Temp disable with Cmd/Alt
// Note: vUseSmartSwitchKey is DISABLED - feature removed due to instability
```

### Utility Functions
```cpp
void* vKeyInit();                  // Initialize engine
Uint32 getCharacterCode(const Uint32& data); // Convert engine char to real char
void startNewSession();            // Start new word
wstring utf8ToWideString(const string& str);
string wideStringToUtf8(const wstring& str);
```

## 🚨 Critical Architecture Notes

### Smart Switch Key Feature - DISABLED
The Smart Switch Key feature has been completely removed due to crashes:
- Feature was causing crashes when enabling "Tắt tiếng Việt khi bộ gõ hệ thống khác tiếng Anh"
- All related code in `SmartSwitchKey.cpp` is still present but disabled
- UI checkbox removed from main settings panel
- Do NOT re-enable this feature without addressing memory corruption issues

### Buffer Overflow Vulnerabilities - FIXED
Multiple buffer overflow issues have been fixed in `Engine.cpp`:
- Added bounds checking for `TypingWord` array access
- Fixed negative index access in vowel checking
- Added defensive programming for map access
- All loops now validate array bounds

### Memory Safety
- Always validate `_index` before accessing `TypingWord[_index]`
- Use bounds checks: `i < _index && i < MAX_UNICODE_STRING`
- Validate map existence: `_vowelCombine.find(key) != .end()`
- Check array bounds before accessing elements at specific positions

## 📁 Important Files

### Core Engine (C++)
- `Sources/EndKey/engine/Engine.h` - Main API và global variables
- `Sources/EndKey/engine/Engine.cpp` - Core typing logic, buffer overflow fixes
- `Sources/EndKey/engine/Vietnamese.h` - Vietnamese data structures
- `Sources/EndKey/engine/Vietnamese.cpp` - Character mapping tables
- `Sources/EndKey/engine/DataType.h` - Type definitions và enums
- `Sources/EndKey/engine/Macro.h/cpp` - Macro system
- `Sources/EndKey/engine/SmartSwitchKey.h/cpp` - DISABLED feature

### macOS UI (Objective-C)
- `Sources/EndKey/macOS/ModernKey/AppDelegate.m` - Main app delegate, global vars
- `Sources/EndKey/macOS/ModernKey/EndKeyManager.m` - Core manager, callback handling
- `Sources/EndKey/macOS/ModernKey/ViewController.m` - Main UI, settings management
- `Sources/EndKey/macOS/ModernKey/MacroViewController.mm` - Macro UI
- `Sources/EndKey/macOS/ModernKey/EndKey.mm` - Main callback implementation
- `Sources/EndKey/macOS/ModernKey/Base.lproj/Main.storyboard` - UI layout

### Configuration
- `version.json` - Version tracking: v2.0.3 (macOS), v2.0.2 (Windows)
- `Sources/EndKey/macOS/ModernKey/Info.plist` - macOS app info
- `Sources/EndKey/macOS/EndKey.xcodeproj/project.pbxproj` - Xcode project configuration

## 🔧 Development Notes

### Code Style
- C++: Follow Google C++ Style Guide
- Objective-C: Follow Apple Coding Guidelines
- Tab size: 4 spaces
- Encoding: UTF-8

### Platform-Specific Notes
- **Bundle ID**: `com.tuyenmai.endkey` (updated from com.tuyenmai.openkey)
- **Deployment Target**: macOS 10.13+ (updated from 10.10 for Xcode compatibility)
- **Code Signing**: Use `CODE_SIGNING_ALLOWED=NO` for development builds
- **Permissions**: Requires Accessibility permissions in System Preferences

### Testing Guidelines
- Test with: TextEdit, Chrome, Safari, Microsoft Word, Excel
- Test all input methods: Telex, VNI, Simple Telex
- Verify all code tables: Unicode, TCVN3, VNI Windows, Unicode Compound
- Test edge cases in Vietnamese spelling and grammar checking
- Test macro functionality and quick consonant features
- Test accessibility permissions persistence

### Debugging Crash Issues
1. **Check Console.app** for crash logs and error messages
2. **Monitor processes**: `ps aux | grep EndKey`
3. **Kill and restart**: `killall EndKey EndKeyHelper`
4. **Check accessibility permissions**: System Preferences > Security & Privacy > Accessibility
5. **Verify bundle identifier**: Ensure `com.tuyenmai.endkey` is consistent

## 🎯 Current Project Status

**Stable and Production Ready** - All major crashes and buffer overflows have been fixed.

### Recent Major Changes (2025-10-24)
- ✅ Complete OpenKey → EndKey migration
- ✅ Fixed 21+ buffer overflow vulnerabilities in Engine.cpp
- ✅ Removed unstable Smart Switch Key feature
- ✅ Updated bundle identifier to `com.tuyenmai.endkey`
- ✅ Fixed deployment target compatibility
- ✅ Improved accessibility permissions handling
- ✅ Added comprehensive bounds checking for memory safety

## 🔗 Resources

- **Documentation**: [README.md](README.md), [CHANGELOG.md](CHANGELOG.md)
- **Download**: https://github.com/tuyenvm/EndKey/releases
- **Build Guide**: [macOS_Build.md](macOS_Build.md)
- **Repository**: https://github.com/dtateks/OpenKey (forked from tuyenvm/EndKey)

---

## Developer Context

### Working Style
- **Method**: GitHub Spec Kit với Spec-Driven Development
- **Coding Philosophy**: 100% Vibe Code - tập trung vào trải nghiệm người dùng
- **Code Review**: Không đọc code sâu, chỉ feedback qua UI/UX

### Communication Preferences
- **Language**: Luôn giao tiếp Tiếng Việt, ngắn gọn, súc tích, dễ hiểu
- **Decision Making**: Coding Agent tự quyết định các thay đổi nhỏ
- **Feedback**: Ưu tiên giải pháp thực tế thay vì phân tích kỹ thuật

### Important Technical Constraints
- **Memory Safety**: Critical for this C++ codebase - always validate array bounds
- **Platform Dependencies**: Engine is cross-platform, UI is platform-specific
- **Bundle Identifier**: Must be `com.tuyenmai.endkey` (NOT com.tuyenmai.openkey)
- **Accessibility**: Requires proper permissions and signing for persistence

### MCP Server Guide
**gkg** - ❌ Không hỗ trợ C++/Objective-C
- Không thể dùng cho project này, use alternative approaches

**context7**
- Library/framework API documentation
- Version-specific features
- BEFORE implementing with unfamiliar libraries

**playwright**
- Open browser for UI testing
- ONLY when browser rendering required

**web-search-prime**
- Web Research, finding latest info
- Community discussions
- When context7 doesn't have docs

---

**Last Updated**: 2025-10-24 (Post-Merge to Main)
**Status**: 🟢 Production Ready - All Major Issues Fixed
**Current Branch**: main
**Project**: EndKey Vietnamese Input Method Engine