import SwiftUI
import Combine

/// Shared application state with UserDefaults persistence
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
        // Sync initial state to KeyboardManager
        KeyboardManager.shared.setVietnameseMode(isVietnameseMode)
        if let method = InputMethod(rawValue: inputMethodRaw) {
            KeyboardManager.shared.inputMethod = method
        }
    }
}

extension Notification.Name {
    static let modeChanged = Notification.Name("modeChanged")
}
