# EndKey Code Standards & Development Guidelines

**Version:** 1.0.0
**Last Updated:** 2025-11-26
**Scope:** Swift development standards for EndKey project

## Architecture Principles

### 1. EventTap Architecture
```
User Input → EventTap Capture → Vietnamese Logic → Event Modification → System Output
```
- **Principle:** Use low-level EventTap for direct event interception
- **Rationale:** Simpler than IME registration, enables direct distribution
- **Implementation:** CGEvent.tapCreate with session-level interception

### 2. Protocol-Based Design
- **InputEngine Protocol:** Defines common interface for all input methods
- **Extensibility:** Easy to add new input methods (VIQR, custom rules)
- **Testability:** Protocol enables mock implementations for testing

### 3. Singleton Pattern for Core Services
- **KeyboardManager:** Centralized event processing
- **AppState:** Global state management with persistence
- **Rationale:** Single source of truth, avoids dependency injection complexity

### 4. State Management
- **@AppStorage:** Automatic UserDefaults persistence
- **NotificationCenter:** Decoupled UI updates
- **Bidirectional Sync:** AppState ↔ KeyboardManager

## Swift Coding Conventions

### 1. Naming Conventions

#### Classes & Structs
```swift
// Use PascalCase for types
class KeyboardManager { }
struct ProcessResult { }
enum InputMethod { }

// Descriptive names that indicate purpose
class VietnameseData { } // Good
class VNIData { }       // Avoid - unclear abbreviation
```

#### Functions & Methods
```swift
// Use camelCase, start with verb
func processKey(_ char: Character) -> ProcessResult
func setupEventTap() -> Bool
func toggleVietnameseMode()

// Boolean functions start with is/has/should
func isVowel(_ char: Character) -> Bool
func hasToneMark(_ char: Character) -> Bool
func shouldProcessEvent(_ event: CGEvent) -> Bool
```

#### Properties & Variables
```swift
// Use camelCase, descriptive names
var isVietnameseMode: Bool
var inputMethod: InputMethod
var eventTap: CFMachPort?

// Constants use camelCase or UPPER_SNAKE_CASE for globals
let maxBufferSize = 7
let TONE_MARKS = ["s", "f", "r", "x", "j"]
```

#### Enums
```swift
// Use PascalCase, singular names
enum InputMethod {
    case telex
    case vni
}

// Associated values when needed
enum ProcessResult {
    case passthrough
    case transform(backspaceCount: Int, replacement: String)
}
```

### 2. Code Organization

#### File Structure
```
EndKey/
├── Core/                    # Vietnamese input logic
│   ├── InputEngine.swift   # Protocols & types
│   ├── TelexEngine.swift   # Telex implementation
│   ├── VNIEngine.swift     # VNI implementation
│   ├── VietnameseData.swift # Character mappings
│   └── KeyboardManager.swift # EventTap management
├── UI/                     # User interface
│   ├── MenuBarManager.swift # Status bar integration
│   └── ConfigView.swift    # Settings panel
├── Models/                 # Data models
│   └── AppState.swift      # Application state
├── Utils/                  # Utilities
│   └── PermissionHelper.swift # System permissions
└── EndKeyApp.swift        # App entry point
```

#### Class Organization
```swift
class KeyboardManager {
    // MARK: - Properties
    static let shared = KeyboardManager()
    private var eventTap: CFMachPort?

    // MARK: - Public API
    func setupEventTap() -> Bool { }
    func toggleVietnameseMode() { }

    // MARK: - Private Methods
    private func handleKeyEvent(...) -> Unmanaged<CGEvent>? { }

    // MARK: - Key Simulation
    private func sendBackspaces(count: Int) { }
}
```

### 3. Access Control

```swift
// Use appropriate access levels
public class KeyboardManager { }      // Only if needed for framework
internal class TelexEngine { }        // Default - visible within module
private var buffer: [Character] = []  // Implementation details
fileprivate let maxRetries = 5        // Within file only

// Prefer private over internal
private func processComplexLogic() { } // Good - hidden implementation
func helperMethod() { }                // Avoid - defaults to internal
```

### 4. Optionals & Error Handling

```swift
// Use optional binding with meaningful names
if let eventTap = eventTap {
    CGEvent.tapEnable(tap: eventTap, enable: true)
}

// Use guard for early returns
guard let char = getCharacter(from: event) else {
    return Unmanaged.passRetained(event)
}

// Provide default values where appropriate
let method = InputMethod(rawValue: rawValue) ?? .telex

// Use Result type for complex operations
func validateInput(_ input: String) -> Result<ProcessedInput, InputError>
```

