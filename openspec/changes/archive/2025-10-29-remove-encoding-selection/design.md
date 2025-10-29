## Context
Tính năng lựa chọn bảng mã tồn tại ở UI và logic Engine. Mục tiêu chuẩn hóa xử lý sang Unicode-only để giảm phức tạp và rủi ro tương thích.

## Goals / Non-Goals
- Goals: Unicode-only; xóa UI selector; đơn giản hóa cấu hình và pipeline.
- Non-Goals: Thêm bảng mã mới; hỗ trợ chuyển đổi batch ngoài engine.

## Decisions
- Engine MUST xử lý Unicode duy nhất, không cho phép chọn encoding khác.
- Cấu hình encoding bị loại bỏ; nếu có giá trị tồn tại, bỏ qua.
- UI không hiển thị bất kỳ control chọn bảng mã.

## Risks / Trade-offs
- Thay đổi thói quen người dùng dùng bảng mã legacy → chấp nhận vì macOS/iOS ecosystem chuẩn Unicode.

## Migration Plan
1) Xóa keys/enum encoding khỏi code và UI binding
2) Mặc định Unicode tại thời điểm khởi tạo cấu hình
3) Bỏ qua giá trị cũ trong UserDefaults/plist nếu còn

## Open Questions
- Có cần thông báo one-time trong release notes? (ngoài phạm vi mã nguồn)
