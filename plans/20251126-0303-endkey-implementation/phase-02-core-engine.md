# Phase 02: Core Engine

## Context Links
- Parent: [plan.md](plan.md)
- Dependency: [Phase 01](phase-01-foundation.md)
- Reference: [Brainstorm Summary](../251126-endkey-brainstorm-summary.md)

## Overview
- **Priority:** P0 (Critical)
- **Status:** Pending
- **Description:** Implement InputEngine protocol, TelexEngine, VNIEngine, and KeyboardManager

## Key Insights
- Stateless buffer (2-3 char lookback) sufficient for Vietnamese
- Protocol-based design enables easy engine switching
- CGEvent keycode mapping required for character detection
- Backspace + character replacement timing critical

## Requirements

### Functional
- Telex input produces correct Vietnamese characters
- VNI input produces correct Vietnamese characters
- Engine switching works seamlessly
- Handles uppercase/lowercase correctly

### Non-functional
- Low latency (<50ms response)
- No memory leaks in buffer management

## Architecture

```
┌─────────────────┐
│ KeyboardManager │ Intercepts keyDown events
└────────┬────────┘
         │ CGEvent keycode + char
         ▼
┌─────────────────┐
│  InputEngine    │ Protocol: processKey()
│  (Protocol)     │
└────────┬────────┘
         │
    ┌────┴────┐
    ▼         ▼
┌────────┐ ┌────────┐
│ Telex  │ │  VNI   │
│ Engine │ │ Engine │
└────────┘ └────────┘
         │
         ▼
┌─────────────────┐
│ ProcessResult   │ backspaceCount, replacement
└─────────────────┘
```

## Related Code Files

### Create
| File | Description |
|------|-------------|
| EndKey/Core/InputEngine.swift | Protocol + result types |
| EndKey/Core/TelexEngine.swift | Simple Telex rules |
| EndKey/Core/VNIEngine.swift | VNI rules |
| EndKey/Core/KeyboardManager.swift | EventTap integration |
| EndKey/Core/VietnameseData.swift | Character mappings |

### Modify
| File | Change |
|------|--------|
| EndKey/AppDelegate.swift | Integrate KeyboardManager |

## Implementation Steps

### Step 1: Create VietnameseData.swift
```swift
import Foundation

struct VietnameseData {
    // Base vowels that can receive diacritics
    static let baseVowels: Set<Character> = ["a", "e", "i", "o", "u", "y"]
    static let modifiedVowels: Set<Character> = ["ă", "â", "ê", "ô", "ơ", "ư"]

    // Tone marks (sắc, huyền, hỏi, ngã, nặng)
    static let toneMarks: [Character: [Character: Character]] = [
        // Base vowel: [tone: result]
        "a": ["1": "á", "2": "à", "3": "ả", "4": "ã", "5": "ạ"],
        "ă": ["1": "ắ", "2": "ằ", "3": "ẳ", "4": "ẵ", "5": "ặ"],
        "â": ["1": "ấ", "2": "ầ", "3": "ẩ", "4": "ẫ", "5": "ậ"],
        "e": ["1": "é", "2": "è", "3": "ẻ", "4": "ẽ", "5": "ẹ"],
        "ê": ["1": "ế", "2": "ề", "3": "ể", "4": "ễ", "5": "ệ"],
        "i": ["1": "í", "2": "ì", "3": "ỉ", "4": "ĩ", "5": "ị"],
        "o": ["1": "ó", "2": "ò", "3": "ỏ", "4": "õ", "5": "ọ"],
        "ô": ["1": "ố", "2": "ồ", "3": "ổ", "4": "ỗ", "5": "ộ"],
        "ơ": ["1": "ớ", "2": "ờ", "3": "ở", "4": "ỡ", "5": "ợ"],
        "u": ["1": "ú", "2": "ù", "3": "ủ", "4": "ũ", "5": "ụ"],
        "ư": ["1": "ứ", "2": "ừ", "3": "ử", "4": "ữ", "5": "ự"],
        "y": ["1": "ý", "2": "ỳ", "3": "ỷ", "4": "ỹ", "5": "ỵ"]
    ]

    // Telex tone keys
    static let telexTones: [Character: Character] = [
        "s": "1", // sắc
        "f": "2", // huyền
        "r": "3", // hỏi
        "x": "4", // ngã
        "j": "5"  // nặng
    ]

    // Vowel transformations
    static let vowelTransforms: [String: Character] = [
        "aa": "â", "ee": "ê", "oo": "ô",
        "aw": "ă", "ow": "ơ", "uw": "ư",
        "a6": "â", "e6": "ê", "o6": "ô",  // VNI
        "a7": "ă", "o7": "ơ", "u7": "ư"   // VNI
    ]
}
```

