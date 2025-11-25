import Cocoa
import Carbon

class AppDelegate: NSObject, NSApplicationDelegate {
    private var eventTap: CFMachPort?
    private var permissionRetryCount = 0
    private let maxPermissionRetries = 5

    func applicationDidFinishLaunching(_ notification: Notification) {
        checkAccessibilityPermission()
    }

    private func checkAccessibilityPermission() {
        if PermissionHelper.isAccessibilityEnabled {
            setupEventTap()
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

    private func setupEventTap() {
        let eventMask = (1 << CGEventType.keyDown.rawValue)

        eventTap = CGEvent.tapCreate(
            tap: .cgSessionEventTap,
            place: .headInsertEventTap,
            options: .defaultTap,
            eventsOfInterest: CGEventMask(eventMask),
            callback: { (proxy, type, event, refcon) -> Unmanaged<CGEvent>? in
                // Skeleton - will be implemented in Phase 2
                return Unmanaged.passUnretained(event)
            },
            userInfo: nil
        )

        guard let eventTap = eventTap else {
            showEventTapError()
            return
        }

        let runLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, eventTap, 0)
        CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, .commonModes)
        CGEvent.tapEnable(tap: eventTap, enable: true)

        print("EventTap setup complete")
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
        if let eventTap = eventTap {
            CGEvent.tapEnable(tap: eventTap, enable: false)
        }
    }
}
