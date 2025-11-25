# Phase 01: Foundation

## Context Links
- Parent: [plan.md](plan.md)
- Reference: [Brainstorm Summary](../251126-endkey-brainstorm-summary.md)

## Overview
- **Priority:** P0 (Critical)
- **Status:** DONE (2025-11-26)
- **Description:** Setup Xcode project, AppDelegate, EventTap skeleton, Accessibility permission flow
- **Completion Date:** 2025-11-26
- **Review Report:** [251126-code-reviewer-phase01-comprehensive-review.md](../phase-01-foundation/reports/251126-code-reviewer-phase01-comprehensive-review.md)

## Key Insights
- macOS EventTap requires Accessibility permission
- SwiftUI @main needs AppDelegate adapter for EventTap lifecycle
- Info.plist must declare LSUIElement=true for menubar-only app

## Requirements

### Functional
- Xcode project compiles and runs
- EventTap captures keyboard events
- Permission prompt appears on first launch
- App runs as menubar-only (no dock icon)

### Non-functional
- macOS 11.0+ deployment target
- Swift 5.5+ language version

## Architecture

```
┌─────────────────┐
│  EndKeyApp      │ @main SwiftUI entry
│  (SwiftUI)      │
└────────┬────────┘
         │ @NSApplicationDelegateAdaptor
         ▼
┌─────────────────┐
│  AppDelegate    │ EventTap setup, permission check
│  (AppKit)       │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│ PermissionHelper│ Accessibility API wrapper
└─────────────────┘
```

## Related Code Files

### Create
| File | Description |
|------|-------------|
| EndKey/EndKeyApp.swift | @main SwiftUI app entry |
| EndKey/AppDelegate.swift | NSApplicationDelegate with EventTap |
| EndKey/Utils/PermissionHelper.swift | Accessibility permission helper |
| EndKey/Info.plist | App configuration |
| EndKey.xcodeproj | Xcode project file |

## Implementation Steps

### Step 1: Create Xcode Project
1. Open Xcode → New Project → macOS App
2. Product Name: EndKey
3. Organization: com.endkey
4. Interface: SwiftUI, Language: Swift
5. Deployment Target: macOS 11.0

### Step 2: Configure Info.plist
Add keys:
```xml
<key>LSUIElement</key>
<true/>
<key>NSAccessibilityUsageDescription</key>
<string>EndKey needs Accessibility access to process keyboard input for Vietnamese typing.</string>
```

### Step 3: Create EndKeyApp.swift
```swift
import SwiftUI

@main
struct EndKeyApp: App {
    @NSApplicationDelegateAdaptor(AppDelegate.self) var appDelegate

    var body: some Scene {
        Settings {
            EmptyView()
        }
    }
}
```

### Step 4: Create AppDelegate.swift
```swift
import Cocoa
import Carbon

class AppDelegate: NSObject, NSApplicationDelegate {
    private var eventTap: CFMachPort?

    func applicationDidFinishLaunching(_ notification: Notification) {
        checkAccessibilityPermission()
    }

    private func checkAccessibilityPermission() {
        let options = [kAXTrustedCheckOptionPrompt.takeUnretainedValue() as String: true]
        let trusted = AXIsProcessTrustedWithOptions(options as CFDictionary)

        if trusted {
            setupEventTap()
        } else {
            // Will prompt user, retry after permission granted
            DispatchQueue.main.asyncAfter(deadline: .now() + 2) { [weak self] in
                self?.checkAccessibilityPermission()
            }
        }
    }

    private func setupEventTap() {
        let eventMask = (1 << CGEventType.keyDown.rawValue)

        eventTap = CGEvent.tapCreate(
            tap: .cgSessionEventTap,
            place: .headInsertEventTap,
            options: .defaultTap,
            eventsOfInterest: CGEventMask(eventMask),
            callback: { (proxy, type, event, refcon) -> Unmanaged<CGEvent>? in
                // Skeleton - will be implemented in Phase 2
                return Unmanaged.passRetained(event)
            },
            userInfo: nil
        )

        guard let eventTap = eventTap else {
            print("Failed to create event tap")
            return
        }

        let runLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, eventTap, 0)
        CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, .commonModes)
        CGEvent.tapEnable(tap: eventTap, enable: true)

        print("EventTap setup complete")
    }

    func applicationWillTerminate(_ notification: Notification) {
        if let eventTap = eventTap {
            CGEvent.tapEnable(tap: eventTap, enable: false)
        }
    }
}
```

### Step 5: Create PermissionHelper.swift
```swift
import Cocoa

struct PermissionHelper {
    static var isAccessibilityEnabled: Bool {
        AXIsProcessTrusted()
    }

    static func requestAccessibility() -> Bool {
        let options = [kAXTrustedCheckOptionPrompt.takeUnretainedValue() as String: true]
        return AXIsProcessTrustedWithOptions(options as CFDictionary)
    }

    static func openAccessibilityPreferences() {
        let url = URL(string: "x-apple.systempreferences:com.apple.preference.security?Privacy_Accessibility")!
        NSWorkspace.shared.open(url)
    }
}
```

### Step 6: Test EventTap
1. Build and run
2. Grant Accessibility permission when prompted
3. Check console for "EventTap setup complete"
4. Verify app captures keystrokes (add print statement)

## Todo List
- [x] Create Xcode project with correct settings
- [x] Configure Info.plist (LSUIElement, NSAccessibilityUsageDescription)
- [x] Implement EndKeyApp.swift with AppDelegate adapter
- [x] Implement AppDelegate with EventTap skeleton
- [x] Implement PermissionHelper utility
- [x] Test permission flow and EventTap capture
- [ ] **FIX CRITICAL:** Memory leak - Change passRetained to passUnretained
- [ ] **FIX CRITICAL:** Add retry limit to permission check (prevent infinite loop)
- [ ] **FIX CRITICAL:** Add error recovery for EventTap creation failure
- [ ] **FIX HIGH:** Delete unused PermissionHelper or use it (YAGNI violation)
- [ ] **OPTIONAL:** Add entitlements file for security hardening

## Success Criteria
- [ ] App runs without dock icon (menubar only)
- [ ] Accessibility permission prompt appears
- [ ] EventTap successfully captures keydown events
- [ ] Console shows "EventTap setup complete"

## Risk Assessment
| Risk | Likelihood | Impact | Mitigation |
|------|------------|--------|------------|
| Permission not granted | Medium | High | Clear description, retry loop |
| EventTap fails | Low | High | Error handling, fallback message |

## Security Considerations
- Only request necessary permission (Accessibility)
- Clear usage description in Info.plist
- No data collection or transmission

## Next Steps
→ Phase 02: Core Engine (InputEngine, Telex, VNI)
