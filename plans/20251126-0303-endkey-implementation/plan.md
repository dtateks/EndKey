# EndKey Implementation Plan

## Overview
**Project:** EndKey - macOS Vietnamese Input App
**Date:** 2025-11-26
**Target:** macOS 11.0+ (Big Sur)
**Tech Stack:** Pure Swift/SwiftUI + EventTap

## Context
- Reference: [Brainstorm Summary](../251126-endkey-brainstorm-summary.md)
- Approach: EventTap + backspace character replacement (NOT IME)
- Input methods: Simple Telex, VNI
- Distribution: Direct download (no notarization)

## Implementation Phases

| Phase | Name | Status | Progress |
|-------|------|--------|----------|
| 01 | [Foundation](phase-01-foundation.md) | DONE (2025-11-26) | 100% |
| 02 | [Core Engine](phase-02-core-engine.md) | DONE (2025-11-26) | 100% |
| 03 | [UI Components](phase-03-ui.md) | DONE (2025-11-26) | 100% |
| 04 | [Polish & Distribution](phase-04-polish.md) | Pending | 0% |

## Dependencies
- Xcode 13+ (Swift 5.5+)
- macOS 11.0+ SDK
- No external packages required

## Key Architecture

```
EndKey/
├── EndKeyApp.swift          # @main SwiftUI entry
├── AppDelegate.swift        # NSApplicationDelegate, EventTap
├── Core/                    # Input engines
├── UI/                      # MenuBar, ConfigWindow
├── Models/                  # AppState
└── Utils/                   # Helpers
```

## Success Criteria
1. Vietnamese typing works in any app
2. E/V toggle via menu and Cmd+Shift
3. Both Telex/VNI engines functional
4. Launch at login works
5. Clean permission onboarding

## Risks
| Risk | Severity | Mitigation |
|------|----------|------------|
| Gatekeeper blocks | Medium | README: xattr -cr |
| Accessibility denied | High | Clear onboarding flow |
| EventTap timing | Medium | Configurable delay |
