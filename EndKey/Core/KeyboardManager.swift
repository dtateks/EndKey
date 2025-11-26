import Cocoa
import Carbon

/// Manages keyboard event interception and Vietnamese input processing
class KeyboardManager {
    static let shared = KeyboardManager()

    private var eventTap: CFMachPort?
    private var runLoopSource: CFRunLoopSource?
    private var engine: InputEngine = TelexEngine()
    private(set) var isVietnameseMode = true

    // Timing delay between backspaces and replacement (microseconds)
    private let eventDelayMicroseconds: useconds_t = 1000

    var inputMethod: InputMethod = .telex {
        didSet {
            guard oldValue != inputMethod else { return }
            switch inputMethod {
            case .telex: engine = TelexEngine()
            case .vni: engine = VNIEngine()
            }
            engine.reset()
        }
    }

    private init() {}

    // MARK: - Public API

    func setVietnameseMode(_ enabled: Bool) {
        isVietnameseMode = enabled
        engine.reset()
    }

    func toggleVietnameseMode() {
        isVietnameseMode.toggle()
        engine.reset()
    }

    /// Setup EventTap for keyboard interception. Returns true on success.
    func setupEventTap() -> Bool {
        // Listen to keyboard events, mouse clicks, and scroll wheel
        let eventMask: CGEventMask =
            (1 << CGEventType.keyDown.rawValue) |
            (1 << CGEventType.leftMouseDown.rawValue) |
            (1 << CGEventType.rightMouseDown.rawValue) |
            (1 << CGEventType.scrollWheel.rawValue)

        let refcon = UnsafeMutableRawPointer(Unmanaged.passUnretained(self).toOpaque())

        eventTap = CGEvent.tapCreate(
            tap: .cgSessionEventTap,
            place: .headInsertEventTap,
            options: .defaultTap,
            eventsOfInterest: eventMask,
            callback: { (proxy, type, event, refcon) -> Unmanaged<CGEvent>? in
                guard let refcon = refcon else {
                    return Unmanaged.passRetained(event)
                }
                let manager = Unmanaged<KeyboardManager>.fromOpaque(refcon).takeUnretainedValue()
                return manager.handleEvent(proxy: proxy, type: type, event: event)
            },
            userInfo: refcon
        )

        guard let eventTap = eventTap else {
            return false
        }

        runLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, eventTap, 0)
        CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, .commonModes)
        CGEvent.tapEnable(tap: eventTap, enable: true)

        return true
    }

    func cleanup() {
        if let eventTap = eventTap {
            CGEvent.tapEnable(tap: eventTap, enable: false)
        }
        if let runLoopSource = runLoopSource {
            CFRunLoopRemoveSource(CFRunLoopGetCurrent(), runLoopSource, .commonModes)
        }
        eventTap = nil
        runLoopSource = nil
    }

    // MARK: - Event Handling

    private func handleEvent(
        proxy: CGEventTapProxy,
        type: CGEventType,
        event: CGEvent
    ) -> Unmanaged<CGEvent>? {
        // Re-enable tap if it got disabled
        if type == .tapDisabledByTimeout || type == .tapDisabledByUserInput {
            if let eventTap = eventTap {
                CGEvent.tapEnable(tap: eventTap, enable: true)
            }
            return Unmanaged.passRetained(event)
        }

        // Reset buffer on mouse events (click means cursor moved)
        if type == .leftMouseDown || type == .rightMouseDown || type == .scrollWheel {
            engine.reset()
            return Unmanaged.passRetained(event)
        }

        // Only process keyDown events from here
        guard type == .keyDown else {
            return Unmanaged.passRetained(event)
        }

        // Skip if Vietnamese mode is off
        guard isVietnameseMode else {
            return Unmanaged.passRetained(event)
        }

        // Get virtual key code
        let keyCode = event.getIntegerValueField(.keyboardEventKeycode)

        // Handle special keys that reset buffer
        if handleSpecialKeys(Int(keyCode)) {
            return Unmanaged.passRetained(event)
        }

        // Skip if modifier keys pressed (except Shift)
        let flags = event.flags
        if flags.contains(.maskCommand) || flags.contains(.maskControl) ||
           flags.contains(.maskAlternate) {
            return Unmanaged.passRetained(event)
        }

        // Get unicode character from event
        guard let char = getCharacter(from: event) else {
            return Unmanaged.passRetained(event)
        }

        // Handle word boundaries - reset buffer
        if isWordBoundary(char) {
            engine.reset()
            return Unmanaged.passRetained(event)
        }

        // Skip non-alphanumeric (except for VNI number handling)
        if !char.isLetter && !char.isNumber {
            return Unmanaged.passRetained(event)
        }

        // Process through engine
        let result = engine.processKey(char)

        // If transformation needed, suppress event and send replacement
        if result.backspaceCount > 0, let replacement = result.replacement {
            sendBackspaces(count: result.backspaceCount)
            usleep(eventDelayMicroseconds) // Add delay to prevent race conditions
            sendString(replacement)
            return nil // Suppress original event
        }

        return Unmanaged.passRetained(event)
    }

    /// Handle special keys that affect buffer state
    private func handleSpecialKeys(_ keyCode: Int) -> Bool {
        switch keyCode {
        case kVK_Delete:
            // Backspace - remove last char from buffer
            if !engine.buffer.isEmpty {
                var mutableEngine = engine
                mutableEngine.reset() // For now, reset on backspace
                engine = mutableEngine
            }
            return true

        case kVK_ForwardDelete:
            // Delete key - reset buffer
            engine.reset()
            return true

        case kVK_LeftArrow, kVK_RightArrow, kVK_UpArrow, kVK_DownArrow:
            // Arrow keys - cursor moved, reset buffer
            engine.reset()
            return true

        case kVK_Escape:
            // Escape key - reset buffer
            engine.reset()
            return true

        case kVK_Return, kVK_Tab:
            // Enter/Tab - word boundary
            engine.reset()
            return true

        case kVK_Home, kVK_End, kVK_PageUp, kVK_PageDown:
            // Navigation keys - reset buffer
            engine.reset()
            return true

        default:
            return false
        }
    }

    private func getCharacter(from event: CGEvent) -> Character? {
        var length = 0
        event.keyboardGetUnicodeString(maxStringLength: 4, actualStringLength: &length, unicodeString: nil)

        guard length > 0 else { return nil }

        var chars = [UniChar](repeating: 0, count: 4)
        event.keyboardGetUnicodeString(maxStringLength: 4, actualStringLength: &length, unicodeString: &chars)

        guard let scalar = Unicode.Scalar(chars[0]) else { return nil }
        return Character(scalar)
    }

    private func isWordBoundary(_ char: Character) -> Bool {
        return char == " " || char == "\n" || char == "\r" || char == "\t" ||
               char.isPunctuation || char == "." || char == "," || char == "!" ||
               char == "?" || char == ";" || char == ":"
    }

    // MARK: - Key Simulation

    private func sendBackspaces(count: Int) {
        let src = CGEventSource(stateID: .hidSystemState)

        for _ in 0..<count {
            let keyDown = CGEvent(keyboardEventSource: src, virtualKey: CGKeyCode(kVK_Delete), keyDown: true)
            let keyUp = CGEvent(keyboardEventSource: src, virtualKey: CGKeyCode(kVK_Delete), keyDown: false)
            keyDown?.post(tap: .cgAnnotatedSessionEventTap)
            keyUp?.post(tap: .cgAnnotatedSessionEventTap)
        }
    }

    private func sendString(_ string: String) {
        let src = CGEventSource(stateID: .hidSystemState)

        for char in string {
            let chars = Array(String(char).utf16)

            let keyDown = CGEvent(keyboardEventSource: src, virtualKey: 0, keyDown: true)
            keyDown?.keyboardSetUnicodeString(stringLength: chars.count, unicodeString: chars)
            keyDown?.post(tap: .cgAnnotatedSessionEventTap)

            let keyUp = CGEvent(keyboardEventSource: src, virtualKey: 0, keyDown: false)
            keyUp?.post(tap: .cgAnnotatedSessionEventTap)
        }
    }
}
