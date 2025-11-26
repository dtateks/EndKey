# EndKey Codebase Summary

**Generated:** 2025-11-26
**Phase:** 03 - UI Components (Completed)

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
│   ├── AppDelegate.swift      # EventTap setup, menu bar & global hotkey
│   ├── Models/
│   │   └── AppState.swift     # Shared state with @AppStorage
│   ├── UI/
│   │   ├── MenuBarManager.swift   # Menu bar status item & dropdown
│   │   └── ConfigView.swift       # SwiftUI preferences panel
│   ├── Core/                  # Vietnamese input engine
│   │   ├── VietnameseData.swift    # Character mappings & data
│   │   ├── InputEngine.swift       # Protocol & types
│   │   ├── TelexEngine.swift       # Telex input method
│   │   ├── VNIEngine.swift         # VNI input method
│   │   └── KeyboardManager.swift   # EventTap manager
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
- **Purpose:** Core EventTap setup, menu bar & global hotkey management
- **Key Features:**
  - Accessibility permission checking with retry logic (max 5 attempts)
  - KeyboardManager integration for Vietnamese input
  - MenuBarManager integration for status item UI
  - Global hotkey (Cmd+Shift) for mode toggle
  - Error recovery dialogs for permission/EventTap failures
  - Graceful cleanup on termination
- **Architecture:**
  - Delegates keyboard handling to `KeyboardManager.shared`
  - Delegates menu bar to `MenuBarManager`
  - Uses NSEvent.addGlobalMonitorForEvents for hotkey detection
  - Permission flow: check → request → retry → alert → setup
- **Lines:** 125

### 3. Models/AppState.swift
- **Purpose:** Singleton shared state with UserDefaults persistence
- **Key Features:**
  - @AppStorage for automatic persistence (isVietnameseMode, inputMethodRaw, launchAtLogin)
  - Computed property `inputMethod` for type-safe access
  - Bidirectional sync with KeyboardManager via didSet
  - NotificationCenter.modeChanged for UI updates
- **State Properties:**
  - `isVietnameseMode: Bool` - Vietnamese/English toggle (default: true)
  - `inputMethod: InputMethod` - Telex/VNI selection (default: .telex)
  - `launchAtLogin: Bool` - Launch at login preference (default: false)
- **Architecture:**
  - ObservableObject with singleton pattern (`AppState.shared`)
  - Private init syncs initial state to KeyboardManager
  - State changes trigger KeyboardManager updates + UI refresh
- **Lines:** 46

### 4. UI/MenuBarManager.swift
- **Purpose:** Menu bar status item and dropdown menu management
- **Key Features:**
  - Status item with V/E icon (Vietnamese/English indicator)
  - Dropdown menu: Mode toggle, Input method submenu, Preferences, Quit
  - Dynamic menu rebuild on mode changes via NotificationCenter
  - ConfigView preferences window (NSHostingView wrapper)
- **Menu Items:**
  - Vietnamese Mode toggle (checkmark state)
  - Input Method submenu (Telex/VNI radio selection)
  - Preferences panel (Cmd+,)
  - Quit EndKey (Cmd+Q)
- **Architecture:**
  - NSStatusItem management with NSMenu
  - SwiftUI ConfigView wrapped in NSWindow + NSHostingView
  - Singleton window pattern for preferences (reuse if exists)
  - NotificationCenter observer for .modeChanged
- **Lines:** 127

### 5. UI/ConfigView.swift
- **Purpose:** SwiftUI preferences panel for app configuration
- **Key Features:**
  - Input method picker (Telex/VNI segmented control)
  - Launch at login toggle with SMAppService integration
  - Keyboard shortcut display (⌘ + ⇧)
  - App version footer
- **Sections:**
  - Input Method: Segmented picker for Telex/VNI
  - Settings: Launch at login toggle
  - Shortcut: Read-only display of global hotkey
  - Footer: App version info
- **LaunchAtLoginHelper:**
  - Uses SMAppService.mainApp (macOS 13+)
  - Falls back to SMLoginItemSetEnabled (older macOS)
- **Architecture:**
  - @ObservedObject binding to AppState.shared
  - Binding wrappers for computed properties
  - Fixed size: 300x220 points
- **Lines:** 84

