import SwiftUI
import ServiceManagement

/// Preferences panel for EndKey configuration
struct ConfigView: View {
    @ObservedObject private var appState = AppState.shared

    var body: some View {
        VStack(alignment: .leading, spacing: 16) {
            // Input Method Section
            GroupBox(label: Text("Input Method").font(.headline)) {
                Picker("", selection: Binding(
                    get: { appState.inputMethod },
                    set: { appState.inputMethod = $0 }
                )) {
                    ForEach(InputMethod.allCases, id: \.self) { method in
                        Text(method.rawValue).tag(method)
                    }
                }
                .pickerStyle(.segmented)
                .labelsHidden()
            }

            // Settings Section
            GroupBox(label: Text("Settings").font(.headline)) {
                Toggle("Launch at Login", isOn: Binding(
                    get: { appState.launchAtLogin },
                    set: { newValue in
                        appState.launchAtLogin = newValue
                        LaunchAtLoginHelper.setEnabled(newValue)
                    }
                ))
            }

            // Shortcut Section
            GroupBox(label: Text("Shortcut").font(.headline)) {
                HStack {
                    Text("Toggle Vietnamese/English:")
                    Spacer()
                    Text("⌘ + ⇧")
                        .foregroundColor(.secondary)
                        .padding(.horizontal, 8)
                        .padding(.vertical, 4)
                        .background(Color.gray.opacity(0.2))
                        .cornerRadius(4)
                }
            }

            Spacer()

            // Footer
            HStack {
                Spacer()
                Text("EndKey v1.0")
                    .font(.footnote)
                    .foregroundColor(.secondary)
                Spacer()
            }
        }
        .padding()
        .frame(width: 300, height: 220)
    }
}

/// Helper for managing launch at login functionality
enum LaunchAtLoginHelper {
    static func setEnabled(_ enabled: Bool) {
        if #available(macOS 13.0, *) {
            do {
                if enabled {
                    try SMAppService.mainApp.register()
                } else {
                    try SMAppService.mainApp.unregister()
                }
            } catch {
                print("Launch at login error: \(error)")
            }
        } else {
            // Fallback for older macOS versions - uses deprecated API
            SMLoginItemSetEnabled("com.endkey.EndKey" as CFString, enabled)
        }
    }
}
