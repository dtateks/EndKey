# EndKey System Architecture

**Version:** 1.0.0
**Last Updated:** 2025-11-26
**Status:** Production Ready

## Architecture Overview

EndKey employs a **EventTap-based architecture** that intercepts keyboard events at the system level to provide Vietnamese input capabilities without registering as a traditional Input Method Editor (IME). This design choice enables direct distribution while maintaining full system-wide input transformation capabilities.

```
┌─────────────────────────────────────────────────────────────────────┐
│                        System Architecture                          │
├─────────────────────────────────────────────────────────────────────┤
│  User Input Layer                                                   │
│  ├─ Physical Keyboard → CGEvent (keyDown)                        │
│  └─ Global Hotkey (Cmd+Shift) → NSEvent Monitor                  │
│                                                                     │
│  EventTap Layer (CGEvent.tapCreate)                                │
│  ├─ Session-level Event Interception                              │
│  ├─ Modifier Key Filtering (Cmd/Ctrl/Alt)                        │
│  ├─ Word Boundary Detection                                       │
│  └─ Character Extraction                                          │
│                                                                     │
│  Input Processing Layer                                            │
│  ├─ Vietnamese Mode Toggle                                        │
│  ├─ Input Method Selection (Telex/VNI)                           │
│  ├─ Buffer Management (max 7 chars)                              │
│  ├─ Engine Processing (TelexEngine/VNIEngine)                    │
│  ├─ Tone Placement Logic                                          │
│  └─ Transformation Rules                                          │
│                                                                     │
│  Event Simulation Layer                                            │
│  ├─ Backspace Injection (kVK_Delete)                             │
│  ├─ Unicode String Injection (CGEvent keyboardSetUnicodeString)   │
│  └─ Event Suppression (return nil)                               │
│                                                                     │
│  UI Layer                                                          │
│  ├─ Menu Bar (NSStatusItem)                                       │
│  ├─ Settings Panel (SwiftUI)                                      │
│  ├─ Permission Dialogs (NSAlert)                                  │
│  └─ State Persistence (@AppStorage)                               │
└─────────────────────────────────────────────────────────────────────┘
```

## Core Components Architecture

### 1. EventTap Subsystem

**Purpose:** Low-level keyboard event interception at the Core Graphics level

**Key Technologies:**
- `CGEvent.tapCreate()` - Creates event tap for keyboard events
- `CGEvent.tapEnable()` - Enables/disables the event tap
- `CGSessionEventTap` - Session-level event interception
- `CGEvent.keyboardGetUnicodeString()` - Character extraction

**Architecture Details:**
```swift
// EventTap creation with callback
let eventTap = CGEvent.tapCreate(
    tap: .cgSessionEventTap,           // Session-level interception
    place: .headInsertEventTap,        // Insert at head of event stream
    options: .defaultTap,              // Standard behavior
    eventsOfInterest: CGEventMask(1 << CGEventType.keyDown.rawValue),
    callback: { (proxy, type, event, refcon) in
        // Process event on dedicated thread
        return manager.handleKeyEvent(proxy: proxy, type: type, event: event)
    },
    userInfo: refcon
)
```

**Threading Model:**
- EventTap callback executes on dedicated system thread
- Synchronous event simulation to prevent race conditions
- Main thread dispatch for UI updates only

**Error Handling:**
- Automatic re-enable on timeout (`tapDisabledByTimeout`)
- Permission-based failure recovery
- Graceful degradation to passthrough mode

### 2. Input Processing Engine

**Purpose:** Transform Vietnamese input sequences according to Telex and VNI rules

**Architecture Pattern:** Strategy pattern with protocol-based design

```swift
protocol InputEngine {
    mutating func processKey(_ char: Character) -> ProcessResult
    mutating func reset()
    var buffer: [Character] { get }
}
```

**Engine Implementations:**
- **TelexEngine:** Letter-based transformations (aa→â, dd→đ, s/f/r/x/j for tones)
- **VNIEngine:** Number-based transformations (d9→đ, a6→â, 1-5 for tones)

**Processing Pipeline:**
```
Character Input
    ↓
Word Boundary Check (space/punctuation)
    ↓
Buffer Management (max 7 characters)
    ↓
Tone Mark Processing (priority: modified vowels → last vowel → penultimate)
    ↓
Vowel Transformation (aa→â, aw→ă, etc.)
    ↓
Consonant Processing (dd→đ)
    ↓
ProcessResult (backspaceCount + replacement)
```