### 6. Core/VietnameseData.swift
- **Purpose:** Central data store for Vietnamese character mappings
- **Data Structures:**
  - `baseVowels`: Set of base vowels (a, e, i, o, u, y)
  - `modifiedVowels`: Set of vowels with diacritics (ă, â, ê, ô, ơ, ư)
  - `toneMarks`: Mapping base vowels to 5 tone marks (1-5 → sắc, huyền, hỏi, ngã, nặng)
  - `telexTones`: Letter keys for Telex tones (s, f, r, x, j)
  - `vowelTransforms`: Transformation rules (aa→â, aw→ă, a6→â, etc.)
  - `tonedToBase`: Reverse mapping from toned to base vowels
- **Utilities:**
  - `getBaseVowel(_:)`: Extract base vowel from toned character
  - `isVowel(_:)`: Check if character is Vietnamese vowel
- **Lines:** 74

### 7. Core/InputEngine.swift
- **Purpose:** Protocol and types for input engines
- **Components:**
  - `ProcessResult`: Result struct with backspaceCount + replacement string
  - `InputEngine` protocol: `processKey(_:)`, `reset()`, `buffer` property
  - `InputMethod` enum: `.telex` and `.vni` cases
- **Architecture:** Protocol-based design for extensibility
- **Lines:** 31

### 8. Core/TelexEngine.swift
- **Purpose:** Telex input method implementation
- **Rules:**
  - Doubling: aa→â, aw→ă, ee→ê, oo→ô, ow→ơ, uw→ư
  - dd→đ transformation
  - Tone keys: s/f/r/x/j for 5 tones
- **Key Logic:**
  - Tone placement: modified vowels > ending consonant > penultimate vowel
  - Buffer management: max 7 characters (longest Vietnamese syllable)
  - Case preservation: uppercase detection + preservation
- **Architecture:**
  - `processKey(_:)`: Process character → return transformation result
  - `applyTone(_:)`: Find vowel position + apply tone
  - `findTonePosition()`: Vietnamese tone placement rules
- **Lines:** 129

### 9. Core/VNIEngine.swift
- **Purpose:** VNI input method implementation
- **Rules:**
  - Numbers 1-5 for tones
  - Number 6 for circumflex (â, ê, ô)
  - Number 7 for breve/horn (ă, ơ, ư)
  - d9→đ transformation
- **Key Logic:**
  - Same tone placement rules as TelexEngine
  - Vowel modifier application: find last applicable vowel
  - Number key handling: tone marks vs modifiers vs regular numbers
- **Architecture:**
  - Shares tone placement logic with TelexEngine
  - `applyVowelModifier(_:)`: Backward search for applicable vowel
- **Lines:** 134

### 10. Core/KeyboardManager.swift
- **Purpose:** Singleton manager for EventTap and input processing
- **Key Features:**
  - EventTap lifecycle: setup, cleanup, re-enable on timeout
  - Vietnamese mode toggle: `isVietnameseMode`, `toggleVietnameseMode()`
  - Input method switching: `.telex` ↔ `.vni`
  - Engine state management: reset on word boundaries
- **Event Processing:**
  - Filter modifier keys (skip Cmd, Ctrl, Alt)
  - Word boundary detection (space, enter, punctuation)
  - Character extraction from CGEvent
  - Transformation via InputEngine
  - Backspace + replacement string injection
- **Key Simulation:**
  - `sendBackspaces(count:)`: CGEvent backspace simulation
  - `sendString(_:)`: Unicode string injection via CGEvent
- **Architecture:**
  - Singleton pattern: `KeyboardManager.shared`
  - EventTap callback runs on dedicated thread (synchronous event sending)
- **Lines:** 187

### 11. Utils/PermissionHelper.swift
- **Purpose:** Accessibility permission utilities
- **Capabilities:**
  - Check if Accessibility is enabled: `isAccessibilityEnabled`
  - Request permission with system prompt: `requestAccessibility()`
  - Open System Preferences: `openAccessibilityPreferences()`
- **Architecture:** Static utility struct wrapping AXIsProcessTrusted APIs
- **Lines:** 18

### 12. Info.plist
- **Key Configurations:**
  - `LSUIElement: true` - Runs as background agent (no dock icon/menu bar)
  - `NSAccessibilityUsageDescription` - Permission request message
  - `LSMinimumSystemVersion` - macOS 11.0+
  - Bundle identifier, version, copyright metadata
- **Lines:** 35

## Implementation Status

✅ **Phase 01 - Foundation (Completed):**
- SwiftUI app structure with AppDelegate adapter
- EventTap skeleton setup with keyDown event capture
- Accessibility permission flow with retry mechanism (max 5)
- Error recovery dialogs for permission/EventTap failures
- Info.plist configuration (LSUIElement, permissions)
- Project structure and build configuration

