# EndKey - macOS Vietnamese Input App

## Brainstorm Summary Report
**Date:** 2025-11-26
**Status:** Ready for Implementation

---

## Problem Statement

Build lightweight macOS Vietnamese typing app using EventTap approach (not IME). Target: personal use first, public distribution later.

---

## Confirmed Requirements

| Requirement | Decision |
|-------------|----------|
| Approach | EventTap + backspace character replacement |
| Input methods | Simple Telex, VNI |
| UI | MenuBar icon (E/V) + dropdown + config panel |
| Hotkey | Cmd+Shift toggle En/Vi |
| macOS target | 11.0+ (Big Sur) |
| Distribution | Direct download (no notarization) |
| Tech stack | Pure Swift (SwiftUI) |

### Simple Telex Rules
- `aa→â, ee→ê, oo→ô, dd→đ`
- `aw→ă, ow→ơ, uw→ư`
- `s→sắc, f→huyền, r→hỏi, x→ngã, j→nặng`
- **NOT supported:** `w→ư, [→ơ, ]→ư`

### VNI Rules
- `a1→á, a2→à, a3→ả, a4→ã, a5→ạ`
- `a6→â, a7→ă, o7→ơ, u7→ư`
- `d9→đ`

### Config Panel
- Toggle: Telex / VNI
- Launch at login checkbox

---

## Technical Architecture

```
EndKey/
├── EndKey.xcodeproj
├── EndKey/
│   ├── EndKeyApp.swift              # @main SwiftUI entry
│   ├── AppDelegate.swift            # NSApplicationDelegate, EventTap setup
│   ├── Core/
│   │   ├── KeyboardManager.swift    # CGEvent tap, key interception
│   │   ├── InputEngine.swift        # Protocol + state machine
│   │   ├── TelexEngine.swift        # Telex composition rules
│   │   └── VNIEngine.swift          # VNI composition rules
│   ├── UI/
│   │   ├── MenuBarManager.swift     # NSStatusItem, dropdown menu
│   │   └── ConfigWindow.swift       # SwiftUI preferences panel
│   ├── Models/
│   │   └── AppState.swift           # ObservableObject, UserDefaults
│   └── Utils/
│       ├── PermissionHelper.swift   # Accessibility permission check
│       └── LaunchAtLogin.swift      # SMAppService wrapper
└── Assets.xcassets/
    └── MenuBarIcon/                 # E, V icons
```

### Key Design Decisions

| Decision | Rationale |
|----------|-----------|
| Stateless buffer (2-3 char lookback) | KISS - sufficient for Vietnamese composition |
| Protocol-based engines | DRY - TelexEngine/VNIEngine conform to InputEngine |
| SwiftUI for config | Modern, less code, macOS 11+ compatible |
| AppDelegate for EventTap | Need NSApplication lifecycle for CGEvent |

---

## Core Flow

```
┌─────────────┐     ┌──────────────────┐     ┌─────────────────┐
│  Keypress   │────▶│  KeyboardManager │────▶│  InputEngine    │
└─────────────┘     │  (EventTap)      │     │  (Telex/VNI)    │
                    └──────────────────┘     └────────┬────────┘
                                                      │
                              ┌────────────────────────┘
                              ▼
                    ┌──────────────────┐
                    │ Returns:         │
                    │ - backspaceCount │
                    │ - newCharacter   │
                    └────────┬─────────┘
                              │
                              ▼
                    ┌──────────────────┐
                    │ Send CGEvents:   │
                    │ 1. N backspaces  │
                    │ 2. New char      │
                    └──────────────────┘
```

---

## Risk Assessment

| Risk | Severity | Mitigation |
|------|----------|------------|
| Gatekeeper blocks unsigned app | Medium | README: `xattr -cr EndKey.app` or right-click Open |
| Accessibility permission denied | High | Clear first-run onboarding, auto-open System Prefs |
| EventTap timing issues | Medium | Configurable delay, extensive testing |
| Some apps don't handle backspace well | Low | Document known incompatibilities |

---

## Implementation Phases

### Phase 1: Foundation
- [ ] Xcode project setup
- [ ] AppDelegate with EventTap skeleton
- [ ] Accessibility permission flow

### Phase 2: Core Engine
- [ ] InputEngine protocol
- [ ] TelexEngine implementation
- [ ] VNIEngine implementation
- [ ] KeyboardManager integration

### Phase 3: UI
- [ ] MenuBar icon + dropdown
- [ ] Config panel (SwiftUI)
- [ ] Cmd+Shift hotkey

### Phase 4: Polish
- [ ] Launch at login
- [ ] App icon
- [ ] Build .dmg for distribution

---

## Success Criteria

1. Can type Vietnamese in any app (TextEdit, browser, etc.)
2. E/V toggle works via menu and hotkey
3. Both Telex and VNI work correctly
4. Survives reboot with launch-at-login
5. Clean permission onboarding

---

## Out of Scope (Future)

- Spell checking
- Custom hotkey configuration
- Advanced Telex (`w→ư, [/]` shortcuts)
- App Store distribution
- Auto-update mechanism

---

## Next Steps

Ready to execute `/plan` command to create detailed implementation plan.
