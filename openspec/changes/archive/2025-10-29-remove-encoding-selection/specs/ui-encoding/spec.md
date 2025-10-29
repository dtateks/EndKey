## ADDED Requirements
### Requirement: No Encoding Selector in UI
The macOS application SHALL NOT expose any UI component to select or change text encoding.

#### Scenario: Open Preferences
- **WHEN** the user opens Preferences or related menus
- **THEN** there is no control (menu, toggle, dropdown) for encoding selection

### Requirement: Unicode Default Without Surfacing
UI SHALL assume Unicode as the only mode without surfacing encoding state in settings.

#### Scenario: View settings
- **WHEN** the user reviews settings
- **THEN** encoding is not displayed and cannot be modified

## REMOVED Requirements
### Requirement: UI Encoding Selector
**Reason**: Reduce cognitive load and remove legacy behavior inconsistent with Unicode-only engine.
**Migration**: Remove controls and bindings; any stored UI preference for encoding is ignored.
