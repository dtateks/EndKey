import Cocoa

class AppDelegate: NSObject, NSApplicationDelegate {
    private var permissionRetryCount = 0
    private let maxPermissionRetries = 5
    private var menuBarManager: MenuBarManager?
    private var hotkeyMonitor: Any?

    func applicationDidFinishLaunching(_ notification: Notification) {
        checkAccessibilityPermission()
    }

    private func checkAccessibilityPermission() {
        if PermissionHelper.isAccessibilityEnabled {
            setupApp()
            return
        }

        // First attempt: show system prompt
        if permissionRetryCount == 0 {
            _ = PermissionHelper.requestAccessibility()
        }

        permissionRetryCount += 1

        if permissionRetryCount >= maxPermissionRetries {
            showPermissionAlert()
            return
        }

        // Retry after delay
        DispatchQueue.main.asyncAfter(deadline: .now() + 2) { [weak self] in
            self?.checkAccessibilityPermission()
        }
    }

    private func showPermissionAlert() {
        let alert = NSAlert()
        alert.messageText = "Accessibility Permission Required"
        alert.informativeText = "EndKey needs Accessibility access to process keyboard input. Please enable it in System Preferences."
        alert.alertStyle = .warning
        alert.addButton(withTitle: "Open Preferences")
        alert.addButton(withTitle: "Retry")
        alert.addButton(withTitle: "Quit")

        let response = alert.runModal()
        switch response {
        case .alertFirstButtonReturn:
            PermissionHelper.openAccessibilityPreferences()
            permissionRetryCount = 0
            DispatchQueue.main.asyncAfter(deadline: .now() + 3) { [weak self] in
                self?.checkAccessibilityPermission()
            }
        case .alertSecondButtonReturn:
            permissionRetryCount = 0
            checkAccessibilityPermission()
        default:
            NSApplication.shared.terminate(nil)
        }
    }

    private func setupApp() {
        // Setup keyboard manager
        if KeyboardManager.shared.setupEventTap() {
            print("KeyboardManager setup complete - Vietnamese mode active")
        } else {
            showEventTapError()
            return
        }

        // Initialize AppState (syncs with KeyboardManager)
        _ = AppState.shared

        // Setup menubar
        menuBarManager = MenuBarManager()
        menuBarManager?.setup()

        // Setup global hotkey (Cmd + Shift)
        setupGlobalHotkey()
    }

    private func setupGlobalHotkey() {
        // Track if Cmd+Shift is pressed together
        var cmdShiftPressed = false

        hotkeyMonitor = NSEvent.addGlobalMonitorForEvents(matching: .flagsChanged) { event in
            let flags = event.modifierFlags.intersection(.deviceIndependentFlagsMask)

            // Detect Cmd+Shift pressed together (no other modifiers)
            if flags == [.command, .shift] {
                if !cmdShiftPressed {
                    cmdShiftPressed = true
                    AppState.shared.toggleMode()
                }
            } else {
                cmdShiftPressed = false
            }
        }
    }

    private func showEventTapError() {
        let alert = NSAlert()
        alert.messageText = "Failed to Create Event Tap"
        alert.informativeText = "EndKey could not capture keyboard events. This may happen if Accessibility permission was revoked."
        alert.alertStyle = .critical
        alert.addButton(withTitle: "Check Permission")
        alert.addButton(withTitle: "Quit")

        let response = alert.runModal()
        if response == .alertFirstButtonReturn {
            permissionRetryCount = 0
            checkAccessibilityPermission()
        } else {
            NSApplication.shared.terminate(nil)
        }
    }

    func applicationWillTerminate(_ notification: Notification) {
        // Remove hotkey monitor
        if let monitor = hotkeyMonitor {
            NSEvent.removeMonitor(monitor)
        }
        KeyboardManager.shared.cleanup()
    }
}
