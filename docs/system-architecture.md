# EndKey System Architecture

## Overview

EndKey's architecture is designed around a layered, modular approach that separates core Vietnamese text processing logic from platform-specific UI and system integration. The system combines a performant C++ engine for linguistic processing with a native macOS application layer for user interaction and system integration.

## Architectural Principles

1. **Separation of Concerns**: Clear boundaries between engine and UI
2. **Platform Abstraction**: Engine is platform-agnostic
3. **Performance First**: Optimized for real-time text processing
4. **Minimal Dependencies**: Self-contained with no external libraries
5. **Event-Driven**: Reactive system based on input events
6. **State Management**: Centralized state with clear transitions

## System Layers

```
┌─────────────────────────────────────────┐
│         User Interface Layer            │
│    (Objective-C / Cocoa Framework)      │
│                                         │
│  ┌─────────────┐  ┌─────────────────┐  │
│  │Preferences  │  │  Status Menu    │  │
│  │   Window    │  │                 │  │
│  └─────────────┘  └─────────────────┘  │
│           │               │             │
│  ┌─────────────┐  ┌─────────────────┐  │
│  │  Macro      │  │  Convert Tool   │  │
│  │  Manager    │  │                 │  │
│  └─────────────┘  └─────────────────┘  │
│           │               │             │
│           └───────┬───────┘             │
│                   │                     │
│  ┌───────────────────────────────────┐ │
│  │   macOS Integration Layer         │ │
│  │   (EndKeyManager, AppDelegate)    │ │
│  └───────────────────────────────────┘ │
└───────────────┬─────────────────────────┘
                │
                ▼
┌─────────────────────────────────────────┐
│      Event Interception Layer           │
│        (CGEventTap, Core Graphics)      │
│                                         │
│  ┌───────────────────────────────────┐ │
│  │   Input Event Tap                 │ │
│  │   - Keyboard Events               │ │
│  │   - Mouse Events                  │ │
│  │   - Modifier Key Changes          │ │
│  └───────────────────────────────────┘ │
└───────────────┬─────────────────────────┘
                │
                ▼
┌─────────────────────────────────────────┐
│         Engine Bridge Layer             │
│     (Objective-C++ / C Export)          │
│                                         │
│  ┌───────────────────────────────────┐ │
│  │   C Function Interface            │ │
│  │   - vKeyInit()                    │ │
│  │   - vKeyHandleEvent()             │ │
│  │   - startNewSession()             │ │
│  └───────────────────────────────────┘ │
└───────────────┬─────────────────────────┘
                │
                ▼
┌─────────────────────────────────────────┐
│      Core Processing Engine             │
│        (C++ / Platform Agnostic)        │
│                                         │
│  ┌─────────────┐  ┌─────────────────┐  │
│  │ Vietnamese  │  │     Macro       │  │
│  │  Module     │  │    Module       │  │
│  └─────────────┘  └─────────────────┘  │
│           │               │             │
│  ┌─────────────┐  ┌─────────────────┐  │
│  │   Engine    │  │ SmartSwitchKey  │  │
│  │ Controller  │  │     Module      │  │
│  └─────────────┘  └─────────────────┘  │
│           │               │             │
│           └───────┬───────┘             │
│                   │                     │
│           ┌───────▼───────┐             │
│           │  Data Types   │             │
│           │    & Enums    │             │
│           └───────────────┘             │
└─────────────────────────────────────────┘
```

## Component Details

### 1. User Interface Layer

#### AppDelegate
**Responsibilities:**
- Application lifecycle management
- Global menu handling (Preferences, About, Quit)
- Status menu management (enable/disable, quick settings)
- Auto-startup with system configuration
- Configuration persistence (NSUserDefaults)
- Global hotkey registration and handling
- Version update checking via GitHub API

**Key Methods:**
```objc
- (void)onImputMethodChanged:(BOOL)willNotify;
- (void)onInputMethodSelected;
- (void)askPermission;
- (void)setRunOnStartup:(BOOL)val;
- (void)loadDefaultConfig;
```

