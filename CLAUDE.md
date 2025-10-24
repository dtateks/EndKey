# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

---

## ⚠️ CRITICAL NOTICE: Outdated Fork!

**This repository is an OUTDATED FORK of OpenKey!**

| | Current Fork | Official Upstream |
|---|---|---|
| **Repository** | [dtateks/EndKey](https://github.com/dtateks/EndKey) | [tuyenvm/OpenKey](https://github.com/tuyenvm/OpenKey) ⭐ |
| **Version** | v2.0.3 (Nov 2021) | **v2.0.5 (Mar 2022)** |
| **Status** | ⚠️ 2 versions behind | 🟢 Latest |
| **M1 Support** | ❌ No | ✅ Yes (v2.0.4+) |
| **macOS Big Sur+** | ⚠️ Limited | ✅ Full (v2.0.5) |

**Missing Critical Updates:**
- ❌ v2.0.4: Apple Silicon (M1/M2) support
- ❌ v2.0.5: Status bar improvements, Windows stability

**Action Required:** [See Sync Instructions](#-sync-with-upstream-recommended)

---

## 🔍 Quick Navigation

### For New Developers
1. [Project Overview](#-project-overview) - High-level understanding
2. [Architecture](#-architecture) - System design & patterns
3. [Core APIs](#-core-apis) - Main entry points & global variables
4. [Memory Safety](#memory-safety) - Critical coding guidelines

### For Feature Development
1. [Features & Capabilities](#-features--capabilities) - What already exists
2. [Engine Internals](#engine-internals) - Masks, constants, state management
3. [Important Files](#-important-files) - Where to find code
4. [Build Commands](#-build-commands) - How to compile & test

### For Bug Fixing
1. [Known Issues & Limitations](#known-issues--limitations) - Common problems
2. [Memory Safety](#memory-safety) - Buffer overflow prevention
3. [Debugging Crash Issues](#debugging-crash-issues) - Debug checklist
4. [Testing Guidelines](#testing-guidelines) - How to verify fixes

### For Integration Work
1. [Dependencies & Frameworks](#dependencies--frameworks) - External libraries
2. [Platform-Specific Notes](#platform-specific-notes) - macOS/Windows/Linux differences
3. [Key Global Variables](#key-global-variables-engineh) - Configuration options
4. [Codebase Statistics](#-codebase-statistics) - Project metrics

---

## 📋 Project Overview

**OpenKey/EndKey** là bộ gõ tiếng Việt nguồn mở cross-platform, sử dụng kỹ thuật Backspace để loại bỏ lỗi gạch chân khó chịu của bộ gõ mặc định trên macOS.

- **License**: GPL (GNU General Public License)
- **Platform chính**: macOS
- **Hỗ trợ**: Windows, Linux (đang phát triển)
- **Ngôn ngữ**: C++, Objective-C

### ⚠️ Repository Status
- **Current Repository**: https://github.com/dtateks/EndKey (FORK - Outdated)
- **Current Version**: v2.0.3 (build 47) - November 6, 2021
- **Official Repository**: https://github.com/tuyenvm/OpenKey ⭐
- **Latest Upstream**: v2.0.5 (March 19, 2022)
- **Website**: http://open-key.org

### 🚨 IMPORTANT: This is an Outdated Fork!
This repository is **2 versions behind** the official source:
- ❌ **Missing v2.0.4** (Jan 28, 2022): Apple Silicon (M1) support
- ❌ **Missing v2.0.5** (Mar 19, 2022): Status bar improvements, Windows stability

**Recommendation**: Sync with upstream `tuyenvm/OpenKey` to get latest features and fixes!

### Input Methods Supported
1. **Telex** (0): Standard Telex typing (aw→ă, ow→ơ, w→ư, aa→â, etc.)
2. **VNI** (1): VNI-style typing with numbers (a6→ă, o7→ơ, u7→ư, etc.)
3. **Simple Telex 1** (2): Simplified Telex without brackets
4. **Simple Telex 2** (3): Alternative simplified Telex

### Code Tables Supported
1. **Unicode** (0): Standard Unicode (UTF-8) - Recommended
2. **TCVN3 (ABC)** (1): Legacy Vietnamese encoding
3. **VNI-Windows** (2): VNI Windows encoding
4. **Unicode Compound** (3): Unicode with combining characters
5. **CP 1258**: Vietnamese locale code page

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

### Engine Internals

#### Memory Management
- **MAX_BUFF**: 32 characters - Maximum buffer size for typing word
- **TypingWord Array**: Internal array storing current word being typed
- **_index**: Current position in TypingWord array (MUST validate bounds!)

#### Critical Masks & Constants (DataType.h)
```cpp
// Character State Masks
#define CAPS_MASK           0x10000   // Capital letter indicator
#define TONE_MASK           0x20000   // Tone marker
#define TONEW_MASK          0x40000   // Tone W marker

// Vietnamese Tone Marks (Dấu)
#define MARK1_MASK          0x80000   // Sắc (á)
#define MARK2_MASK          0x100000  // Huyền (à)
#define MARK3_MASK          0x200000  // Hỏi (ả)
#define MARK4_MASK          0x400000  // Ngã (ã)
#define MARK5_MASK          0x800000  // Nặng (ạ)
#define MARK_MASK           0xF80000  // Check if has any mark

// Character Data Masks
#define CHAR_MASK           0xFFFF    // Get first 16 bytes character
#define STANDALONE_MASK     0x1000000 // Created by standalone key (W)
#define CHAR_CODE_MASK      0x2000000 // Keyboard code vs character code
#define PURE_CHARACTER_MASK 0x80000000

// Special Feature Masks
#define END_CONSONANT_MASK  0x4000    // End consonant marker
#define CONSONANT_ALLOW_MASK 0x8000   // Consonant allowed marker
```

#### Hook State Codes (vKeyHookState)
```cpp
enum HoolCodeState {
    vDoNothing = 0,                // Do nothing
    vWillProcess,                  // Will process/reverse
    vBreakWord,                    // Start new word
    vRestore,                      // Restore to old char
    vReplaceMaro,                  // Replace by macro
    vReplaceString,                // Replace by string
    vRestoreAndStartNewSession,    // Restore + start new (for invalid word)
};
```

## ⚙️ Build Commands

### macOS Build

**Quick Rebuild Script (Recommended):**
```bash
# Complete rebuild workflow - use this for most development
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

**Development Build (Step-by-step):**
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
# Force kill all EndKey processes
killall -9 EndKey EndKeyHelper 2>/dev/null

# Restart EndKey
open /Applications/EndKey.app

# Manual launch (for debugging)
/Applications/EndKey.app/Contents/MacOS/EndKey &

# Check running processes
ps aux | grep -E "EndKey|EndKeyHelper" | grep -v grep

# View live logs
log stream --predicate 'process == "EndKey"' --level debug
```

## 🎯 Features & Capabilities

### Core Features
1. **Vietnamese Input Processing**
   - Real-time character composition with backspace technique
   - Supports 4 input methods: Telex, VNI, Simple Telex 1 & 2
   - 5 code tables: Unicode, TCVN3, VNI-Windows, Unicode Compound, CP 1258

2. **Smart Typing Features**
   - **Modern Orthography** (`vUseModernOrthography`): oà, uý thay vì òa, úy
   - **Quick Telex** (`vQuickTelex`): cc→ch, gg→gi, kk→kh, nn→ng, qq→qu, pp→ph, tt→th
   - **Quick Start Consonant** (`vQuickStartConsonant`): f→ph, j→gi, w→qu
   - **Quick End Consonant** (`vQuickEndConsonant`): g→ng, h→nh, k→ch
   - **Upper Case First Char** (`vUpperCaseFirstChar`): Auto-capitalize sentence start

3. **Spelling & Grammar**
   - **Spelling Check** (`vCheckSpelling`): Validate Vietnamese words
   - **Restore If Wrong Spelling** (`vRestoreIfWrongSpelling`): Auto-correct invalid words
   - **Temp Off Spelling** (`vTempOffSpelling`): Hold Ctrl to bypass checking

4. **Macro System** (`Macro.h/cpp`)
   - Unlimited text expansion (macOS default: 20 chars limit)
   - **Auto Caps Macro** (`vAutoCapsMacro`): btw→By the way, BTW→BY THE WAY
   - **Macro in English Mode** (`vUseMacroInEnglishMode`): Use macro even in English

5. **Code Table Conversion** (`ConvertTool.h/cpp`)
   - Convert between Unicode, TCVN3, VNI, Unicode Compound
   - **Remember Code Table** (`vRememberCode`): Auto-switch code table per app

6. **Browser & App Compatibility**
   - **Fix Recommend Browser** (`vFixRecommendBrowser`): Fix autocorrect in Chrome, Safari, Firefox, Excel
   - Supports TextEdit, Microsoft Word, Excel, PowerPoint

7. **System Integration**
   - **Run on Startup**: Auto-launch with macOS
   - **Gray Menu Bar Icon**: Dark mode compatible
   - **Dock Icon**: Quick access to control panel
   - **Temp Off EndKey** (`vTempOffEndKey`): Hold Cmd/Alt to temporarily disable

### Removed/Disabled Features
- ❌ **Smart Switch Key** (`vUseSmartSwitchKey`): DISABLED due to crashes
  - Feature was auto-switching language per app
  - Caused memory corruption issues
  - UI checkbox removed, code still present but inactive

## 🔑 Core APIs

### Main Entry Point
```cpp
void vKeyHandleEvent(const vKeyEvent& event,
                     const vKeyEventState& state,
                     const Uint16& data,
                     const Uint8& capsStatus=0,
                     const bool& otherControlKey=false);
```

### Key Global Variables (Engine.h)

#### Language & Input Settings
```cpp
extern int vLanguage;              // 0=English, 1=Vietnamese
extern int vInputType;             // 0=Telex, 1=VNI, 2=SimpleTelex1, 3=SimpleTelex2
extern int vCodeTable;             // 0=Unicode, 1=TCVN3, 2=VNI-Windows, 3=Unicode Compound, 4=CP1258
extern int vFreeMark;              // 0=No, 1=Yes (free mark placement)
```

#### Spelling & Grammar
```cpp
extern int vCheckSpelling;         // 0=No, 1=Yes - Enable spelling check
extern int vRestoreIfWrongSpelling; // 0=No, 1=Yes - Auto-restore if invalid
extern int vTempOffSpelling;       // 0=No, 1=Yes - Temp disable with Ctrl
```

#### Smart Typing Features
```cpp
extern int vUseModernOrthography;  // 0=òa,úy  1=oà,uý
extern int vQuickTelex;            // 0=No, 1=Yes - cc→ch, gg→gi, etc.
extern int vUpperCaseFirstChar;    // 0=No, 1=Yes - Auto capitalize
extern int vQuickStartConsonant;   // 0=No, 1=Yes - f→ph, j→gi, w→qu
extern int vQuickEndConsonant;     // 0=No, 1=Yes - g→ng, h→nh, k→ch
extern int vAllowConsonantZFWJ;    // 0=No, 1=Yes - Allow Z,F,W,J as consonants
```

#### Macro System
```cpp
extern int vUseMacro;              // 0=No, 1=Yes - Enable macro
extern int vUseMacroInEnglishMode; // 0=No, 1=Yes - Use macro in English mode
extern int vAutoCapsMacro;         // 0=No, 1=Yes - Auto capitalize macro expansion
```

#### System & Compatibility
```cpp
extern int vFixRecommendBrowser;   // 0=No, 1=Yes - Fix browser autocorrect
extern int vRememberCode;          // 0=No, 1=Yes - Remember code table per app
extern int vTempOffEndKey;         // 0=No, 1=Yes - Temp disable with Cmd/Alt
extern int vOtherLanguage;         // 0=No, 1=Yes - Turn off when other language active
extern int vSwitchKeyStatus;       // Hotkey config (8bit:keycode, bit8-10:modifiers)

// DISABLED FEATURES
// extern int vUseSmartSwitchKey;  // ❌ REMOVED - caused crashes
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

### Known Issues & Limitations

#### Fixed Issues ✅
- **Buffer Desync with Claude Code CLI** (2025-10-24): Fixed in [Engine.cpp](Sources/EndKey/engine/Engine.cpp)
  - **Before**: Gõ tiếng Việt trong Claude Code CLI bị mất chữ, cascade failures sau error đầu tiên
  - **Root Cause**: Race condition giữa EndKey backspace technique và Claude Code CLI buffered input
  - **Solution**:
    - Enhanced `startNewSession()` với complete buffer clear (line 473-503)
    - Added `validateBufferState()` - validate buffer integrity before processing (line 509-531)
    - Added `detectBufferDesync()` - detect desync between internal/external buffers (line 537-555)
    - Added `emergencyReset()` - aggressive reset on critical errors (line 561-573)
    - Added validation checks in `vKeyHandleEvent()` before & after processing (line 1451-1457, 1707-1715)
  - **Impact**: ✅ Không còn mất chữ, ✅ Graceful error recovery, ✅ Stable typing experience

- **Macro Feature - Limited Special Character Support** (2025-10-24): Fixed in [Engine.cpp:30-41](Sources/EndKey/engine/Engine.cpp#L30-L41)
  - **Before**: Macro chỉ trigger với 9 ký tự đặc biệt
  - **After**: FULL support - 23 ký tự trigger macro:
    - Control: `Enter`, `Space`, `Tab`
    - Numbers: `1-0` (bao gồm cả Shift variants: `! @ # $ % ^ & * ( )`)
    - Punctuation: `, . / ; ' \ - = [ ] ` `
  - **Impact**: Macro giờ hoạt động với TẤT CẢ ký tự đặc biệt trên bàn phím

- **21+ Buffer Overflow Vulnerabilities** (2025-10-24): All fixed in [Engine.cpp](Sources/EndKey/engine/Engine.cpp)
  - Added comprehensive bounds checking for `TypingWord[_index]` access
  - Fixed negative index access in vowel checking loops
  - Added defensive programming for map access operations
  - All array access now validates bounds before dereferencing

#### Active Limitations ⚠️
- **MAX_BUFF = 32**: Maximum word length is 32 characters
  - Very long words (>32 chars) may be truncated or cause issues
  - Consider increasing if supporting technical/scientific Vietnamese text

- **Platform Dependencies**:
  - macOS: Requires Accessibility permissions (must re-grant after unsigned builds)
  - Windows: Different keyboard event handling mechanism
  - Linux: Still in development, incomplete implementation

- **App Compatibility**:
  - Some apps may require "Send each key" mode (older compatibility mode)
  - Browser autocorrect requires `vFixRecommendBrowser` enabled
  - Excel, Word need special handling for autocorrect behavior

#### Permanently Disabled Features 🚫
- **Smart Switch Key** (`SmartSwitchKey.cpp`):
  - Auto-switch language per application
  - **Reason**: Memory corruption crashes when enabled
  - **Status**: Code present but disabled, UI checkbox removed
  - **Resolution**: Requires complete rewrite with proper memory management
  - **DO NOT RE-ENABLE** without fixing underlying memory issues

## 📁 Important Files

### Core Engine (C++ - Cross-Platform)
- [Engine.h](Sources/EndKey/engine/Engine.h) - Main API, global variables, function declarations
- [Engine.cpp](Sources/EndKey/engine/Engine.cpp) - Core typing logic (1,603 lines), buffer overflow fixes
- [Vietnamese.h](Sources/EndKey/engine/Vietnamese.h) - Vietnamese data structures, character maps
- [Vietnamese.cpp](Sources/EndKey/engine/Vietnamese.cpp) - Character mapping tables (575 lines)
- [DataType.h](Sources/EndKey/engine/DataType.h) - Type definitions, enums, masks, constants
- [Macro.h](Sources/EndKey/engine/Macro.h) / [Macro.cpp](Sources/EndKey/engine/Macro.cpp) - Macro system (295 lines)
- [ConvertTool.h](Sources/EndKey/engine/ConvertTool.h) / [ConvertTool.cpp](Sources/EndKey/engine/ConvertTool.cpp) - Code table conversion (180 lines)
- [SmartSwitchKey.h](Sources/EndKey/engine/SmartSwitchKey.h) / [SmartSwitchKey.cpp](Sources/EndKey/engine/SmartSwitchKey.cpp) - ❌ DISABLED feature (74 lines)

### Platform Headers
- [platforms/mac.h](Sources/EndKey/engine/platforms/mac.h) - macOS-specific definitions
- [platforms/win32.h](Sources/EndKey/engine/platforms/win32.h) - Windows-specific definitions
- [platforms/linux.h](Sources/EndKey/engine/platforms/linux.h) - Linux-specific definitions

### macOS UI (Objective-C/C++)
- [AppDelegate.m](Sources/EndKey/macOS/ModernKey/AppDelegate.m) - Main app delegate, lifecycle, global vars
- [EndKey.mm](Sources/EndKey/macOS/ModernKey/EndKey.mm) - Main keyboard event handler (35KB, critical)
- [EndKeyManager.m](Sources/EndKey/macOS/ModernKey/EndKeyManager.m) - Core manager, callback coordination
- [ViewController.m](Sources/EndKey/macOS/ModernKey/ViewController.m) - Main UI, settings management (20KB)
- [MacroViewController.mm](Sources/EndKey/macOS/ModernKey/MacroViewController.mm) - Macro UI interface
- [ConvertToolViewController.mm](Sources/EndKey/macOS/ModernKey/ConvertToolViewController.mm) - Code conversion UI
- [AboutViewController.m](Sources/EndKey/macOS/ModernKey/AboutViewController.m) - About dialog
- [MJAccessibilityUtils.m](Sources/EndKey/macOS/ModernKey/MJAccessibilityUtils.m) - Accessibility helpers
- [Base.lproj/Main.storyboard](Sources/EndKey/macOS/ModernKey/Base.lproj/Main.storyboard) - UI layout

### Helper App (Auto-launch)
- [EndKeyHelper/AppDelegate.m](Sources/EndKey/macOS/EndKeyHelper/AppDelegate.m) - Helper app delegate
- [EndKeyHelper/main.m](Sources/EndKey/macOS/EndKeyHelper/main.m) - Helper app entry point

### Configuration & Metadata
- [version.json](version.json) - Version tracking: v2.0.3 (macOS build 47), v2.0.2 (Windows build 131074)
- [Info.plist](Sources/EndKey/macOS/ModernKey/Info.plist) - macOS app metadata, bundle ID: `com.tuyenmai.endkey`
- [EndKey.xcodeproj](Sources/EndKey/macOS/EndKey.xcodeproj/project.pbxproj) - Xcode project configuration
- [CLAUDE.md](CLAUDE.md) - This file - Project documentation for AI coding assistants
- [README.md](README.md) - User-facing documentation (Vietnamese)
- [CHANGELOG.md](CHANGELOG.md) - Version history and changes

## 🔧 Development Notes

### Code Style
- C++: Follow Google C++ Style Guide
- Objective-C: Follow Apple Coding Guidelines
- Tab size: 4 spaces
- Encoding: UTF-8

### Dependencies & Frameworks

#### macOS Frameworks
- **Carbon.framework**: Legacy keyboard event handling (CGEventTap, CGEventPost)
- **Cocoa.framework**: UI framework (NSApplication, NSWindow, NSViewController)
- **Foundation.framework**: Base functionality (NSString, NSArray, NSUserDefaults)
- **AppKit.framework**: macOS UI components (NSButton, NSTextField, NSMenu)
- **ServiceManagement.framework**: Auto-launch helper app (SMLoginItemSetEnabled)

#### C++ Standard Library
- `<string>`, `<wstring>`: Unicode string handling
- `<vector>`, `<map>`: STL containers for data structures
- `<locale>`, `<codecvt>`: UTF-8 ↔ Wide string conversion

### Platform-Specific Notes

#### macOS
- **Bundle ID**: `com.tuyenmai.endkey` (updated from com.tuyenmai.openkey)
- **Deployment Target**: macOS 10.13+ (updated from 10.10 for Xcode compatibility)
- **Code Signing**: Use `CODE_SIGNING_ALLOWED=NO` for development builds
- **Permissions**: Requires Accessibility permissions in System Preferences
- **LSUIElement**: True (runs as menu bar app, no dock icon by default)
- **LSMultipleInstancesProhibited**: True (single instance only)

#### Windows
- Visual Studio 2017+ required
- Win32 API for keyboard hooks
- Different event handling from macOS

#### Linux
- In development
- X11/Wayland support planned
- Qt framework for UI (planned)

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

## 📊 Codebase Statistics

### Project Size
- **Total Engine Code**: ~2,700 lines (C++)
  - Engine.cpp: 1,603 lines
  - Vietnamese.cpp: 575 lines
  - Macro.cpp: 295 lines
  - ConvertTool.cpp: 180 lines
  - SmartSwitchKey.cpp: 74 lines (disabled)

- **macOS UI Code**: ~18 files (Objective-C/C++)
  - EndKey.mm: 35KB (main event handler)
  - ViewController.m: 20KB (settings UI)
  - AppDelegate.m, EndKeyManager.m, MacroViewController.mm, etc.

- **Windows UI Code**: 25+ files (C++/Win32)

- **Platform Headers**: 3 files (mac.h, win32.h, linux.h)

### Supported Features Count
- **4** Input Methods: Telex, VNI, Simple Telex 1 & 2
- **5** Code Tables: Unicode, TCVN3, VNI-Windows, Unicode Compound, CP 1258
- **20+** Configuration Options (global variables)
- **Unlimited** Macro Expansions (vs macOS 20-char limit)

## 🎯 Current Project Status

**🟢 Stable and Production Ready** - All major crashes and buffer overflows have been fixed.

### Recent Major Changes (2025-10-24)
- ✅ **NEW: Fixed buffer desync with Claude Code CLI** - Giải quyết race condition causing mất chữ khi gõ tiếng Việt
  - Enhanced buffer reset logic với complete state clear
  - Added buffer validation & desync detection mechanisms
  - Implemented graceful error recovery - no more cascade failures
  - 4 new functions: `validateBufferState()`, `detectBufferDesync()`, `emergencyReset()`, enhanced `startNewSession()`
- ✅ Complete OpenKey → EndKey migration
- ✅ Fixed 21+ buffer overflow vulnerabilities in [Engine.cpp](Sources/EndKey/engine/Engine.cpp)
- ✅ Removed unstable Smart Switch Key feature
- ✅ Updated bundle identifier to `com.tuyenmai.endkey`
- ✅ Fixed deployment target compatibility (macOS 10.13+)
- ✅ Improved accessibility permissions handling
- ✅ Added comprehensive bounds checking for memory safety
- ✅ Deep codebase analysis and comprehensive CLAUDE.md update

## 🔄 Sync with Upstream (Recommended!)

### Why Sync?
This fork is **2 versions behind** and missing critical updates:
- Apple Silicon (M1) support
- macOS Big Sur+ compatibility improvements
- Windows stability fixes

### How to Sync

```bash
# Add upstream remote (one-time setup)
git remote add upstream https://github.com/tuyenvm/OpenKey.git

# Fetch latest from upstream
git fetch upstream

# View what's new
git log HEAD..upstream/master --oneline

# Merge upstream changes
git checkout main
git merge upstream/master

# Resolve conflicts if any, then push
git push origin main
```

### Alternative: Fresh Clone from Upstream
```bash
# Clone official repository
git clone https://github.com/tuyenvm/OpenKey.git
cd OpenKey

# Check version
cat version.json
```

## 🔗 Resources

- **Official Repository**: https://github.com/tuyenvm/OpenKey ⭐
- **This Fork**: https://github.com/dtateks/EndKey (Outdated - v2.0.3)
- **Latest Release**: https://github.com/tuyenvm/OpenKey/releases (v2.0.5)
- **Documentation**: [README.md](README.md), [CHANGELOG.md](CHANGELOG.md)
- **Build Guide**: [macOS_Build.md](macOS_Build.md)
- **Website**: http://open-key.org
- **Homebrew**: `brew install --cask openkey`

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

**Last Updated**: 2025-10-24 (Buffer Desync Fix + Enhanced Error Recovery)
**Fork Status**: ⚠️ OUTDATED - 2 versions behind upstream (v2.0.3 vs v2.0.5)
**Upstream Status**: 🟢 Active - Latest v2.0.5 (March 2022)
**Current Branch**: feature_fix_buffer_reset
**Project**: OpenKey/EndKey Vietnamese Input Method Engine
**Documentation Version**: 2.1 (Comprehensive + Buffer Desync Fix)