### Step 2: Create InputEngine.swift
```swift
import Foundation

/// Result of processing a keystroke
struct ProcessResult {
    let backspaceCount: Int
    let replacement: String?

    static let passthrough = ProcessResult(backspaceCount: 0, replacement: nil)
}

/// Protocol for Vietnamese input engines
protocol InputEngine {
    /// Process incoming character, return transformation
    mutating func processKey(_ char: Character) -> ProcessResult

    /// Reset engine state (e.g., on space, enter)
    mutating func reset()

    /// Current buffer content
    var buffer: [Character] { get }
}

/// Input method type
enum InputMethod: String, CaseIterable {
    case telex = "Telex"
    case vni = "VNI"
}
```

### Step 3: Create TelexEngine.swift
```swift
import Foundation

struct TelexEngine: InputEngine {
    private(set) var buffer: [Character] = []
    private let maxBufferSize = 3

    mutating func processKey(_ char: Character) -> ProcessResult {
        let lowerChar = Character(char.lowercased())

        // Check for tone marks (s, f, r, x, j)
        if let toneKey = VietnameseData.telexTones[lowerChar] {
            if let result = applyTone(toneKey, isUppercase: char.isUppercase) {
                return result
            }
        }

        // Check for vowel doubling (aa, ee, oo)
        if let lastChar = buffer.last {
            let combo = String([lastChar.lowercased().first!, lowerChar])
            if let transformed = VietnameseData.vowelTransforms[combo] {
                let finalChar = char.isUppercase ? Character(transformed.uppercased()) : transformed
                buffer.removeLast()
                buffer.append(finalChar)
                return ProcessResult(backspaceCount: 1, replacement: String(finalChar))
            }
        }

        // Check for dd → đ
        if lowerChar == "d" && buffer.last?.lowercased() == "d" {
            let finalChar: Character = char.isUppercase ? "Đ" : "đ"
            buffer.removeLast()
            buffer.append(finalChar)
            return ProcessResult(backspaceCount: 1, replacement: String(finalChar))
        }

        // Regular character - add to buffer
        buffer.append(char)
        if buffer.count > maxBufferSize {
            buffer.removeFirst()
        }

        return .passthrough
    }

    private mutating func applyTone(_ toneKey: Character, isUppercase: Bool) -> ProcessResult? {
        // Find last vowel in buffer that can receive tone
        for i in stride(from: buffer.count - 1, through: 0, by: -1) {
            let vowel = buffer[i]
            let lowerVowel = Character(vowel.lowercased())

            // Get base vowel (remove existing tone if any)
            let baseVowel = getBaseVowel(lowerVowel)

            if let toneMap = VietnameseData.toneMarks[baseVowel],
               let toned = toneMap[toneKey] {
                let finalChar = vowel.isUppercase ? Character(toned.uppercased()) : toned
                let oldChar = buffer[i]
                buffer[i] = finalChar

                // Calculate backspace: need to delete from current position to vowel
                let backspaces = buffer.count - i
                let replacement = String(buffer[i...])

                return ProcessResult(backspaceCount: backspaces, replacement: replacement)
            }
        }
        return nil
    }

    private func getBaseVowel(_ char: Character) -> Character {
        // Map toned vowels back to base
        let tonedToBase: [Character: Character] = [
            "á": "a", "à": "a", "ả": "a", "ã": "a", "ạ": "a",
            "ắ": "ă", "ằ": "ă", "ẳ": "ă", "ẵ": "ă", "ặ": "ă",
            "ấ": "â", "ầ": "â", "ẩ": "â", "ẫ": "â", "ậ": "â",
            "é": "e", "è": "e", "ẻ": "e", "ẽ": "e", "ẹ": "e",
            "ế": "ê", "ề": "ê", "ể": "ê", "ễ": "ê", "ệ": "ê",
            "í": "i", "ì": "i", "ỉ": "i", "ĩ": "i", "ị": "i",
            "ó": "o", "ò": "o", "ỏ": "o", "õ": "o", "ọ": "o",
            "ố": "ô", "ồ": "ô", "ổ": "ô", "ỗ": "ô", "ộ": "ô",
            "ớ": "ơ", "ờ": "ơ", "ở": "ơ", "ỡ": "ơ", "ợ": "ơ",
            "ú": "u", "ù": "u", "ủ": "u", "ũ": "u", "ụ": "u",
            "ứ": "ư", "ừ": "ư", "ử": "ư", "ữ": "ư", "ự": "ư",
            "ý": "y", "ỳ": "y", "ỷ": "y", "ỹ": "y", "ỵ": "y"
        ]
        return tonedToBase[char] ?? char
    }

    mutating func reset() {
        buffer.removeAll()
    }
}
```