## Architecture Patterns

### 1. Singleton Pattern (Core Services)
```swift
class KeyboardManager {
    static let shared = KeyboardManager()
    private init() {} // Prevent multiple instances
}

// Usage
KeyboardManager.shared.setupEventTap()
```

### 2. Protocol-Oriented Design
```swift
protocol InputEngine {
    mutating func processKey(_ char: Character) -> ProcessResult
    mutating func reset()
    var buffer: [Character] { get }
}

// Multiple implementations
struct TelexEngine: InputEngine { }
struct VNIEngine: InputEngine { }
```

### 3. Observer Pattern (State Updates)
```swift
// AppState changes trigger UI updates
class AppState: ObservableObject {
    @AppStorage("isVietnameseMode") var isVietnameseMode: Bool = true {
        didSet {
            NotificationCenter.default.post(name: .modeChanged, object: nil)
        }
    }
}

// UI observes changes
NotificationCenter.default.addObserver(
    self,
    selector: #selector(modeChanged),
    name: .modeChanged,
    object: nil
)
```

### 4. Strategy Pattern (Input Methods)
```swift
class KeyboardManager {
    private var engine: InputEngine = TelexEngine()

    var inputMethod: InputMethod = .telex {
        didSet {
            switch inputMethod {
            case .telex: engine = TelexEngine()
            case .vni: engine = VNIEngine()
            }
        }
    }
}
```

## Vietnamese Input Logic Standards

### 1. Tone Placement Rules
```swift
// Priority order for tone placement
func findTonePosition(in buffer: [Character]) -> Int? {
    // Priority 1: Modified vowels (ă, â, ê, ô, ơ, ư)
    if let modifiedIndex = buffer.lastIndex(where: { isModifiedVowel($0) }) {
        return modifiedIndex
    }

    // Priority 2: Syllable ending with consonant
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

### 2. Buffer Management
```swift
struct InputEngine {
    private var buffer: [Character] = []
    private let maxBufferSize = 7 // Longest Vietnamese syllable

    mutating func processKey(_ char: Character) -> ProcessResult {
        // Handle word boundaries
        if isWordBoundary(char) {
            reset()
            return .passthrough
        }

        // Manage buffer size
        if buffer.count >= maxBufferSize {
            buffer.removeFirst()
        }

        buffer.append(char)
        // ... processing logic
    }
}
```

### 3. Case Preservation
```swift
func preserveCase(for result: String, original: Character) -> String {
    if original.isUppercase {
        return result.uppercased()
    }
    return result.lowercased()
}
```

## Threading & Concurrency

### 1. EventTap Threading
```swift
// EventTap callback runs on dedicated thread
private func handleKeyEvent(proxy: CGEventTapProxy,
                          type: CGEventType,
                          event: CGEvent) -> Unmanaged<CGEvent>? {
    // Process on callback thread
    let result = engine.processKey(char)

    // Send events synchronously to avoid race conditions
    if result.backspaceCount > 0 {
        sendBackspaces(count: result.backspaceCount)
        sendString(result.replacement!)
        return nil // Suppress original
    }

    return Unmanaged.passRetained(event)
}
```

### 2. UI Updates on Main Thread
```swift
// Always update UI on main thread
DispatchQueue.main.async {
    self.updateMenuBarIcon()
    self.rebuildMenu()
}
```

## Memory Management

### 1. Resource Cleanup
```swift
deinit {
    // Remove observers
    NotificationCenter.default.removeObserver(self)

    // Clean up EventTap
    if let monitor = hotkeyMonitor {
        NSEvent.removeMonitor(monitor)
    }

    KeyboardManager.shared.cleanup()
}
```

### 2. Strong Reference Cycles
```swift
// Use weak self in closures
DispatchQueue.main.asyncAfter(deadline: .now() + 2) { [weak self] in
    self?.checkAccessibilityPermission()
}

