import Cocoa
import SwiftUI

/// Manages the menu bar status item and dropdown menu
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
            selector: #selector(handleModeChange),
            name: .modeChanged,
            object: nil
        )
    }

    @objc private func handleModeChange() {
        updateIcon()
        setupMenu()
    }

    @objc func updateIcon() {
        guard let button = statusItem?.button else { return }

        let icon = AppState.shared.isVietnameseMode ? "V" : "E"
        button.title = icon
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
    }

    @objc private func selectInputMethod(_ sender: NSMenuItem) {
        guard let method = sender.representedObject as? InputMethod else { return }
        AppState.shared.inputMethod = method
        setupMenu()
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

    deinit {
        NotificationCenter.default.removeObserver(self)
    }
}