**State Managed:**
- Current application preferences
- Global settings
- System integration status
- Update check status

#### ViewController
**Responsibilities:**
- Preferences window management
- Tabbed interface for different settings
- Input method selection UI
- Feature toggle controls
- Event tap initialization and status
- Permission status display

**Tab Sections:**
- **Primary Tab**: Input method, encoding, basic features
- **Macro Tab**: Macro management and configuration
- **System Tab**: Advanced features and system integration

**Data Binding:**
- Reads/writes to global configuration variables
- Updates engine state through AppDelegate
- Validates user input and provides feedback

#### EndKeyManager
**Responsibilities:**
- CGEventTap lifecycle management
- Event processing thread coordination
- Error handling for permission failures
- Event callback registration
- Background thread run loop management

**Key Methods:**
```objc
+ (BOOL)initEventTap;
+ (BOOL)stopEventTap;
+ (NSArray*)getTableCodes;
+ (NSString*)getBuildDate;
```

**Threading Model:**
- Main thread: UI operations
- Background thread: Event tap processing via CFRunLoop
- GCD: Asynchronous operations (version check, initialization)

#### Specialized View Controllers

**ConvertToolViewController**
- Batch Vietnamese text conversion
- Clipboard integration (paste, copy)
- HTML and plain text support
- Encoding detection and validation

**MacroViewController**
- Macro table display and editing
- Add/edit/delete macro operations
- Search and filter macros
- Import/export functionality

**AboutViewController**
- Version and build information display
- Copyright and credits
- Link to documentation

### 2. macOS Integration Layer

#### Event Tap Architecture

The CGEventTap is the cornerstone of EndKey's ability to intercept and modify input:

```objc
eventTap = CGEventTapCreate(kCGSessionEventTap,
                            kCGHeadInsertEventTap,
                            0,
                            eventMask,
                            EndKeyCallback,
                            NULL);
```

**Event Types Captured:**
- `kCGEventKeyDown`: Keyboard press events
- `kCGEventKeyUp`: Keyboard release events
- `kCGEventFlagsChanged`: Modifier key changes (Ctrl, Shift, etc.)
- Mouse events: For cursor position tracking

**Event Mask Configuration:**
```objc
eventMask = ((1 << kCGEventKeyDown) |
             (1 << kCGEventKeyUp) |
             (1 << kCGEventFlagsChanged) |
             (1 << kCGEventLeftMouseDown) |
             (1 << kCGEventRightMouseDown) |
             (1 << kCGEventLeftMouseDragged) |
             (1 << kCGEventRightMouseDragged));
```

**Processing Flow:**
1. Event generated by system
2. CGEventTap intercepts event
3. Callback function invoked on background thread
4. C++ engine processes event
5. Modified event returned or event replaced
6. Event continues to target application

#### Event Callback Function

```cpp
// C function bridge between CGEventTap and C++ engine
CGEventRef EndKeyCallback(CGEventTapProxy proxy,
                          CGEventType type,
                          CGEventRef event,
                          void *refcon) {
    // Extract event data
    CGKeyCode keyCode = CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
    CGEventFlags flags = CGEventGetFlags(event);

    // Convert to engine representation
    vKeyEvent eventType = Keyboard;
    vKeyEventState eventState = (type == kCGEventKeyDown) ? KeyDown : KeyUp;
    Uint16 keyData = keyCode;

    // Process through engine
    vKeyHandleEvent(eventType, eventState, keyData, capsStatus, otherControlKey);

    return event;
}
```

#### Accessibility Permission Management

The system requires macOS Accessibility permission:

```objc
- (void)askPermission {
    // Check current status
    // Display permission dialog
    // Guide user through System Preferences
    // Monitor permission changes
}
```

**Permission Handling:**
1. Check current permission status
2. Display informative dialog
3. Open System Preferences if needed
4. Poll for permission changes
5. Initialize event tap when granted

### 3. Engine Bridge Layer

#### C Interface Specification

The C++ engine exposes a C-compatible interface:

