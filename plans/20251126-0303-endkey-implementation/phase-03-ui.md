# Phase 03: UI Components

## Context Links
- Parent: [plan.md](plan.md)
- Dependency: [Phase 02](phase-02-core-engine.md)
- Reference: [Brainstorm Summary](../251126-endkey-brainstorm-summary.md)

## Overview
- **Priority:** P1 (High)
- **Status:** Pending
- **Description:** Implement MenuBar with dropdown, Config panel (SwiftUI), and Cmd+Shift hotkey

## Key Insights
- NSStatusItem for menubar icon
- SwiftUI Window for preferences panel
- Global hotkey via addGlobalMonitorForEvents
- Icon should clearly indicate E/V state

## Requirements

### Functional
- Menubar shows "E" or "V" based on mode
- Dropdown menu: Toggle mode, Telex/VNI selection, Preferences, Quit
- Config panel: Input method picker, launch at login toggle
- Cmd+Shift toggles between En/Vi

### Non-functional
- Icon readable at all menubar sizes
- Config panel follows macOS design guidelines

## Architecture

```
┌─────────────────┐
│  MenuBarManager │ NSStatusItem + NSMenu
└────────┬────────┘
         │
    ┌────┴────┐
    ▼         ▼
┌────────┐ ┌────────────┐
│ AppState│ │ ConfigWindow│
│(Shared) │ │ (SwiftUI)  │
└────────┘ └────────────┘
```

## Related Code Files

### Create
| File | Description |
|------|-------------|
| EndKey/Models/AppState.swift | Shared state with UserDefaults |
| EndKey/UI/MenuBarManager.swift | NSStatusItem, menu construction |
| EndKey/UI/ConfigWindow.swift | SwiftUI preferences view |

### Modify
| File | Change |
|------|--------|
| EndKey/AppDelegate.swift | Initialize MenuBarManager, global hotkey |

## Implementation Steps

### Step 1: Create AppState.swift
```swift
import SwiftUI
import Combine

class AppState: ObservableObject {
    static let shared = AppState()

    @AppStorage("isVietnameseMode") var isVietnameseMode: Bool = true {
        didSet {
            KeyboardManager.shared.setVietnameseMode(isVietnameseMode)
            NotificationCenter.default.post(name: .modeChanged, object: nil)
        }
    }

    @AppStorage("inputMethod") var inputMethodRaw: String = InputMethod.telex.rawValue {
        didSet {
            if let method = InputMethod(rawValue: inputMethodRaw) {
                KeyboardManager.shared.inputMethod = method
            }
        }
    }

    var inputMethod: InputMethod {
        get { InputMethod(rawValue: inputMethodRaw) ?? .telex }
        set { inputMethodRaw = newValue.rawValue }
    }

    @AppStorage("launchAtLogin") var launchAtLogin: Bool = false

    func toggleMode() {
        isVietnameseMode.toggle()
    }

    private init() {
        // Sync initial state
        KeyboardManager.shared.setVietnameseMode(isVietnameseMode)
        if let method = InputMethod(rawValue: inputMethodRaw) {
            KeyboardManager.shared.inputMethod = method
        }
    }
}

extension Notification.Name {
    static let modeChanged = Notification.Name("modeChanged")
}
```

### Step 2: Create MenuBarManager.swift
```swift
import Cocoa
import SwiftUI

class MenuBarManager: NSObject {
    private var statusItem: NSStatusItem?
    private var configWindow: NSWindow?

    func setup() {
        statusItem = NSStatusBar.system.statusItem(withLength: NSStatusItem.variableLength)

        updateIcon()
        setupMenu()

        // Listen for mode changes
        NotificationCenter.default.addObserver(
            self,
            selector: #selector(updateIcon),
            name: .modeChanged,
            object: nil
        )
    }

    @objc func updateIcon() {
        guard let button = statusItem?.button else { return }

        let icon = AppState.shared.isVietnameseMode ? "V" : "E"
        button.title = icon

        // Style
        button.font = NSFont.monospacedSystemFont(ofSize: 14, weight: .bold)
    }

    private func setupMenu() {
        let menu = NSMenu()

        // Mode toggle
        let modeItem = NSMenuItem(
            title: "Vietnamese Mode",
            action: #selector(toggleMode),
            keyEquivalent: ""
        )
        modeItem.target = self
        modeItem.state = AppState.shared.isVietnameseMode ? .on : .off
        menu.addItem(modeItem)

        menu.addItem(NSMenuItem.separator())

        // Input method submenu
        let methodMenu = NSMenu()
        for method in InputMethod.allCases {
            let item = NSMenuItem(
                title: method.rawValue,
                action: #selector(selectInputMethod(_:)),
                keyEquivalent: ""
            )
            item.target = self
            item.representedObject = method
            item.state = AppState.shared.inputMethod == method ? .on : .off
            methodMenu.addItem(item)
        }
        let methodItem = NSMenuItem(title: "Input Method", action: nil, keyEquivalent: "")
        methodItem.submenu = methodMenu
        menu.addItem(methodItem)

        menu.addItem(NSMenuItem.separator())

        // Preferences
        let prefsItem = NSMenuItem(
            title: "Preferences...",
            action: #selector(showPreferences),
            keyEquivalent: ","
        )
        prefsItem.target = self
        menu.addItem(prefsItem)

        menu.addItem(NSMenuItem.separator())

        // Quit
        let quitItem = NSMenuItem(
            title: "Quit EndKey",
            action: #selector(quitApp),
            keyEquivalent: "q"
        )
        quitItem.target = self
        menu.addItem(quitItem)

        statusItem?.menu = menu
    }

    @objc private func toggleMode() {
        AppState.shared.toggleMode()
        setupMenu() // Refresh menu state
    }

    @objc private func selectInputMethod(_ sender: NSMenuItem) {
        guard let method = sender.representedObject as? InputMethod else { return }
        AppState.shared.inputMethod = method
        setupMenu() // Refresh menu state
    }

    @objc private func showPreferences() {
        if configWindow == nil {
            let contentView = ConfigView()
            configWindow = NSWindow(
                contentRect: NSRect(x: 0, y: 0, width: 300, height: 200),
                styleMask: [.titled, .closable],
                backing: .buffered,
                defer: false
            )
            configWindow?.title = "EndKey Preferences"
            configWindow?.contentView = NSHostingView(rootView: contentView)
            configWindow?.center()
        }
        configWindow?.makeKeyAndOrderFront(nil)
        NSApp.activate(ignoringOtherApps: true)
    }

    @objc private func quitApp() {
        NSApp.terminate(nil)
    }
}
```

