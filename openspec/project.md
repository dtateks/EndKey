# Project Context

## Purpose
EndKey là bộ gõ tiếng Việt cho macOS, đang được refactor từ kiến trúc monolithic sang modular C++ hiện đại. Mục tiêu: hiệu năng cao, dễ bảo trì, an toàn bộ nhớ, và 100% tương thích ngược với API legacy thông qua EngineBridge, đồng thời đảm bảo trải nghiệm ổn định (quyền Trợ năng, tự khởi động, UI nhẹ).

## Tech Stack
- Ngôn ngữ: C++14/17 (Engine), Objective‑C++ (macOS app)
- Build: Xcode/xcodebuild; ký mã (codesign) để giữ quyền Trợ năng
- Kiến trúc: Clean architecture (interfaces/core/bridge) với DI, RAII, memory pools, LRU cache ~50K
- Thành phần chính:
  - Engine: TypingEngine, SpellingEngine, VietnameseOptimized, ConvertTool, SmartSwitchKey
  - Bridge: EngineBridge đồng bộ biến global legacy ↔ ConfigurationManager
  - macOS App: EndKeyManager, InputEventManager (global keyboard events), UIManager, ModernKey (Storyboard), EndKeyHelper (login item)
- Nền tảng: macOS (Accessibility permissions cho keyboard monitoring)

## Project Conventions

### Code Style
- Modern C++: ưu tiên smart pointers (std::unique_ptr), RAII, interface-based design, DI.
- Quản lý cấu hình tập trung qua ConfigurationManager; tự đồng bộ biến global legacy.
- Cấu trúc file và tên interface dùng tiền tố I (IEngineCore, IVietnameseProcessor, ...).
- Tối ưu hot path; cấu trúc dữ liệu tiết kiệm bộ nhớ; tránh cấp phát không cần thiết.

### Architecture Patterns
- Clean architecture với phân tách rõ:
  - interfaces/: hợp đồng trừu tượng cho Engine
  - core/: triển khai cụ thể (EngineCore, ConfigurationManager, VietnameseProcessor, MacroProcessor, MemoryManager)
  - EngineBridge: tương thích 100% API legacy, ánh xạ legacy → hệ cấu hình mới
- macOS app tách quản lý sự kiện, UI, và helper khởi động.

### Testing Strategy
- Test theo module nhờ kiến trúc tách lớp; mock qua interface.
- Kiểm thử tương thích ngược: hành vi API legacy không đổi.
- Ưu tiên unit test cho pipeline nhập liệu và kiểm tra chính tả Vietnamese.

### Git Workflow
- Nhánh mặc định: main.
- Áp dụng OpenSpec: mọi tính năng mới/đổi kiến trúc/breaking/perf thay đổi hành vi cần proposal dưới openspec/changes/[change-id]/ với validate nghiêm ngặt trước khi implement.
- Sửa lỗi, chỉnh format, deps không breaking có thể thực hiện trực tiếp.

## Domain Context
- Bộ gõ tiếng Việt với pipeline: xử lý phím, chuyển đổi ký tự tiếng Việt, kiểm tra chính tả, macro/text expansion, và logic chuyển ngôn ngữ (SmartSwitchKey).
- Yêu cầu xử lý Unicode/encoding chính xác và hiệu quả.

## Important Constraints
- macOS yêu cầu quyền Trợ năng (Accessibility) cho global keyboard monitoring; cần codesign để quyền được giữ ổn định.
- Duy trì 100% tương thích ngược qua EngineBridge.
- Mục tiêu hiệu năng: tải lười bảng chuyển đổi, cache ký tự 50K, tối ưu đường nóng.

## External Dependencies
- Hệ thống Trợ năng macOS cho theo dõi sự kiện bàn phím.
- Cơ chế Login Item của macOS (EndKeyHelper) cho auto-start.
- Không phụ thuộc dịch vụ mạng bên ngoài.
