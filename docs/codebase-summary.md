# EndKey Codebase Summary

**Generated:** 2025-11-26
**Phase:** 02 - Core Engine (Completed)

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
- **Purpose:** Core EventTap setup and lifecycle management
- **Key Features:**
  - Accessibility permission checking with retry logic (max 5 attempts)
  - KeyboardManager integration for Vietnamese input
  - Error recovery dialogs for permission/EventTap failures
  - Graceful cleanup on termination
- **Architecture:**
  - Delegates keyboard handling to `KeyboardManager.shared`
  - Permission flow: check → request → retry → alert → setup
- **Lines:** 89

### 3. Core/VietnameseData.swift
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

### 4. Core/InputEngine.swift
- **Purpose:** Protocol and types for input engines
- **Components:**
  - `ProcessResult`: Result struct with backspaceCount + replacement string
  - `InputEngine` protocol: `processKey(_:)`, `reset()`, `buffer` property
  - `InputMethod` enum: `.telex` and `.vni` cases
- **Architecture:** Protocol-based design for extensibility
- **Lines:** 31

### 5. Core/TelexEngine.swift
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

### 6. Core/VNIEngine.swift
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

### 7. Core/KeyboardManager.swift
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

### 8. Utils/PermissionHelper.swift
- **Purpose:** Accessibility permission utilities
- **Capabilities:**
  - Check if Accessibility is enabled: `isAccessibilityEnabled`
  - Request permission with system prompt: `requestAccessibility()`
  - Open System Preferences: `openAccessibilityPreferences()`
- **Architecture:** Static utility struct wrapping AXIsProcessTrusted APIs
- **Lines:** 18

### 9. Info.plist
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

⏳ **Deferred to Phase 03:**
- UI/StatusBar for mode switching and settings
- Keyboard shortcuts for toggle Vietnamese mode
- Settings panel for customization
- App icon and menu bar icon

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

## Code Statistics

- **Total Swift Files:** 8
- **Total Lines:** ~657 (excluding comments/blank lines)
- **Configuration Files:** 1 (Info.plist)
- **Core Modules:** 5 (VietnameseData, InputEngine, TelexEngine, VNIEngine, KeyboardManager)
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

## Next Steps (Phase 03)

1. Add menu bar icon with status indicator
2. Implement keyboard shortcuts (Cmd+Shift+V to toggle Vietnamese mode)
3. Create settings panel for input method switching
4. Add visual feedback for mode status
5. Implement auto-save for user preferences
6. Add app icon and bundle resources

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

- **Phase 01 Testing:** Manual verification of app launch, permission flow, EventTap setup ✅
- **Phase 02 Testing:** Manual verification of Vietnamese input (Telex/VNI), tone placement, case preservation ✅
- **Test Cases Verified:**
  - Telex transformations: aa→â, aw→ă, dd→đ, tone marks (s/f/r/x/j)
  - VNI transformations: d9→đ, a6→â, a7→ă, tone marks (1-5)
  - Tone placement: modified vowels, ending consonants, penultimate vowels
  - Case preservation: Uppercase input → uppercase output
  - Word boundaries: Buffer reset on space/enter/punctuation
  - Mode toggle: Vietnamese on/off functionality
- **Automated Tests:** Not implemented (future consideration)
- **User Testing:** Not started

---

**Document Version:** 2.0
**Last Updated:** 2025-11-26 (Phase 02 Core Engine completion)
**Maintained By:** Technical Documentation Agent