### Step 3: Create ConfigWindow.swift
```swift
import SwiftUI

struct ConfigView: View {
    @ObservedObject private var appState = AppState.shared

    var body: some View {
        Form {
            Section {
                Picker("Input Method", selection: $appState.inputMethod) {
                    ForEach(InputMethod.allCases, id: \.self) { method in
                        Text(method.rawValue).tag(method)
                    }
                }
                .pickerStyle(.segmented)
            }

            Section {
                Toggle("Launch at Login", isOn: $appState.launchAtLogin)
                    .onChange(of: appState.launchAtLogin) { newValue in
                        LaunchAtLogin.setEnabled(newValue)
                    }
            }

            Section {
                HStack {
                    Text("Toggle Shortcut:")
                    Spacer()
                    Text("⌘ + ⇧")
                        .foregroundColor(.secondary)
                }
            }

            Section {
                Text("EndKey v1.0")
                    .font(.footnote)
                    .foregroundColor(.secondary)
            }
        }
        .formStyle(.grouped)
        .padding()
        .frame(width: 300, height: 200)
    }
}

// Extension for Picker binding
extension AppState {
    var inputMethodBinding: Binding<InputMethod> {
        Binding(
            get: { self.inputMethod },
            set: { self.inputMethod = $0 }
        )
    }
}
```

### Step 4: Setup Global Hotkey in AppDelegate
```swift
// Add to AppDelegate.swift

private var menuBarManager: MenuBarManager?

func applicationDidFinishLaunching(_ notification: Notification) {
    checkAccessibilityPermission()
}

private func setupApp() {
    // Setup keyboard
    _ = KeyboardManager.shared.setupEventTap()

    // Setup menubar
    menuBarManager = MenuBarManager()
    menuBarManager?.setup()

    // Setup global hotkey (Cmd + Shift)
    setupGlobalHotkey()
}

private func setupGlobalHotkey() {
    NSEvent.addGlobalMonitorForEvents(matching: .flagsChanged) { [weak self] event in
        // Check for Cmd + Shift pressed together
        let flags = event.modifierFlags.intersection(.deviceIndependentFlagsMask)
        if flags == [.command, .shift] {
            AppState.shared.toggleMode()
        }
    }
}
```

### Step 5: Add Icon Assets (Optional)
If text icons not preferred, create image assets:
1. Assets.xcassets → New Image Set → "MenuBarIconE"
2. Assets.xcassets → New Image Set → "MenuBarIconV"
3. Size: 18x18 @1x, 36x36 @2x
4. Template rendering for proper dark/light mode

## Todo List
- [ ] Create AppState.swift with @AppStorage
- [ ] Implement MenuBarManager with NSStatusItem
- [ ] Create ConfigView SwiftUI panel
- [ ] Add global hotkey (Cmd+Shift)
- [ ] Wire up AppDelegate initialization
- [ ] Test mode toggle from menu
- [ ] Test input method switching
- [ ] Test config panel opens/closes
- [ ] Test Cmd+Shift hotkey

## Success Criteria
- [ ] Menubar shows "E" or "V" correctly
- [ ] Clicking menu shows dropdown with options
- [ ] Cmd+Shift toggles mode (icon changes)
- [ ] Preferences panel opens and saves settings
- [ ] Input method switch works immediately

## Risk Assessment
| Risk | Likelihood | Impact | Mitigation |
|------|------------|--------|------------|
| Menu not appearing | Low | Medium | Check statusItem initialization |
| Hotkey conflict | Medium | Low | Use uncommon combination |
| Config not saving | Low | Medium | Test @AppStorage behavior |

## Security Considerations
- Settings stored in UserDefaults (local only)
- No sensitive data in preferences

## Next Steps
→ Phase 04: Polish & Distribution
