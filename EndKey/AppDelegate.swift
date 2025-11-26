import Cocoa

class AppDelegate: NSObject, NSApplicationDelegate {
    private var permissionRetryCount = 0
    private let maxPermissionRetries = 5

    func applicationDidFinishLaunching(_ notification: Notification) {
        checkAccessibilityPermission()
    }

    private func checkAccessibilityPermission() {
        if PermissionHelper.isAccessibilityEnabled {
            setupKeyboardManager()
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

    private func setupKeyboardManager() {
        if KeyboardManager.shared.setupEventTap() {
            print("KeyboardManager setup complete - Vietnamese mode active")
        } else {
            showEventTapError()
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
        KeyboardManager.shared.cleanup()
    }
}
