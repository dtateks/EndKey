## Why
Tính năng lựa chọn bảng mã kế thừa làm tăng độ phức tạp UI và logic, gây rủi ro sai khác hành vi và chi phí bảo trì. Chuẩn hóa về Unicode giúp đơn giản hóa cấu hình, giảm lỗi và tối ưu đường xử lý nóng.

## What Changes

### REQUIREMENTS
- **SHALL** remove encoding selection from UI (Preferences, menu, any related toggle).
- **SHALL** remove configuration/synchronization keys related to encoding; default and lock to Unicode.
- **SHALL** make engine process Unicode input/output only; remove conversion to legacy encoding in pipeline.
- **SHALL** handle migration by ignoring old non-Unicode config and using Unicode; no crash, no blocking warning.
- **BREAKING**: users SHALL NOT be able to select encoding other than Unicode.

### SCENARIOS
1. **Scenario**: User opens Preferences
   - **SHALL** not see any encoding selection dropdown/controls
   - **SHALL** only have Vietnamese/English language toggle

2. **Scenario**: User clicks menu bar icon
   - **SHALL** only see "Unicode dựng sẵn" option
   - **SHALL NOT** see TCVN3, VNI Windows, or other encoding options

3. **Scenario**: Application starts with old saved encoding
   - **SHALL** ignore saved value and default to Unicode
   - **SHALL** function normally without errors

4. **Scenario**: User types Vietnamese text
   - **SHALL** always get Unicode output regardless of previous settings
   - **SHALL** maintain all existing Vietnamese input functionality

## Impact
- Affected specs: engine-encoding, ui-encoding.
- Affected code: Engine (ConfigurationManager, ConvertTool/encoding paths, EngineBridge mapping), macOS app (Preferences/UIManager/EndKeyManager binding cho encoding).