**Tone Placement Algorithm:**
```swift
func findTonePosition(in buffer: [Character]) -> Int? {
    // Priority 1: Modified vowels (ă, â, ê, ô, ơ, ư)
    if let modifiedIndex = buffer.lastIndex(where: { isModifiedVowel($0) }) {
        return modifiedIndex
    }

    // Priority 2: Last vowel in syllable
    if let lastVowel = buffer.lastIndex(where: { isVowel($0) }) {
        return lastVowel
    }

    // Priority 3: Penultimate vowel
    let vowelIndices = buffer.indices(where: { isVowel($0) })
    if vowelIndices.count >= 2 {
        return vowelIndices[vowelIndices.count - 2]
    }

    return nil
}
```

### 3. State Management System

**Purpose:** Coordinate application state between UI and processing components

**Architecture Pattern:** Observer pattern with persistence

**Core State Properties:**
```swift
class AppState: ObservableObject {
    @AppStorage("isVietnameseMode") var isVietnameseMode: Bool = true
    @AppStorage("inputMethod") var inputMethodRaw: String = InputMethod.telex.rawValue
    @AppStorage("launchAtLogin") var launchAtLogin: Bool = false
}
```

**State Synchronization:**
```
User Action (Menu/Hotkey)
    ↓
AppState.toggleMode() / AppState.inputMethod = method
    ↓
@AppStorage didSet triggers
    ↓
├─ KeyboardManager.shared.setVietnameseMode() → EventTap filtering
    ↓
└─ NotificationCenter.post(.modeChanged) → UI updates
```

**Persistence Layer:**
- `@AppStorage` wrapper for automatic UserDefaults synchronization
- Bidirectional binding: AppState ↔ KeyboardManager
- Launch-time state restoration

### 4. User Interface Architecture

**Menu Bar Integration:**
- **NSStatusItem:** System menu bar presence with V/E indicator
- **Dynamic Menu:** Rebuilds on state changes via NotificationCenter
- **SwiftUI Integration:** ConfigView wrapped in NSHostingView

**Settings Panel:**
- **SwiftUI Implementation:** Declarative UI with @ObservedObject binding
- **Launch at Login:** SMAppService integration (macOS 13+) with fallback
- **Fixed Dimensions:** 300×220 points for consistent appearance

**Global Hotkey:**
- **NSEvent Monitor:** System-wide event monitoring
- **Cmd+Shift Detection:** Modifier flag tracking
- **State Management:** Prevents double-toggle on key repeat

### 5. Permission & Security System

**Accessibility Permission Flow:**
```
App Launch
    ↓
Check Permission (AXIsProcessTrustedWithOptions)
    ↓
├─ Granted → Setup EventTap
    ↓
└─ Denied → Request Permission → Retry (max 5 attempts)
    ↓
Still Denied → Show Alert (Open Prefs/Retry/Quit)
```

**Security Considerations:**
- No network access (privacy-focused)
- Local-only processing (no data transmission)
- Required Accessibility permission clearly documented
- Graceful degradation on permission denial

## Event Flow Architecture

### 1. Normal Character Processing
```
Physical Key Press
    ↓
CGEvent (keyDown) captured by EventTap
    ↓
KeyboardManager.handleKeyEvent()
    ↓
├─ Skip if Vietnamese mode disabled
    ↓
├─ Skip if modifier keys pressed
    ↓
├─ Word boundary check → reset buffer if needed
    ↓
├─ Extract Unicode character
    ↓
├─ Pass to current InputEngine
    ↓
├─ Engine returns ProcessResult
    ↓
├─ If transformation needed:
    │   ├─ Send backspace events (kVK_Delete)
    │   ├─ Send replacement string via CGEvent
    │   └─ Return nil (suppress original event)
    ↓
└─ If no transformation:
    └─ Return original event (passthrough)
```

### 2. Mode Toggle Flow
```
User Action (Menu/Hotkey)
    ↓
AppState.toggleMode()
    ↓
@AppStorage didSet triggers
    ↓
├─ KeyboardManager.setVietnameseMode() → EventTap filtering
    ↓
├─ NotificationCenter.post(.modeChanged)
    ↓
├─ MenuBarManager updates icon (V→E or E→V)
    ↓
└─ MenuBarManager rebuilds menu (checkmark states)
```

### 3. Input Method Switching
```
User selects Telex/VNI from menu
    ↓
AppState.inputMethod = method
    ↓
@AppStorage didSet triggers
    ↓
KeyboardManager switches engine (TelexEngine ↔ VNIEngine)
    ↓
Engine.reset() clears buffer
    ↓
MenuBarManager rebuilds menu (radio button selection)
```

