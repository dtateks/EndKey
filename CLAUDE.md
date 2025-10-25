# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

**EndKey** - Bộ gõ tiếng Việt nguồn mở cho macOS, sử dụng kỹ thuật `Backspace` để loại bỏ lỗi gạch chân khó chịu ở bộ gõ mặc định. Hoàn toàn miễn phí và là nguồn mở dưới giấy phép GPL.

**Lưu ý quan trọng**: Khi sử dụng EndKey, nên tắt hẳn bộ gõ khác vì 2 chương trình bộ gõ sẽ xung đột nhau, dẫn đến thao tác không chính xác.

## Features

- **Modern orthography** (On/Off) - Đặt dấu oà, uý thay vì òa, úy
- **Quick Telex** (On/Off) - Gõ nhanh (cc=ch, gg=gi, kk=kh, nn=ng, qq=qu, pp=ph, tt=th)
- **Grammar check** (On/Off) - Kiểm tra ngữ pháp
- **Spelling check** (On/Off) - Kiểm tra chính tả
- **Restore key if invalid word** (on/off) - Phục hồi phím với từ sai
- **Run on startup** (On/Off) - Chạy cùng macOS
- **Gray menu bar icon** (On/Off) - Biểu tượng xám trên thanh menu phù hợp với chế độ Dark mode
- **Switch input mode by shortcut key** - Đổi chế độ gõ bằng phím tắt tùy chọn
- **Autocorrect fixed** (On/Off) - Sửa lỗi autocorrect trên trình duyệt như Chrome, Safari, Firefox, Microsoft Excel
- **Underline issue fixed on macOS** (On/Off) - Sửa lỗi gạch chân trên macOS
- **Tạm tắt kiểm tra chính tả bằng phím Ctrl** (On/Off) (Bản 1.5 về sau)
- **Tạm tắt EndKey bằng phím Cmd/Alt** (On/Off) (Bản 2.0.1 về sau)
- **Cho phép dùng f z w j làm phụ âm đầu** (On/Off) (Bản 1.5 về sau)
- **Gõ tắt phụ âm đầu: f->ph, j->gi, w->qu** (On/Off) (Bản 1.6 về sau)
- **Gõ tắt phụ âm cuối: g->ng, h->nh, k->ch** (On/Off) (Bản 1.6 về sau)
- **Hiện biểu tượng trên thanh Dock** (On/Off) (Bản 2.0.1 về sau)
- **Macro** - Tính năng gõ tắt vô cùng tiện lợi, không giới hạn ký tự
- **Chuyển chế độ thông minh** (On/Off) - Tự động chuyển chế độ gõ theo ứng dụng
- **Viết Hoa chữ cái đầu câu** (On/Off) - Tự động ghi hoa chữ cái đầu câu
- **Chế độ "Gửi từng phím"** (On/Off) - Phù hợp với ứng dụng không tương thích
- **Cập nhật tự động** - Tự động cập nhật phiên bản mới nhất
- **Công cụ chuyển mã** - Hỗ trợ chuyển mã qua lại văn bản
- **Tự ghi nhớ bảng mã theo ứng dụng** - Phù hợp cho Photoshop, CAD,...

## Supported Input Methods

- Telex
- VNI
- Simple Telex

## Character Encodings

- Unicode (Unicode dựng sẵn)
- TCVN3 (ABC)
- VNI Windows
- Unicode Compound (Unicode tổ hợp)
- Vietnamese Locale CP 1258

## Installation Methods

**Cài đặt thủ công:**
```bash
# Tải bản EndKey mới nhất từ releases, mở file dmg và kéo thả EndKey.app vào thư mục Application
```

**Cài bằng Homebrew:**
```bash
# Kiểm tra phiên bản
brew info --cask openkey

# Cài đặt
brew install --cask openkey

# Cập nhật
brew upgrade --cask openkey
```

## Author & Contact

- **Author**: Mai Vũ Tuyên
- **Email**: maivutuyen.91@gmail.com
- **Fanpage**: https://www.facebook.com/EndKeyVN

**Complete rebuild workflow - use this for most development**:
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

**Không có** CMakeLists.txt, Makefile, hay build script riêng - toàn bộ build qua Xcode.

## Architecture Overview