### Step 4: Create VNIEngine.swift
```swift
import Foundation

struct VNIEngine: InputEngine {
    private(set) var buffer: [Character] = []
    private let maxBufferSize = 3

    mutating func processKey(_ char: Character) -> ProcessResult {
        // VNI uses numbers for tones: 1-5 for tones, 6-7 for vowel mods, 9 for đ

        // Check for d9 → đ
        if char == "9" && buffer.last?.lowercased() == "d" {
            let isUpper = buffer.last?.isUppercase ?? false
            let finalChar: Character = isUpper ? "Đ" : "đ"
            buffer.removeLast()
            buffer.append(finalChar)
            return ProcessResult(backspaceCount: 1, replacement: String(finalChar))
        }

        // Check for tone marks (1-5)
        if let digit = char.wholeNumberValue, (1...5).contains(digit) {
            if let result = applyTone(Character("\(digit)")) {
                return result
            }
        }

        // Check for vowel modifiers (6, 7)
        if char == "6" || char == "7" {
            if let result = applyVowelModifier(char) {
                return result
            }
        }

        // Regular character
        buffer.append(char)
        if buffer.count > maxBufferSize {
            buffer.removeFirst()
        }

        return .passthrough
    }

    private mutating func applyTone(_ toneKey: Character) -> ProcessResult? {
        for i in stride(from: buffer.count - 1, through: 0, by: -1) {
            let vowel = buffer[i]
            let lowerVowel = Character(vowel.lowercased())
            let baseVowel = getBaseVowel(lowerVowel)

            if let toneMap = VietnameseData.toneMarks[baseVowel],
               let toned = toneMap[toneKey] {
                let finalChar = vowel.isUppercase ? Character(toned.uppercased()) : toned
                buffer[i] = finalChar

                let backspaces = buffer.count - i
                let replacement = String(buffer[i...])
                return ProcessResult(backspaceCount: backspaces, replacement: replacement)
            }
        }
        return nil
    }

    private mutating func applyVowelModifier(_ modifier: Character) -> ProcessResult? {
        guard let lastChar = buffer.last else { return nil }
        let lowerChar = Character(lastChar.lowercased())

        let combo = String([lowerChar, modifier])
        if let transformed = VietnameseData.vowelTransforms[combo] {
            let finalChar = lastChar.isUppercase ? Character(transformed.uppercased()) : transformed
            buffer.removeLast()
            buffer.append(finalChar)
            return ProcessResult(backspaceCount: 1, replacement: String(finalChar))
        }
        return nil
    }

    private func getBaseVowel(_ char: Character) -> Character {
        // Same logic as TelexEngine - consider extracting to shared utility
        let tonedToBase: [Character: Character] = [
            "á": "a", "à": "a", "ả": "a", "ã": "a", "ạ": "a",
            "ắ": "ă", "ằ": "ă", "ẳ": "ă", "ẵ": "ă", "ặ": "ă",
            "ấ": "â", "ầ": "â", "ẩ": "â", "ẫ": "â", "ậ": "â",
            "é": "e", "è": "e", "ẻ": "e", "ẽ": "e", "ẹ": "e",
            "ế": "ê", "ề": "ê", "ể": "ê", "ễ": "ê", "ệ": "ê",
            "í": "i", "ì": "i", "ỉ": "i", "ĩ": "i", "ị": "i",
            "ó": "o", "ò": "o", "ỏ": "o", "õ": "o", "ọ": "o",
            "ố": "ô", "ồ": "ô", "ổ": "ô", "ỗ": "ô", "ộ": "ô",
            "ớ": "ơ", "ờ": "ơ", "ở": "ơ", "ỡ": "ơ", "ợ": "ơ",
            "ú": "u", "ù": "u", "ủ": "u", "ũ": "u", "ụ": "u",
            "ứ": "ư", "ừ": "ư", "ử": "ư", "ữ": "ư", "ự": "ư",
            "ý": "y", "ỳ": "y", "ỷ": "y", "ỹ": "y", "ỵ": "y"
        ]
        return tonedToBase[char] ?? char
    }

    mutating func reset() {
        buffer.removeAll()
    }
}
```

