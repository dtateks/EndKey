## 1. Implementation
- [x] 1.1 Engine: Khóa cấu hình encoding về Unicode trong ConfigurationManager; xóa enum/keys liên quan
- [x] 1.2 Engine: Loại bỏ nhánh chuyển đổi sang bảng mã legacy trong ConvertTool/pipeline
- [x] 1.3 Bridge: Cập nhật EngineBridge bỏ ánh xạ biến global legacy về encoding
- [x] 1.4 macOS UI: Gỡ control (menu/toggle/segment) chọn bảng mã khỏi Preferences/UIManager
- [x] 1.5 macOS App: Xóa binding/observer tới key encoding; mặc định Unicode
- [x] 1.6 Migration: Bỏ qua giá trị encoding cũ nếu có trong user defaults/plist mà không lỗi
- [x] 1.7 Tests: Thêm test xác nhận không còn đường code chọn encoding khác, và output luôn Unicode
- [x] 1.8 Build & run: xcodebuild Debug; xác minh không còn UI selector; smoke test gõ tiếng Việt

## 2. Validation
- [x] 2.1 openspec validate remove-encoding-selection --strict (nếu CLI sẵn có)
