# Project Overview
EndKey is a lightweight Vietnamese input method for macOS built with an EventTap architecture. It intercepts keyboard events system-wide to transform English keystrokes into Vietnamese characters using Telex or VNI input methods. It runs as a background agent (LSUIElement) with a menu bar interface and requires no traditional IME registration.

**Tech Stack**:
- **Language**: Swift 5+
- **Frameworks**: SwiftUI (UI), Core Graphics (EventTap), AppKit (System integration)
- **Architecture**: EventTap-based interception, Protocol-oriented engines, Singleton core services
- **Distribution**: Direct DMG download (Ad-hoc signed), no App Store

# Build & Run Commands
- **Build Release**: `./scripts/build-release.sh` (Creates optimized build in `build/`)
- **Create Installer**: `./scripts/create-dmg.sh` (Requires release build, creates `EndKey.dmg`)
- **Build Debug**: `xcodebuild -project EndKey.xcodeproj -scheme EndKey -configuration Debug build`
- **Clean**: `rm -rf build/`
- **Run Locally**: Open `EndKey.xcodeproj` in Xcode and run (Cmd+R). Requires granting Accessibility permission to Xcode or the built app.

# Test & Quality Commands
- **Run All Tests**: `xcodebuild test -project EndKey.xcodeproj -scheme EndKey -destination 'platform=macOS'`
- **Run Specific Test Suite**: `xcodebuild test -project EndKey.xcodeproj -scheme EndKey -only-testing:EndKeyTests/EngineTests`
- **Lint/Format**: Follow SwiftLint rules if present (currently relies on Xcode default formatting).

# Architecture & Code Structure
**High-Level Architecture**:
- **EventTap Layer**: Intercepts `CGEvent` at the system level (`KeyboardManager.swift`).
- **Input Processing**: Transforms characters via `InputEngine` protocols (`TelexEngine`, `VNIEngine`).
- **State Management**: `AppState` singleton with `@AppStorage` persistence and `NotificationCenter` for UI sync.
- **UI Layer**: SwiftUI-based `ConfigView` and `MenuBarManager` for system status item.

**Key Directories**:
- `EndKey/Core/`: Input logic (`InputEngine.swift`, `TelexEngine.swift`, `VNIEngine.swift`, `KeyboardManager.swift`).
- `EndKey/UI/`: User interface (`MenuBarManager.swift`, `ConfigView.swift`).
- `EndKey/Models/`: Data models and state (`AppState.swift`).
- `EndKey/Utils/`: Helpers (`PermissionHelper.swift`).
- `EndKeyTests/`: Unit tests for engines and state.
- `scripts/`: Build and distribution scripts.

**Critical Files**:
- `KeyboardManager.swift`: Core event processing loop.
- `InputEngine.swift`: Protocol definition for input methods.
- `AppState.swift`: Central state management.

# Coding Style & Conventions
- **Naming**: PascalCase for types, camelCase for functions/vars.
- **Structure**: Use `MARK: - Section` to organize classes.
- **Access Control**: Prefer `private` over `internal`. Use `final` for classes not intended for subclassing.
- **Concurrency**: EventTap callbacks run on a dedicated system thread; UI updates MUST be dispatched to MainActor/MainThread.
- **Error Handling**: Graceful degradation on permission denial. Use `guard` for early returns.
- **Protocol-Oriented**: Use protocols (`InputEngine`) to define behavior and enable easy extension (e.g., new input methods).

# Testing Strategy
- **Unit Tests**: Focus on `InputEngine` implementations (`TelexEngineTests`, `VNIEngineTests`) to verify character transformations (e.g., `aa` -> `â`).
- **Integration Tests**: Verify `AppState` persistence and `KeyboardManager` state synchronization.
- **Manual Testing**: Required for EventTap interactions (e.g., typing in different apps, handling modifiers) as these are hard to automate fully.
- **Performance**: Event processing must be <10ms. Avoid heavy allocations in the hot path (`handleKeyEvent`).

# Environment & Secrets
- **Secrets**: None. This is a local-only app with no network dependencies.
- **Permissions**: Requires **Accessibility** permission (`AXIsProcessTrusted`). The app handles checking and prompting for this.
- **Local Dev**: No special env vars required. Just build and run.

# Workflow & Common Tasks
- **Adding Input Method**:
    1. Create struct implementing `InputEngine`.
    2. Add case to `InputMethod` enum in `AppState.swift`.
    3. Update `ConfigView` to include the new option.
    4. Add unit tests.
- **Debugging**: Check Console.app for logs if EventTap disables itself (timeout).
- **Release**: Run `./scripts/build-release.sh` then `./scripts/create-dmg.sh`.

# Guidelines For AI Agents
- **Architecture**: Respect the EventTap architecture. Do NOT suggest changing to `IMKit` unless explicitly requested (EventTap is a core design choice).
- **Thread Safety**: ALWAYS ensure UI updates happen on the Main Thread. Be very careful with shared state accessed from the EventTap callback.
- **Performance**: The `handleKeyEvent` function is critical. Do not add blocking calls or heavy logic there.
- **Simplicity**: Prefer simple, robust logic for character transformation over complex regex if possible.
- **Documentation**: Update `docs/` if changing architectural components.

# Security & Compliance
- **Accessibility**: The app MUST have Accessibility permissions to function. Handle the "denied" state gracefully.
- **Privacy**: The app intercepts all keystrokes. Ensure NO data is logged, stored, or transmitted. Processing is strictly in-memory and ephemeral.
- **Sandboxing**: The app is NOT sandboxed (required for EventTap).