### Step 5: Create KeyboardManager.swift
```swift
import Cocoa
import Carbon

class KeyboardManager {
    static let shared = KeyboardManager()

    private var eventTap: CFMachPort?
    private var engine: InputEngine = TelexEngine()
    private var isVietnameseMode = true

    var inputMethod: InputMethod = .telex {
        didSet {
            switch inputMethod {
            case .telex: engine = TelexEngine()
            case .vni: engine = VNIEngine()
            }
        }
    }

    func setVietnameseMode(_ enabled: Bool) {
        isVietnameseMode = enabled
        engine.reset()
    }

    func toggleVietnameseMode() {
        isVietnameseMode.toggle()
        engine.reset()
    }

    func setupEventTap() -> Bool {
        let eventMask = (1 << CGEventType.keyDown.rawValue)

        let refcon = UnsafeMutableRawPointer(Unmanaged.passUnretained(self).toOpaque())

        eventTap = CGEvent.tapCreate(
            tap: .cgSessionEventTap,
            place: .headInsertEventTap,
            options: .defaultTap,
            eventsOfInterest: CGEventMask(eventMask),
            callback: { (proxy, type, event, refcon) -> Unmanaged<CGEvent>? in
                guard let refcon = refcon else { return Unmanaged.passRetained(event) }
                let manager = Unmanaged<KeyboardManager>.fromOpaque(refcon).takeUnretainedValue()
                return manager.handleKeyEvent(proxy: proxy, type: type, event: event)
            },
            userInfo: refcon
        )

        guard let eventTap = eventTap else { return false }

        let runLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, eventTap, 0)
        CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, .commonModes)
        CGEvent.tapEnable(tap: eventTap, enable: true)

        return true
    }

    private func handleKeyEvent(proxy: CGEventTapProxy, type: CGEventType, event: CGEvent) -> Unmanaged<CGEvent>? {
        // Skip if not in Vietnamese mode
        guard isVietnameseMode else {
            return Unmanaged.passRetained(event)
        }

        // Get the character
        var length = 0
        event.keyboardGetUnicodeString(maxStringLength: 4, actualStringLength: &length, unicodeString: nil)

        var chars = [UniChar](repeating: 0, count: 4)
        event.keyboardGetUnicodeString(maxStringLength: 4, actualStringLength: &length, unicodeString: &chars)

        guard length > 0, let scalar = Unicode.Scalar(chars[0]) else {
            return Unmanaged.passRetained(event)
        }

        let char = Character(scalar)

        // Skip non-printable characters
        guard char.isLetter || char.isNumber else {
            // Reset buffer on space, enter, etc.
            if char == " " || char == "\n" || char == "\r" {
                engine.reset()
            }
            return Unmanaged.passRetained(event)
        }

        // Process through engine
        let result = engine.processKey(char)

        if result.backspaceCount > 0, let replacement = result.replacement {
            // Suppress original event
            // Send backspaces then replacement
            DispatchQueue.main.async {
                self.sendBackspaces(count: result.backspaceCount)
                self.sendString(replacement)
            }
            return nil // Suppress original
        }

        return Unmanaged.passRetained(event)
    }

    private func sendBackspaces(count: Int) {
        for _ in 0..<count {
            let keyDown = CGEvent(keyboardEventSource: nil, virtualKey: CGKeyCode(kVK_Delete), keyDown: true)
            let keyUp = CGEvent(keyboardEventSource: nil, virtualKey: CGKeyCode(kVK_Delete), keyDown: false)
            keyDown?.post(tap: .cgAnnotatedSessionEventTap)
            keyUp?.post(tap: .cgAnnotatedSessionEventTap)
        }
    }

    private func sendString(_ string: String) {
        for char in string {
            let chars = Array(String(char).utf16)
            let keyDown = CGEvent(keyboardEventSource: nil, virtualKey: 0, keyDown: true)
            keyDown?.keyboardSetUnicodeString(stringLength: chars.count, unicodeString: chars)
            keyDown?.post(tap: .cgAnnotatedSessionEventTap)

            let keyUp = CGEvent(keyboardEventSource: nil, virtualKey: 0, keyDown: false)
            keyUp?.post(tap: .cgAnnotatedSessionEventTap)
        }
    }

    func cleanup() {
        if let eventTap = eventTap {
            CGEvent.tapEnable(tap: eventTap, enable: false)
        }
    }
}
```