**Core Engine (C++)** - `Sources/EndKey/engine/`
- `Engine.h/cpp`: Main entry point, API interface
- `Vietnamese.h/cpp`: Xử lý tiếng Việt, các kiểu gõ (Telex, VNI)
- `Macro.h/cpp`: Tính năng gõ tắt, macro
- `ConvertTool.h/cpp`: Chuyển mã giữa các bảng mã (Unicode, TCVN3, VNI)
- `SmartSwitchKey.h/cpp`: Chuyển ứng dụng thông minh
- `DataType.h`: Định nghĩa data structures và constants

**macOS UI (Objective-C++)** - `Sources/EndKey/macOS/ModernKey/`
- `EndKey.mm`: Main application entry
- `EndKeyManager.m`: Quản lý trạng thái, preferences
- `ViewController.m/.h`: Main UI controller
- `AppDelegate.m/.h`: Application lifecycle
- Các view controllers khác: `MacroViewController`, `AboutViewController`, `ConvertToolViewController`

**Helper App** - `Sources/EndKey/macOS/EndKeyHelper/`
- App nhỏ chạy ở background cho startup/login items
- Cần riêng biệt vì sandboxing requirements

## Key Files

- `Sources/EndKey/engine/Engine.h`: **QUAN TRỌNG** - API definitions, global variables
- `Sources/EndKey/macOS/ModernKey/EndKey.entitlements`: Quyền system cần thiết
- `Sources/EndKey/macOS/EndKeyHelper/EndKeyHelper.entitlements`: Quyền helper app
- `Sources/EndKey/version.json`: Version information

## Development Workflow

**Môi trường phát triển**:
- macOS required
- Xcode là build system chính
- C++17 cho engine, Objective-C++ cho UI

**Testing**:
- Không có automated tests hiện có
- Testing thủ công qua UI

**Deployment**:
- Build qua Xcode -> tạo .app bundle
- Code signing và notarization cần thiết cho distribution

## System Permissions

**Quan trọng**: EndKey cần các quyền hệ thống:
1. **Accessibility** (System Preferences → Security & Privacy → Accessibility)
   - Bắt buộc để capture keyboard events
   - User phải cấp quyền thủ công

2. **Apple Events** (trong entitlements)
   - Cho communication với EndKeyHelper
   - Auto-start functionality

**Không bao giờ tắt quyền Accessibility khi đang dùng EndKey**.

## Code Style Notes

- Engine code follows C-style patterns trong C++
- UI code sử dụng Objective-C với modern syntax
- String handling qua UTF-8 conversion utilities
- Memory management chủ yếu qua ARC (UI) và manual (engine)

## Platform Support

Codebase hỗ trợ:
- **macOS** (primary) - Complete implementation
- **Windows** - Partial (`platforms/win32.h`)
- **Linux** - Partial (`platforms/linux.h`)

Focus development trên macOS target.

---

## Developer Context

### Working Style

- **Method**: GitHub Spec Kit với Spec-Driven Development
- **Coding Philosophy**: 100% Vibe Code - tập trung vào trải nghiệm người dùng
- **Code Review**: Không đọc code, chỉ feedback qua UI/UX

### Communication Preferences

- **Language**: Luôn giao tiếp Tiếng Việt, ngắn gọn, súc tích, dễ hiểu, không show code
- **Decision Making**: Coding Agent tự quyết định các thay đổi nhỏ
- **Feedback**: Ưu tiên giải pháp thực tế thay vì phân tích kỹ thuật
- **Updates**: Báo cáo tiến độ ngắn gọn, tập trung vào kết quả

### MCP Server Guide

**gkg**

- Analyze codebase
- Map code dependencies
- Understand architecture
- Trace code flow across files
- Refactoring decisions
- MUST run cli gkg server stop > gkg clean > gkg index > gkg server start > reload agent mcp server BEFORE use to get newest codebase

**context7**

- Library/framework API documentation
- Version-specific features
- Code examples from official sources
- BEFORE implementing with unfamiliar libraries

**playwright**

- Open browser for UI testing and debugging
- Performance analysis (Core Web Vitals)
- Screenshot/video capture
- Console errors and network inspection
- ONLY when browser rendering required

**sequential-thinking**
- thinking, planning, refactoring, debugging, researching, analyzing,..
- ALWAYS start complex/multi-step tasks with sequential-thinking BEFORE taking action

**zai-mcp-server**

- Image/Video Analysis
- UI design review
- Layout extraction
- OCR text extraction

**web-search-prime**

- Web Research, finding latest info
- Current events/releases, error message
- Community discussions (Reddit, Stack Overflow, GitHub issues)
- Library comparisons
- When context7 doesn't have docs

---
