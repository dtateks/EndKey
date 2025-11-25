# EndKey Codebase Summary

**Generated:** 2025-11-26
**Phase:** 01 - Foundation (Completed)

## Project Overview

EndKey is a macOS Vietnamese input application built with Pure Swift/SwiftUI using EventTap architecture (not IME). The app runs as a background agent (LSUIElement) and captures keyboard events to provide Vietnamese typing capabilities.

## Technology Stack

- **Language:** Swift 5+
- **UI Framework:** SwiftUI
- **Architecture:** EventTap (CGEvent)
- **Target:** macOS 11.0+ (Big Sur)
- **Distribution:** Direct download (no notarization required)

## Directory Structure

```
EndKey/
├── EndKey.xcodeproj/          # Xcode project configuration
├── EndKey/                    # Main application source
│   ├── EndKeyApp.swift        # SwiftUI @main entry point
│   ├── AppDelegate.swift      # EventTap setup & permission handling
│   ├── Utils/
│   │   └── PermissionHelper.swift  # Accessibility permission utilities
│   └── Info.plist            # App configuration & permissions
├── docs/                      # Documentation
├── plans/                     # Implementation plans
└── reports/                   # Progress reports
```

## Core Components

### 1. EndKeyApp.swift
- **Purpose:** SwiftUI application entry point
- **Architecture:** Uses `@NSApplicationDelegateAdaptor` to bridge SwiftUI with AppKit
- **Body:** Empty Settings scene (background-only app)
- **Lines:** 13

### 2. AppDelegate.swift
- **Purpose:** Core EventTap setup and lifecycle management
- **Key Features:**
  - Accessibility permission checking with retry logic (max 5 attempts)
  - EventTap creation for keyDown events
  - Error recovery dialogs for permission/EventTap failures
  - Graceful cleanup on termination
- **Architecture:**
  - Uses `CGEvent.tapCreate()` with `.cgSessionEventTap` and `.headInsertEventTap`
  - Callback skeleton ready for Phase 02 Vietnamese input logic
- **Lines:** 112

### 3. PermissionHelper.swift
- **Purpose:** Accessibility permission utilities
- **Capabilities:**
  - Check if Accessibility is enabled: `isAccessibilityEnabled`
  - Request permission with system prompt: `requestAccessibility()`
  - Open System Preferences: `openAccessibilityPreferences()`
- **Architecture:** Static utility struct wrapping AXIsProcessTrusted APIs
- **Lines:** 18

### 4. Info.plist
- **Key Configurations:**
  - `LSUIElement: true` - Runs as background agent (no dock icon/menu bar)
  - `NSAccessibilityUsageDescription` - Permission request message
  - `LSMinimumSystemVersion` - macOS 11.0+
  - Bundle identifier, version, copyright metadata
- **Lines:** 35

## Phase 01 Implementation Status

✅ **Completed:**
- SwiftUI app structure with AppDelegate adapter
- EventTap skeleton setup with keyDown event capture
- Accessibility permission flow with retry mechanism (max 5)
- Error recovery dialogs for permission/EventTap failures
- Info.plist configuration (LSUIElement, permissions)
- Project structure and build configuration

⏳ **Deferred to Phase 02:**
- EventTap callback implementation (Vietnamese input logic)
- Input method switching logic
- Character composition/transformation rules

## Key Design Decisions

1. **EventTap over IME:**
   - Simpler architecture for direct download distribution
   - No Input Method Kit complexity
   - Direct event interception at session level

2. **Background Agent:**
   - `LSUIElement: true` removes dock/menu bar presence
   - Runs silently in background
   - User interacts via keyboard shortcuts (future)

3. **Permission Handling:**
   - Automatic retry with 2-second delays (max 5 attempts)
   - User-friendly dialogs with "Open Preferences", "Retry", "Quit" options
   - Graceful degradation on permission denial

4. **SwiftUI + AppDelegate:**
   - SwiftUI for modern declarative UI (future settings panel)
   - AppDelegate for low-level EventTap integration
   - Clean separation via `@NSApplicationDelegateAdaptor`

## Code Statistics

- **Total Swift Files:** 3
- **Total Lines:** ~143 (excluding comments/blank lines)
- **Configuration Files:** 1 (Info.plist)
- **Utilities:** 1 module (PermissionHelper)

## Build Configuration

- **Deployment Target:** macOS 11.0+
- **Build System:** Xcode project (xcodeproj)
- **Bundle Identifier:** (configured in project settings)
- **Version:** 1.0 (build 1)

## Next Steps (Phase 02)

1. Implement EventTap callback with Vietnamese input logic
2. Add input method switching (Telex/VNI/VIQR)
3. Implement character transformation rules
4. Add tone mark and diacritic handling
5. Create settings panel for configuration

## Dependencies

- **System Frameworks:**
  - Cocoa.framework (AppKit, Accessibility APIs)
  - Carbon.framework (CGEvent, EventTap)
  - SwiftUI.framework (UI layer)

- **External Dependencies:** None (pure Swift/system frameworks)

## Security & Privacy

- **Accessibility Permission:** Required for EventTap keyboard capture
- **Data Collection:** None (local-only processing)
- **Network Access:** None
- **Sandboxing:** Not sandboxed (required for EventTap)

## Testing Status

- **Phase 01 Testing:** Manual verification of app launch, permission flow, EventTap setup
- **Automated Tests:** Not implemented (future consideration)
- **User Testing:** Not started

---

**Document Version:** 1.0
**Last Updated:** 2025-11-26
**Maintained By:** Technical Documentation Agent