```cpp
// Core initialization
void* vKeyInit();

// Main event processing
void vKeyHandleEvent(const vKeyEvent& event,
                     const vKeyEventState& state,
                     const Uint16& data,
                     const Uint8& capsStatus=0,
                     const bool& otherControlKey=false);

// Session management
void startNewSession();
void vTempOffSpellChecking();
void vSetCheckSpelling();

// Utility functions
wstring utf8ToWideString(const string& str);
string wideStringToUtf8(const wstring& str);
```

**Design Rationale:**
- C interface ensures Objective-C compatibility
- Minimizes overhead of C++ name mangling
- Clear ABI boundaries
- Easier debugging and interop

#### Objective-C++ Bridge Implementation

```objc
// Bridge initialization (EndKey.mm)
@implementation EndKeyApp

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Initialize C++ engine
    EndKeyInit();

    // Set up configuration
    loadDefaultConfig();

    // Start event tap
    [EndKeyManager initEventTap];
}

@end
```

**String Conversion:**
```cpp
// Helper functions in .mm files
extern "C" NSString* ConvertUtil(NSString* str) {
    // Convert NSString (UTF-16) to std::string (UTF-8)
    // Process through engine
    // Convert back to NSString
}
```

### 4. Core Processing Engine

#### Engine Controller

The main Engine.cpp coordinates all processing:

```cpp
void vKeyHandleEvent(const vKeyEvent& event,
                     const vKeyEventState& state,
                     const Uint16& data,
                     const Uint8& capsStatus,
                     const bool& otherControlKey) {

    // 1. Update global state
    updateKeyState(data, state, capsStatus, otherControlKey);

    // 2. Check if should process
    if (!shouldProcessEvent(event, state, data)) {
        return;
    }

    // 3. Route to appropriate handler
    if (vLanguage == 0) {
        // English mode - check for macros
        vEnglishMode(state, data, capsStatus, otherControlKey);
    } else {
        // Vietnamese mode - full processing
        processVietnameseInput(event, state, data, capsStatus, otherControlKey);
    }

    // 4. Apply results
    applyProcessedEvent(event, data);
}
```

**State Machine:**
- **Idle**: No active typing session
- **Typing**: Building up a word character by character
- **Word Break**: Detected end of word (space, punctuation)
- **Restore**: Reverting to original input
- **Replace**: Using macro or string replacement

#### Vietnamese Module

**Character Mapping:**
- Comprehensive vowel tables with tone mark combinations
- Consonant recognition (initial and final)
- Diphthong and triphthong processing
- Tone mark placement rules

**Data Structures:**
```cpp
// Vowel combinations
extern map<Uint16, vector<vector<Uint16>>> _vowel;

// Combined characters (with tones)
extern map<Uint16, vector<vector<Uint32>>> _vowelCombine;

// Consonant tables
extern vector<vector<Uint16>> _consonantTable;
extern vector<vector<Uint16>> _endConsonantTable;

// Quick access mappings
extern map<Uint32, vector<Uint16>> _quickTelex;
extern map<Uint16, vector<Uint16>> _quickStartConsonant;
extern map<Uint16, vector<Uint16>> _quickEndConsonant;
```

**Processing Algorithm:**
1. **Character Recognition**: Identify if input is vowel, consonant, or mark
2. **Combination Check**: Attempt to form valid Vietnamese characters
3. **Tone Mark Handling**: Apply or adjust tone marks
4. **Validation**: Check if resulting combination is valid Vietnamese
5. **Fallback**: Restore original if invalid

**Character Encoding:**
- Internal representation: Custom bit-packed format
- Component masks:
  - Character base (low 16 bits): CHAR_MASK (0xFFFF)
  - Tone marks: MARK_MASK (0xF80000)
  - Caps indicator: CAPS_MASK (0x10000)
  - Special flags: Various bits for context

#### Macro Module

**Data Structure:**
```cpp
struct MacroData {
    string macroText;           // Shortcut (e.g., "btw")
    string macroContent;        // Expansion (e.g., "by the way")
    vector<Uint32> macroContentCode;  // Pre-computed character codes
};
```