// Use weak delegate pattern
weak var delegate: InputEngineDelegate?
```

## Error Handling

### 1. Graceful Degradation
```swift
func setupEventTap() -> Bool {
    guard let eventTap = CGEvent.tapCreate(...) else {
        // Show user-friendly error
        showEventTapError()
        return false
    }

    // Continue setup
    return true
}
```

### 2. User-Friendly Error Messages
```swift
func showPermissionAlert() {
    let alert = NSAlert()
    alert.messageText = "Accessibility Permission Required"
    alert.informativeText = "EndKey needs Accessibility access to process keyboard input. Please enable it in System Preferences."
    alert.alertStyle = .warning
    alert.addButton(withTitle: "Open Preferences")
    alert.addButton(withTitle: "Retry")
    alert.addButton(withTitle: "Quit")
}
```

## Performance Guidelines

### 1. Event Processing Latency
```swift
// Target: < 10ms event processing
func handleKeyEvent(...) -> Unmanaged<CGEvent>? {
    let startTime = CFAbsoluteTimeGetCurrent()

    // Fast path: skip processing if not needed
    guard isVietnameseMode else {
        return Unmanaged.passRetained(event)
    }

    // Process character
    let result = engine.processKey(char)

    let processingTime = (CFAbsoluteTimeGetCurrent() - startTime) * 1000
    if processingTime > 10 {
        print("Warning: Slow processing (")\(processingTime)ms)")
    }

    return result
}
```

### 2. Memory Usage
```swift
// Keep buffer size reasonable
private let maxBufferSize = 7 // Longest Vietnamese syllable

// Avoid large data structures
// VietnameseData uses static constants, not computed properties
```

## Testing Standards

### 1. Unit Test Structure
```swift
class TelexEngineTests: XCTestCase {
    func testBasicTransformations() {
        var engine = TelexEngine()

        // Test aa → â
        let result1 = engine.processKey("a")
        XCTAssertEqual(result1, .passthrough)

        let result2 = engine.processKey("a")
        XCTAssertEqual(result2.backspaceCount, 2)
        XCTAssertEqual(result2.replacement, "â")
    }
}
```

### 2. Test Naming
```swift
// Use descriptive test names
testTelex_AA_TransformsTo_ACircumflex()
testVNI_D9_TransformsTo_D_Stroke()
testTonePlacement_PrioritizesModifiedVowels()
```

## Documentation Standards

### 1. Code Comments
```swift
/// Manages keyboard event interception and Vietnamese input processing
/// - Thread-safe: EventTap callback runs on dedicated thread
/// - Lifecycle: setup → process → cleanup
class KeyboardManager {
    /// Maximum buffer size for Vietnamese syllables (longest: "nghiêng")
    private let maxBufferSize = 7

    /// Processes a character through the current input engine
    /// - Parameter char: The character to process
    /// - Returns: Transformation result with backspace count and replacement
    func processKey(_ char: Character) -> ProcessResult {
        // Handle tone marks first (higher priority than vowel transforms)
        if isToneMark(char) {
            return applyTone(char)
        }
    }
}
```

### 2. Complex Logic Documentation
```swift
// Vietnamese tone placement rules:
// 1. Modified vowels (ă, â, ê, ô, ơ, ư) always get the tone
// 2. If syllable ends with consonant, tone goes on last vowel
// 3. Otherwise, tone goes on penultimate vowel
func findTonePosition(in buffer: [Character]) -> Int? {
    // Implementation follows priority order above
}
```

## Security Guidelines

### 1. Input Validation
```swift
func processKey(_ char: Character) -> ProcessResult {
    // Validate input
    guard char.isLetter || char.isNumber else {
        return .passthrough
    }

    // Process validated input
    // ...
}
```

### 2. Permission Handling
```swift
// Always check permissions before sensitive operations
func setupEventTap() -> Bool {
    guard PermissionHelper.isAccessibilityEnabled else {
        requestPermission()
        return false
    }

    // Continue with EventTap setup
}
```

## Deployment Guidelines

### 1. Build Configuration
```swift
// Deployment target in project settings
macOS 11.0+ // Big Sur or later

// Code signing
Ad-hoc signing // For direct distribution

// Bundle configuration
LSUIElement: true // Background agent (no dock icon)
NSAccessibilityUsageDescription: "EndKey needs to capture keyboard events"
```

### 2. Version Management
```swift
// Semantic versioning
1.0.0 // Major.Minor.Patch

// Version updates
// Major: Breaking changes to API or behavior
// Minor: New features, backward compatible
// Patch: Bug fixes only
```

---

**Document Version:** 1.0.0
**Last Updated:** 2025-11-26
**Maintained By:** Technical Documentation Agent
**Review Schedule:** With each major release