✅ **Phase 02 - Core Engine (Completed):**
- Vietnamese character data store with all mappings
- InputEngine protocol with ProcessResult architecture
- TelexEngine implementation with tone placement rules
- VNIEngine implementation with number key handling
- KeyboardManager singleton with EventTap lifecycle
- Vietnamese mode toggle functionality
- Input method switching (Telex ↔ VNI)
- Backspace + replacement string injection
- Word boundary detection and buffer reset
- Case preservation for uppercase letters

✅ **Phase 03 - UI Components (Completed):**
- AppState singleton with @AppStorage persistence
- MenuBarManager with V/E status icon
- Dropdown menu with mode toggle, input method, preferences, quit
- ConfigView SwiftUI preferences panel
- Global hotkey (Cmd+Shift) for mode toggle
- Launch at login integration (SMAppService)
- NotificationCenter-based UI updates
- Keyboard shortcut display in preferences

⏳ **Future Enhancements:**
- Custom keyboard shortcuts configuration
- App icon and asset catalog
- Advanced preferences (exclude apps, custom shortcuts)
- Statistics and usage tracking

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

5. **Protocol-Based Engine Architecture:**
   - `InputEngine` protocol for extensibility (future: VIQR, custom methods)
   - Shared tone placement logic across engines
   - ProcessResult architecture: backspaceCount + replacement string
   - Buffer-based state management (max 7 chars per syllable)

6. **Singleton KeyboardManager:**
   - Centralized EventTap lifecycle management
   - Thread-safe event processing (EventTap callback on dedicated thread)
   - Synchronous key simulation to avoid race conditions
   - Clean API: toggle mode, switch method, setup, cleanup

7. **Vietnamese Tone Placement Rules:**
   - Priority 1: Modified vowels (ă, â, ê, ô, ơ, ư) always get tone
   - Priority 2: Syllable ending with consonant → tone on last vowel
   - Priority 3: No ending consonant → tone on penultimate vowel
   - Handles edge cases: single vowel, double vowels, complex syllables

8. **AppState Persistence:**
   - @AppStorage for automatic UserDefaults sync
   - Bidirectional binding: AppState ↔ KeyboardManager
   - NotificationCenter for decoupled UI updates
   - Singleton pattern for global state access

9. **Menu Bar UI:**
   - NSStatusItem for macOS-native menu bar integration
   - V/E icon for quick mode identification
   - Dynamic menu rebuilds on state changes
   - SwiftUI preferences wrapped in NSHostingView

10. **Global Hotkey:**
    - NSEvent.addGlobalMonitorForEvents for system-wide detection
    - Cmd+Shift combination (no third key needed)
    - State tracking to avoid double-toggle on key repeat
    - Works even when app is not frontmost

## Code Statistics

- **Total Swift Files:** 12
- **Total Lines:** ~1,040 (excluding comments/blank lines)
- **Configuration Files:** 1 (Info.plist)
- **Core Modules:** 5 (VietnameseData, InputEngine, TelexEngine, VNIEngine, KeyboardManager)
- **UI Modules:** 2 (MenuBarManager, ConfigView)
- **Models:** 1 (AppState)
- **Utilities:** 1 module (PermissionHelper)
- **Protocols:** 1 (InputEngine)
- **Enums:** 1 (InputMethod)

## Build Configuration

- **Deployment Target:** macOS 11.0+
- **Build System:** Xcode project (xcodeproj)
- **Bundle Identifier:** (configured in project settings)
- **Version:** 1.0 (build 1)

## Technical Highlights

### Vietnamese Input Processing Flow
1. **Event Capture:** CGEvent keyDown intercepted by EventTap
2. **Filtering:** Skip if modifier keys (Cmd/Ctrl/Alt) or Vietnamese mode off
3. **Character Extraction:** Unicode character from CGEvent
4. **Word Boundary Check:** Reset buffer on space/enter/punctuation
5. **Engine Processing:** Character passed to TelexEngine/VNIEngine
6. **Transformation:** Engine returns ProcessResult (backspaceCount, replacement)
7. **Injection:** Send backspaces + replacement string via CGEvent simulation
8. **Event Suppression:** Return nil to suppress original event