**Macro Processing:**
1. **Accumulation**: Build buffer of typed characters
2. **Lookup**: Search for exact match in macro dictionary
3. **Case Matching**: Apply case-sensitive rules
4. **Expansion**: Replace with full text
5. **Case Preservation**: Adjust case of expansion

**Case Rules:**
- Lowercase: `btw` → `by the way`
- Uppercase: `BTW` → `BY THE WAY`
- Capitalized: `Btw` → `By the way`

**Dictionary Management:**
- Stored in memory as std::map
- Serialized to binary format for persistence
- Loaded on initialization
- Save on modification

#### SmartSwitchKey Module

**Per-Application Preferences:**
```cpp
// Application configuration
struct AppConfig {
    string bundleId;    // Application identifier
    int preferredLanguage;  // 0=English, 1=Vietnamese
    int timestamp;      // Last update time
};
```

**Detection Logic:**
1. Get active application bundle ID
2. Look up in per-application table
3. If found: use configured language
4. If not found: use global default
5. Update table with detected choice

**Persistence:**
- Binary format for efficiency
- Stored in Application Support directory
- Versioned for future compatibility
- Automatic backup before updates

#### Data Type System

**Core Types:**
```cpp
typedef unsigned char Byte;
typedef unsigned char Uint8;
typedef unsigned short Uint16;
typedef unsigned int Uint32;
typedef unsigned long int Uint64;
```

**Event Types:**
```cpp
enum vKeyEvent {
    Keyboard,
    Mouse
};

enum vKeyEventState {
    KeyDown,
    KeyUp,
    MouseDown,
    MouseUp
};

enum vKeyInputType {
    vTelex = 0,
    vVNI,
    vSimpleTelex1,
    vSimpleTelex2
};
```

**Processing State:**
```cpp
struct vKeyHookState {
    Byte code;              // Processing code
    Byte backspaceCount;    // Characters to delete
    Byte newCharCount;      // Characters to insert
    Byte extCode;           // Extended code
    Uint32 charData[MAX_BUFF];  // New characters
    vector<Uint32> macroKey;    // Macro input
    vector<Uint32> macroData;   // Macro output
};
```

## Data Flow Architecture

### Input Event Flow

```
┌──────────┐
│ User     │
│ Types    │
└────┬─────┘
     │
     ▼
┌──────────────────┐
│ CGEventTap       │
│ (System Level)   │
└────┬─────────────┘
     │
     ▼
┌──────────────────┐
│ Event Callback   │
│ (Background      │
│  Thread)         │
└────┬─────────────┘
     │
     ▼
┌──────────────────┐
│ vKeyHandleEvent  │
│ (Engine Entry)   │
└────┬─────────────┘
     │
     ▼
┌──────────────────┐
│ ┌────────────┐  │
│ │ Check Lang │  │
│ └────────────┘  │
└────┬─────────────┘
     │
     ├────────────┐
     │            │
     ▼            ▼
┌──────────┐  ┌──────────────────┐
│ English  │  │ Vietnamese       │
│ Mode     │  │ Processing       │
└────┬─────┘  └────┬─────────────┘
     │             │
     ▼             ▼
┌──────────┐  ┌──────────────────┐
│ Check    │  │ Vietnamese       │
│ Macro    │  │ Module           │
└────┬─────┘  └────┬─────────────┘
     │             │
     ▼             ▼
┌──────────┐  ┌──────────────────┐
│ Expand   │  │ Smart Switch     │
│ Macro    │  │ Check            │
└────┬─────┘  └────┬─────────────┘
     │             │
     ▼             ▼
┌──────────────────┐
│ Send Modified    │
│ Event Back       │
└────┬─────────────┘
     │
     ▼
┌──────────┐
│ App      │
│ Receives │
│ Text     │
└──────────┘
```

### Vietnamese Processing Pipeline

