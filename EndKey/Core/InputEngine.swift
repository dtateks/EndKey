import Foundation

/// Result of processing a keystroke
struct ProcessResult {
    let backspaceCount: Int
    let replacement: String?

    /// No transformation needed, pass through original character
    static let passthrough = ProcessResult(backspaceCount: 0, replacement: nil)
}

/// Protocol for Vietnamese input engines (Telex, VNI)
protocol InputEngine {
    /// Process incoming character, return transformation result
    mutating func processKey(_ char: Character) -> ProcessResult

    /// Reset engine state (call on word boundaries: space, enter, punctuation)
    mutating func reset()

    /// Current buffer content for debugging
    var buffer: [Character] { get }
}

/// Input method type enum
enum InputMethod: String, CaseIterable {
    case telex = "Telex"
    case vni = "VNI"

    var displayName: String { rawValue }
}
