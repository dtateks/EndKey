# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

EndKey is a lightweight Vietnamese input method for macOS built with EventTap architecture. It's a native Swift/SwiftUI application that intercepts keyboard events system-wide to transform English keystrokes into Vietnamese characters using Telex or VNI input methods.

## Essential Commands

### Build and Distribution
```bash
# Build release version
./scripts/build-release.sh

# Create DMG installer (requires build-release.sh first)
./scripts/create-dmg.sh

# Clean build artifacts
rm -rf build/
```

### Development and Testing
```bash
# Run tests via Xcode
xcodebuild test -project EndKey.xcodeproj -scheme EndKey -destination 'platform=macOS'

# Build debug version
xcodebuild -project EndKey.xcodeproj -scheme EndKey -configuration Debug build

# Run specific test file
xcodebuild test -project EndKey.xcodeproj -scheme EndKey -only-testing:EndKeyTests/EngineTests
```

## Architecture Overview

### Core Design Pattern: EventTap Interception
The application uses macOS EventTap to intercept keyboard events at the Core Graphics level, avoiding traditional IME registration complexity:

```
User Input → CGEvent.tapCreate() → Vietnamese Logic → CGEventSetStringValue() → System Output
```

### Key Architectural Components

**EventTap Subsystem** (`KeyboardManager.swift`):
- Session-level keyboard event interception
- Thread-safe processing on dedicated system thread
- Automatic re-enable on timeout/disabling
- Zero external dependencies (pure system frameworks)

**Input Processing Engines** (`Core/`):
- Protocol-based design: `InputEngine` with `TelexEngine` and `VNIEngine` implementations
- Buffer management: Maximum 7 characters (longest Vietnamese syllable)
- Tone placement algorithm: Priority-based (modified vowels → last vowel → penultimate)

**State Management** (`AppState.swift`):
- Singleton pattern with `@AppStorage` persistence
- Bidirectional sync between AppState and KeyboardManager
- NotificationCenter for decoupled UI updates

**System Integration**:
- Menu bar integration: NSStatusItem with V/E indicator
- Global hotkey: Cmd+Shift for mode toggling
- Launch-at-login capability
- Accessibility permission handling

### Critical Implementation Details

**Performance Requirements**:
- Event processing latency: <10ms target
- Buffer size: 7 characters maximum
- Memory usage: <20MB runtime

**Security Constraints**:
- Requires Accessibility permission (EventTap requirement)
- No sandboxing (EventTap requirement)
- No network access (privacy-focused design)
- Ad-hoc signing for direct distribution

**Thread Safety**:
- EventTap runs on dedicated system thread
- KeyboardManager uses serial queue for state updates
- AppState uses @MainActor for UI updates

## Code Standards

**Swift Conventions**:
- PascalCase for types, camelCase for functions/variables
- Protocol-oriented design for extensibility
- Comprehensive inline documentation
- Maximum file size: 200 lines (enforced by modular design)

**Error Handling**:
- Graceful degradation on permission denial
- Retry logic for permission requests (max 5 attempts)
- User-friendly error dialogs with recovery options

**Testing Strategy**:
- Unit tests for core engine transformations
- Integration tests for state management
- Performance monitoring for event processing

## Common Development Tasks

**Adding New Input Methods**:
1. Create new engine implementing `InputEngine` protocol
2. Add to `InputMethod` enum in `AppState.swift`
3. Update `ConfigView.swift` UI
4. Add comprehensive test coverage

**Debugging EventTap Issues**:
1. Check Accessibility permission status
2. Verify EventTap creation success
3. Monitor console logs for timeout/disabling events
4. Test with different applications

**Performance Optimization**:
1. Profile event processing in Instruments
2. Optimize string operations in transformation logic
3. Minimize allocations in hot path
4. Test with rapid keystroke sequences

## Project Structure

```
EndKey/
├── EndKey/Core/          # Vietnamese input logic (engines, data)
├── EndKey/UI/            # User interface (menu bar, settings)
├── EndKey/Models/        # Application state and models
├── EndKey/Utils/         # Utilities (permissions, helpers)
├── EndKeyTests/          # XCTest unit tests
├── scripts/              # Build automation
└── docs/                 # Technical documentation
```

## Key Files to Understand First

1. `KeyboardManager.swift` - EventTap implementation and event processing
2. `InputEngine.swift` - Protocol definitions and transformation logic
3. `AppState.swift` - Application state management
4. `AppDelegate.swift` - Application lifecycle and permission handling

## Distribution Notes

- Target: macOS 11.0+ (Big Sur or later)
- Bundle identifier: com.endkey.EndKey
- LSUIElement: true (background agent, no dock icon)
- Direct distribution without App Store/notarization
- DMG creation includes Applications symlink for drag-to-install