```
Input: w, a, s
 │
 ▼
┌──────────────────┐
│ Character        │
│ Recognition      │
└────┬─────────────┘
     │
     ▼
┌──────────────────┐
│ Vowel Lookup     │
│ (w is consonant) │
└────┬─────────────┘
     │
     ▼
┌──────────────────┐
│ Consonant Check  │
│ (w + a: valid?)  │
└────┬─────────────┘
     │
     ▼
┌──────────────────┐
│ Build Character  │
│ wa               │
└────┬─────────────┘
     │
     ▼
Input: s (tone mark)
 │
 ▼
┌──────────────────┐
│ Tone Mark        │
│ Detection        │
└────┬─────────────┘
     │
     ▼
┌──────────────────┐
│ Combine          │
│ wa + s = wás     │
└────┬─────────────┘
     │
     ▼
┌──────────────────┐
│ Validation       │
│ (Valid Vietnamese│
│  word?)          │
└────┬─────────────┘
     │
     ▼
Output: wás
```

### Configuration Flow

```
User Changes
Setting (UI)
    │
    ▼
┌──────────────────┐
│ ViewController   │
│ Updates Variable │
└────┬─────────────┘
     │
     ▼
┌──────────────────┐
│ AppDelegate      │
│ Persists to      │
│ NSUserDefaults   │
└────┬─────────────┘
     │
     ▼
┌──────────────────┐
│ Load on Next     │
│ Startup          │
└────┬─────────────┘
     │
     ▼
┌──────────────────┐
│ Engine           │
│ Configuration    │
└──────────────────┘
```

## State Management

### Global Configuration Variables

All engine configuration uses global variables for simplicity:

```cpp
// Language and input method
extern int vLanguage;          // 0=English, 1=Vietnamese
extern int vInputType;         // 0=Telex, 1=VNI, 2=Simple Telex 1, 3=Simple Telex 2

// Character encoding (legacy, now Unicode-only)
extern int vCodeTable;

// Feature flags
extern int vFreeMark;              // Allow typing marks without base
extern int vCheckSpelling;         // Enable spell checking
extern int vUseModernOrthography;  // Modern Vietnamese rules
extern int vRestoreIfWrongSpelling;
extern int vUseMacro;              // Enable macro expansion
extern int vUseMacroInEnglishMode; // Apply macros in English mode
extern int vUseSmartSwitchKey;     // Per-app language switching
extern int vUpperCaseFirstChar;    // Auto-capitalize sentence start
extern int vDoubleSpacePeriod;     // Double space = ". "
extern int vAutoCapsMacro;         // Preserve case in macros

// Orthography features
extern int vQuickTelex;            // Quick consonant combos (ch, ph, etc.)
extern int vQuickStartConsonant;   // f->ph, j->gi, w->qu
extern int vQuickEndConsonant;     // g->ng, h->nh, k->ch

// Advanced features
extern int vRememberCode;          // Remember encoding per app
extern int vOtherLanguage;         // Switch off Vietnamese for other langs
extern int vTempOffEndKey;         // Hotkey to disable
extern int vTempOffSpelling;       // Ctrl to temporarily disable spell check
extern int vAllowConsonantZFWJ;    // Allow Z, F, W, J as consonants

// Browser/application compatibility
extern int vFixRecommendBrowser;   // Fix for browser address bars
extern int vFixChromiumBrowser;    // Chromium-specific fixes
extern int vPerformLayoutCompat;   // Layout compatibility mode
extern int vSendKeyStepByStep;     // Send characters individually
```

**State Persistence:**
- All variables saved to NSUserDefaults
- Load on application startup
- Immediate persistence on change
- Backward compatibility handling

### Internal Engine State

**Typing Session:**
```cpp
// Internal state (file-static or class members)
Uint16 TypingWord[MAX_BUFF];       // Current word buffer
int _index;                        // Current position in buffer
int _lastIndex;                    // Previous position
bool _isTyping;                    // Session active flag
bool _hasMark;                     // Has tone mark flag
bool _shouldRestore;               // Should restore flag
bool _skipMacroNextBreak;          // Skip macro next word break
```

**Session Lifecycle:**
1. **Start**: User types first character → `_isTyping = true`, `_index = 0`
2. **Build**: Each character added → `_index++`, buffer updated
3. **Validate**: On each key → check valid Vietnamese
4. **Break**: Space/punctuation → finalize word, reset session
5. **Restore**: Invalid sequence → revert to original input