### Step 6: Update AppDelegate
Modify to use KeyboardManager:
```swift
// In AppDelegate.swift
func applicationDidFinishLaunching(_ notification: Notification) {
    checkAccessibilityPermission()
}

private func setupKeyboard() {
    if KeyboardManager.shared.setupEventTap() {
        print("Keyboard manager setup complete")
    } else {
        print("Failed to setup keyboard manager")
    }
}
```

## Todo List
- [ ] Create VietnameseData.swift with character mappings
- [ ] Create InputEngine.swift protocol
- [ ] Implement TelexEngine with all rules
- [ ] Implement VNIEngine with all rules
- [ ] Create KeyboardManager with EventTap integration
- [ ] Update AppDelegate to use KeyboardManager
- [ ] Test Telex typing (aa, aw, dd, tones)
- [ ] Test VNI typing (a6, a7, d9, tones)
- [ ] Test engine switching

## Success Criteria
- [ ] "xin chaof" produces "xin chào" (Telex)
- [ ] "xin cha2o" produces "xin chào" (VNI)
- [ ] "ddi" produces "đi"
- [ ] "aw" produces "ă"
- [ ] Uppercase preserved correctly

## Risk Assessment
| Risk | Likelihood | Impact | Mitigation |
|------|------------|--------|------------|
| Timing issues | Medium | Medium | Add delays between events |
| Buffer overflow | Low | Low | Max buffer size limit |
| Character encoding | Low | Medium | UTF-16 handling |

## Security Considerations
- EventTap sees all keystrokes - no logging/storage
- Buffer cleared on space/enter
- No network transmission

## Next Steps
→ Phase 03: UI Components (MenuBar, Config Panel)