## Data Architecture

### 1. Vietnamese Character Mappings

**Core Data Structures (VietnameseData.swift):**
```swift
// Base vowels and transformations
let baseVowels: Set<Character> = ["a", "e", "i", "o", "u", "y"]
let modifiedVowels: Set<Character> = ["ă", "â", "ê", "ô", "ơ", "ư"]

// Tone mark mappings
let toneMarks: [Character: [Character]] = [
    "a": ["á", "à", "ả", "ã", "ạ"],
    "ă": ["ắ", "ằ", "ẳ", "ẵ", "ặ"],
    // ... all vowels
]

// Telex transformation rules
let telexTransforms: [String: Character] = [
    "aa": "â", "aw": "ă", "ee": "ê",
    "oo": "ô", "ow": "ơ", "uw": "ư"
]

// VNI transformation rules
let vniTransforms: [String: Character] = [
    "a6": "â", "a7": "ă", "o6": "ô",
    "o7": "ơ", "u7": "ư", "e6": "ê"
]
```

### 2. State Persistence

**UserDefaults Keys:**
- `isVietnameseMode: Bool` - Vietnamese/English toggle state
- `inputMethod: String` - Selected input method ("telex" or "vni")
- `launchAtLogin: Bool` - Launch at login preference

**Persistence Strategy:**
- `@AppStorage` property wrapper for automatic synchronization
- Default values ensure app works on first launch
- State restoration on app startup via AppState.init()

## Performance Architecture

### 1. Event Processing Optimization

**Target Latency:** < 10ms per character

**Optimization Strategies:**
```swift
func handleKeyEvent(...) -> Unmanaged<CGEvent>? {
    // Fast path: early returns for non-processing
    guard isVietnameseMode else { return Unmanaged.passRetained(event) }
    guard !isModifierKey(event) else { return Unmanaged.passRetained(event) }

    // Minimal character extraction
    guard let char = getCharacter(from: event) else {
        return Unmanaged.passRetained(event)
    }

    // Efficient buffer management
    if isWordBoundary(char) {
        engine.reset()
        return Unmanaged.passRetained(event)
    }

    // Process through engine
    let result = engine.processKey(char)
    // ... result handling
}
```

### 2. Memory Management

**Buffer Constraints:**
- Maximum 7 characters per buffer (longest Vietnamese syllable)
- Static data structures for character mappings
- Singleton pattern prevents multiple instances

**Resource Cleanup:**
- EventTap lifecycle management (setup/cleanup)
- Notification observer removal
- Global event monitor cleanup

## Security Architecture

### 1. Permission Model

**Required Permissions:**
- **Accessibility:** For EventTap keyboard event interception
- **No Network Access:** Privacy-focused design
- **No File System Access:** Beyond UserDefaults

**Permission Flow:**
```
App Launch → Check Accessibility → Request if Needed → Retry Logic → User Alert → Setup EventTap
```

### 2. Input Validation

**Character Filtering:**
```swift
func processKey(_ char: Character) -> ProcessResult {
    // Validate input
    guard char.isLetter || char.isNumber || isToneMark(char) else {
        return .passthrough
    }

    // Process validated character
    // ... transformation logic
}
```

**Event Validation:**
- Skip processing for system events (Cmd, Ctrl, Alt combinations)
- Word boundary detection prevents buffer overflow
- Unicode validation for character extraction

## Deployment Architecture

### 1. Build System

**Build Scripts:**
- `build-release.sh` - Clean build, archive, export adhoc signed .app
- `create-dmg.sh` - Package .app into DMG with custom UI

**Signing Strategy:**
- Ad-hoc signing for direct distribution
- No notarization required (user accepts Gatekeeper warning)
- Optional Developer ID signing for enterprise deployment

### 2. Distribution Model

**Target:** Direct download distribution
**Package:** DMG installer with drag-to-Applications UI
**Requirements:**
- macOS 11.0+ (Big Sur or later)
- Accessibility permission grant
- User acceptance of non-notarized app

## Scalability Considerations

### 1. Engine Extensibility

**Protocol-Based Architecture:**
- Easy addition of new input methods (VIQR, custom rules)
- Shared tone placement logic across engines
- Consistent ProcessResult interface

### 2. UI Modularity

**SwiftUI Components:**
- ConfigView can be extended with additional settings
- MenuBarManager supports dynamic menu items
- NotificationCenter enables loose coupling

### 3. State Management