## Threading Model

### Multi-Threading Architecture

```
┌─────────────────────────────────────┐
│             Main Thread             │
│                                     │
│  ┌─────────────┐  ┌─────────────┐  │
│  │ UI Updates  │  │ Menu        │  │
│  └─────────────┘  └─────────────┘  │
│                                     │
│  ┌─────────────┐  ┌─────────────┐  │
│  │ Preferences │  │ Status Menu │  │
│  └─────────────┘  └─────────────┘  │
└─────────────────────────────────────┘
                │
                │ (IPC via CFRunLoop)
                ▼
┌─────────────────────────────────────┐
│        Background Thread            │
│                                     │
│  ┌─────────────────────────────────┐ │
│  │     CFRunLoop with Event Tap    │ │
│  │                                 │ │
│  │  ┌──────────┐  ┌────────────┐  │ │
│  │  │ CGEvent  │  │ Engine     │  │ │
│  │  │ Tap      │  │ Processing │  │ │
│  │  └──────────┘  └────────────┘  │ │
│  └─────────────────────────────────┘ │
└─────────────────────────────────────┘
```

**Thread Responsibilities:**

**Main Thread:**
- All UI operations
- NSApplication lifecycle
- Menu handling
- Status menu updates
- Preference window
- User interaction

**Background Thread:**
- CGEventTap event processing
- C++ engine event handling
- Real-time text processing
- Critical performance path

**Dispatch Queue:**
- GitHub version check (asynchronous)
- File I/O operations
- Initialization tasks
- Non-urgent background work

### Thread Synchronization

**Event Tap Threading:**
```objc
// Event tap runs on background thread with its own run loop
dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
    CFRunLoopRef runLoop = CFRunLoopGetCurrent();
    CFRunLoopAddSource(runLoop, runLoopSource, kCFRunLoopCommonModes);
    CGEventTapEnable(eventTap, true);
    CFRunLoopRun();  // Blocks forever
});
```

**Main Thread Communication:**
```objc
// Update UI from background thread
dispatch_async(dispatch_get_main_queue(), ^{
    [self updateUIWithNewStatus:status];
});
```

**Why This Design:**
- UI must be on main thread (Cocoa requirement)
- Event processing must be fast (background thread)
- No shared state between threads (minimal synchronization)
- Engine is thread-local (no global mutable state during processing)

## Performance Architecture

### Optimization Strategies

**1. Minimal Allocation**
- Fixed-size buffers (MAX_BUFF = 32)
- Pre-allocated vectors for common cases
- Stack allocation preferred over heap
- Object pooling where beneficial

**2. Efficient Data Structures**
- `std::map` for lookups (logarithmic time)
- `std::vector` for sequential access
- Custom arrays for character tables (O(1) access)
- Bit packing for character encoding

**3. Caching**
- Pre-computed character mappings
- Case-converted macro content
- Application preferences cache
- Frequently accessed data in registers

**4. Lazy Evaluation**
- Load macro data on demand
- Initialize engine modules only when needed
- Defer UI updates until necessary
- Background thread handles heavy operations

### Memory Management

**Engine Memory:**
- Static allocation for character tables (~50KB)
- Stack allocation for temporary buffers
- Minimal heap allocation
- No memory leaks (C++ RAII)

**UI Memory:**
- ARC manages Objective-C objects
- Strong/weak reference patterns
- No retain cycles
- Proper cleanup on dealloc

**Shared Memory:**
- Global configuration variables
- Per-session state (cleared on word break)
- Application preferences (persisted to disk)
- Macro dictionary (loaded on startup)

## Error Handling

### Error Types and Handling

**1. Permission Errors**
```
EventTap creation fails → Display permission dialog
→ Guide to System Preferences → Retry initialization
```

**2. Invalid Input**
```
Invalid character sequence → Restore to original
→ Reset session → Continue processing
```