### Engine Processing Pipeline (Telex)
1. **Tone Mark Check:** s/f/r/x/j → apply tone to correct vowel
2. **dd Check:** d + d → đ transformation
3. **Vowel Transform:** aa/aw/ow/uw/ee/oo → ă/â/ê/ô/ơ/ư
4. **Buffer Append:** Regular character → add to buffer
5. **Buffer Trim:** Keep max 7 characters

### Engine Processing Pipeline (VNI)
1. **d9 Check:** d + 9 → đ transformation
2. **Tone Mark Check:** 1-5 → apply tone to correct vowel
3. **Vowel Modifier:** 6/7 → a6→â, a7→ă, o7→ơ, etc.
4. **Buffer Append:** Regular character → add to buffer
5. **Buffer Trim:** Keep max 7 characters

## UI Architecture (Phase 03)

### State Management Flow
```
User Action (Menu/Hotkey)
    ↓
AppState.toggleMode() / AppState.inputMethod = .telex
    ↓
@AppStorage didSet triggers
    ↓
┌─────────────────────────┬───────────────────────────┐
│ KeyboardManager.shared  │ NotificationCenter.post   │
│ .setVietnameseMode()    │ (.modeChanged)            │
└─────────────────────────┴───────────────────────────┘
    ↓                           ↓
EventTap filters input    MenuBarManager updates icon/menu
```

### UI Components Interaction
```
MenuBarManager (NSStatusItem)
    ├── Status Icon (V/E)
    ├── Menu Items
    │   ├── Vietnamese Mode Toggle → AppState.toggleMode()
    │   ├── Input Method Submenu → AppState.inputMethod = method
    │   ├── Preferences → Opens ConfigView window
    │   └── Quit → NSApp.terminate()
    └── Observer: .modeChanged → updateIcon() + setupMenu()

ConfigView (SwiftUI)
    ├── @ObservedObject appState
    ├── Input Method Picker → Binding to appState.inputMethod
    ├── Launch at Login Toggle → SMAppService.mainApp.register/unregister
    └── Keyboard Shortcut Display (read-only)

AppDelegate
    ├── Global Hotkey (Cmd+Shift) → AppState.toggleMode()
    ├── MenuBarManager lifecycle
    └── KeyboardManager setup
```

### Persistence Layer
- **UserDefaults Keys:**
  - `isVietnameseMode: Bool` (default: true)
  - `inputMethod: String` (default: "telex")
  - `launchAtLogin: Bool` (default: false)
- **Sync on Launch:** AppState.init() reads UserDefaults → syncs to KeyboardManager
- **Sync on Change:** @AppStorage didSet → updates KeyboardManager + posts notification

## Dependencies

- **System Frameworks:**
  - Cocoa.framework (AppKit, NSStatusItem, Accessibility APIs)
  - Carbon.framework (CGEvent, EventTap)
  - SwiftUI.framework (ConfigView UI layer)
  - ServiceManagement.framework (Launch at login - SMAppService)

- **External Dependencies:** None (pure Swift/system frameworks)

## Security & Privacy

- **Accessibility Permission:** Required for EventTap keyboard capture
- **Data Collection:** None (local-only processing)
- **Network Access:** None
- **Sandboxing:** Not sandboxed (required for EventTap)

## Testing Status

- **Phase 01 Testing:** Manual verification of app launch, permission flow, EventTap setup ✅
- **Phase 02 Testing:** Manual verification of Vietnamese input (Telex/VNI), tone placement, case preservation ✅
- **Phase 03 Testing:** Manual verification of UI components, menu bar, preferences, global hotkey ✅
- **Test Cases Verified:**
  - Telex transformations: aa→â, aw→ă, dd→đ, tone marks (s/f/r/x/j)
  - VNI transformations: d9→đ, a6→â, a7→ă, tone marks (1-5)
  - Tone placement: modified vowels, ending consonants, penultimate vowels
  - Case preservation: Uppercase input → uppercase output
  - Word boundaries: Buffer reset on space/enter/punctuation
  - Mode toggle: Vietnamese on/off via menu + global hotkey (Cmd+Shift)
  - Menu bar icon: V/E status indicator updates correctly
  - Input method switching: Telex ↔ VNI via menu
  - Preferences panel: Opens, closes, persists settings
  - Launch at login: SMAppService registration/unregistration
  - State persistence: UserDefaults sync on app restart
- **Automated Tests:** Not implemented (future consideration)
- **User Testing:** Not started

---

**Document Version:** 3.0
**Last Updated:** 2025-11-26 (Phase 03 UI Components completion)
**Maintained By:** Technical Documentation Agent