**Observable Pattern:**
- AppState can be extended with new properties
- @AppStorage automatically handles persistence
- NotificationCenter supports multiple observers

## Future Architecture Considerations

### 1. Multi-Language Support

**Potential Architecture:**
```swift
protocol LanguageInputEngine {
    func processKey(_ char: Character) -> ProcessResult
    var languageCode: String { get }
}

class KhmerEngine: LanguageInputEngine { }
class ThaiEngine: LanguageInputEngine { }
```

### 2. Plugin Architecture

**Extensibility Model:**
- Dynamic input method loading
- Custom transformation rules
- User-defined keyboard shortcuts

### 3. Cloud Integration

**Sync Architecture:**
- Settings synchronization across devices
- Custom dictionary sharing
- Usage analytics (opt-in)

---

**Document Version:** 1.0.0
**Last Updated:** 2025-11-26
**Maintained By:** Technical Documentation Agent
**Review Schedule:** With each architectural change or major release

## Appendix A: Component Interaction Diagram

```
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│   AppDelegate   │    │   AppState       │    │  MenuBarManager │
│                 │◄───┤                  │◄───┤                 │
│ - EventTap      │    │ - Persistence    │    │ - Status Item   │
│ - Hotkey        │    │ - State Mgmt     │    │ - Menu UI       │
│ - Permission    │    │ - Notifications  │    │ - Preferences   │
└────────┬────────┘    └────────┬─────────┘    └────────┬────────┘
         │                      │                        │
         ▼                      ▼                        ▼
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│KeyboardManager  │    │  @AppStorage     │    │  ConfigView     │
│                 │◄───┤                  │◄───┤   (SwiftUI)     │
│ - EventTap      │    │ - isVietnamese   │    │ - Settings UI   │
│ - Engine Mgmt   │    │ - inputMethod    │    │ - Launch@Login  │
│ - Event Process │    │ - launchAtLogin  │    │ - Method Picker │
└────────┬────────┘    └────────┬─────────┘    └────────┬────────┘
         │                      │                        │
         ▼                      ▼                        ▼
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│  InputEngine    │    │  UserDefaults    │    │  SMAppService   │
│   Protocol      │◄───┤                  │◄───┤                 │
│                 │    │ - Preferences    │    │ - Launch@Login  │
│ - TelexEngine   │    │ - State Data     │    │ - Registration  │
│ - VNIEngine     │    │ - Settings       │    │ - Management    │
└─────────────────┘    └──────────────────┘    └─────────────────┘
```

## Appendix B: Event Processing State Machine

```
┌─────────────┐     ┌─────────────┐     ┌─────────────┐
│   Idle      │────▶│ Vietnamese  │────▶│   Process   │
│  (English)  │     │   Mode      │     │  Character  │
└─────────────┘     └─────────────┘     └─────────────┘
      ▲                    │                    │
      │                    │                    ▼
      │                    │              ┌─────────────┐
      │                    └───────────── │   Apply     │
      │                                   │Transform    │
      │                                   └─────────────┘
      │                                          │
      │                                          ▼
      │                                   ┌─────────────┐
      │                                   │Send Result  │
      │                                   │(Backspace + │
      │                                   │Replacement) │
      │                                   └─────────────┘
      │                                          │
      └──────────────────────────────────────────┘
```

## Appendix C: Vietnamese Tone Placement Logic

```
Input Buffer: ["v", "i", "e", "t", "6", "s"]
                      ▲
                      │
    ┌─────────────────┼─────────────────┐
    │                 │                 │
┌───▼───┐        ┌───▼───┐        ┌───▼───┐
│Check 1│        │Check 2│        │Check 3│
│Modified│       │Last   │       │Penultimate│
│Vowel? │       │Vowel? │       │Vowel? │
└───┬───┘        └───┬───┘        └───┬───┘
    │No              │No              │Yes
    ▼               ▼               ▼
┌───┴───┐        ┌───┴───┐        ┌───┴───┐
│Next   │        │Next   │        │Apply  │
│Check  │       │Check  │       │Tone   │
└───────┘        └───────┘        └───┬───┘
                                      │
                                      ▼
                               ┌─────────────┐
                               │Result:      │
                               │Position 4   │
                               │(penultimate)│
                               └─────────────┘
```

**Sources:**
- Apple Developer Documentation: CGEvent, EventTap
- Vietnamese Unicode Standard: Unicode 14.0 Vietnamese Character Set
- Vietnamese Orthography Rules: Quy tắc đặt dấu thanh trong tiếng Việt