**3. Configuration Errors**
```
Corrupted preferences → Load defaults → Warn user
→ Restore to known-good state
```

**4. System Errors**
```
Memory allocation failure → Fallback to limited mode
→ Log error → Continue operation
```

### Resilience Strategies

**Graceful Degradation:**
- Core functionality works even if advanced features fail
- Continue processing if optional features unavailable
- Provide user feedback on errors
- Automatic recovery on next session

**Validation:**
- Validate all input before processing
- Check bounds before array access
- Verify file format before parsing
- Sanitize user input

**Recovery:**
- Reset to known state on error
- Clear buffers on session reset
- Reload configuration if corrupted
- Retry operations with backoff

## Security Architecture

### Security Considerations

**1. Permission Model**
- macOS Accessibility permission only
- No additional elevated privileges
- Sandboxed where possible
- Minimal attack surface

**2. Input Validation**
- All key codes validated (0-255 range)
- Buffer bounds checking
- No buffer overflow vulnerabilities
- Safe string handling

**3. Data Privacy**
- No telemetry or analytics
- No user data transmission
- Local storage only
- Optional version check (no data sent)

**4. Code Integrity**
- Code signing for distribution
- Static linking (no external dependencies)
- No dynamic code loading
- ASLR-compatible

### Data Protection

**Configuration Files:**
- Stored in user's Application Support directory
- Not accessible to other applications
- Backed up by Time Machine
- Can be deleted to reset to defaults

**Macro Data:**
- Stored locally only
- Not transmitted anywhere
- Binary format (not easily edited)
- Versioned for compatibility

**No External Dependencies:**
- All code included in app bundle
- No network communication (except optional version check)
- No third-party libraries
- Self-contained application

## Scalability Considerations

### Performance Scaling

**Current Limits:**
- Real-time processing: <10ms latency
- Buffer size: 32 characters (MAX_BUFF)
- Macro dictionary: Limited by memory (thousands of entries)
- Concurrent apps: N/A (single application)

**Future Enhancements:**
- Multiple input method support (future)
- Cloud sync (optional, not implemented)
- User statistics (not implemented)
- Plugin architecture (hypothetical)

### Design Flexibility

**Extensibility Points:**
- New input methods: Add enum value + mappings
- New features: Add global variable + UI control
- New modules: Add C interface + implementation
- New platforms: Rewrite UI layer

**Maintainability:**
- Clear module boundaries
- Well-documented interfaces
- Consistent coding standards
- Comprehensive configuration

## Platform-Specific Design

### macOS Integration Points

**1. Accessibility API**
- CGEventTap for event interception
- CFRunLoop for event processing
- Core Graphics framework
- Requires user permission

**2. Application Integration**
- NSApplication for lifecycle
- NSStatusItem for menu bar
- NSUserDefaults for preferences
- NSPasteboard for clipboard

**3. System Integration**
- Auto-start with system (LaunchAgents)
- Code signing for distribution
- Bundle structure for resources
- Crash reporting (not implemented)

### Platform Abstraction

**What is Platform-Specific:**
- Event tap (CGEventTap)
- UI layer (Cocoa)
- System integration (LaunchAgents)
- File I/O (NSFileManager)

**What is Platform-Agnostic:**
- All C++ engine code
- Vietnamese processing logic
- Macro system
- SmartSwitch logic
- Data structures and types

**Portability:**
- Engine could be ported to Windows/Linux
- Would require rewriting event tap layer
- Would require new UI framework
- Core logic remains unchanged

## Conclusion

EndKey's architecture balances performance, maintainability, and platform integration. The layered approach separates concerns cleanly, with the C++ engine handling all linguistic processing and the Objective-C layer managing macOS integration. The event-driven, reactive architecture ensures real-time performance while the modular design allows for maintainability and future enhancement.

Key architectural strengths:
- Clear separation of engine and UI
- Minimal dependencies and complexity
- Efficient event processing
- Platform abstraction in core
- Comprehensive state management

The architecture has proven stable and performant for production use, handling real-time Vietnamese text processing efficiently while providing a rich feature set through an intuitive macOS-native interface.
