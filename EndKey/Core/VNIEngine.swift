import Foundation

/// VNI input engine for Vietnamese typing
/// Rules: 1-5 for tones, 6 for circumflex (â,ê,ô), 7 for breve/horn (ă,ơ,ư), 9 for đ
struct VNIEngine: InputEngine {
    private(set) var buffer: [Character] = []
    private let maxBufferSize = 7

    mutating func processKey(_ char: Character) -> ProcessResult {
        // 1. Check for d9 → đ
        if char == "9" {
            if let lastChar = buffer.last, lastChar.lowercased() == "d" {
                let isUppercase = lastChar.isUppercase
                let finalChar: Character = isUppercase ? "Đ" : "đ"
                buffer.removeLast()
                buffer.append(finalChar)
                return ProcessResult(backspaceCount: 1, replacement: String(finalChar))
            }
        }

        // 2. Check for tone marks (1-5)
        if let digit = char.wholeNumberValue, (1...5).contains(digit) {
            if let result = applyTone(Character("\(digit)")) {
                return result
            }
            // No vowel found, treat as regular number
        }

        // 3. Check for vowel modifiers (6=circumflex, 7=breve/horn)
        if char == "6" || char == "7" {
            if let result = applyVowelModifier(char) {
                return result
            }
            // No applicable vowel, treat as regular number
        }

        // 4. Regular character - add to buffer
        buffer.append(char)
        trimBuffer()

        return .passthrough
    }

    /// Apply tone mark to the most suitable vowel
    private mutating func applyTone(_ toneKey: Character) -> ProcessResult? {
        guard let vowelIndex = findTonePosition() else { return nil }

        let vowel = buffer[vowelIndex]
        let baseVowel = VietnameseData.getBaseVowel(vowel)

        guard let toneMap = VietnameseData.toneMarks[baseVowel],
              let toned = toneMap[toneKey] else { return nil }

        let finalChar = vowel.isUppercase ? Character(toned.uppercased()) : toned
        buffer[vowelIndex] = finalChar

        let backspaces = buffer.count - vowelIndex
        let replacement = String(buffer[vowelIndex...])

        return ProcessResult(backspaceCount: backspaces, replacement: replacement)
    }

    /// Apply vowel modifier (6 for ^, 7 for breve/horn)
    private mutating func applyVowelModifier(_ modifier: Character) -> ProcessResult? {
        // Find last applicable vowel
        for i in stride(from: buffer.count - 1, through: 0, by: -1) {
            let vowel = buffer[i]
            let baseVowel = VietnameseData.getBaseVowel(vowel)

            let combo = String([baseVowel, modifier])
            if let transformed = VietnameseData.vowelTransforms[combo] {
                let finalChar = vowel.isUppercase ? Character(transformed.uppercased()) : transformed
                buffer[i] = finalChar

                let backspaces = buffer.count - i
                let replacement = String(buffer[i...])

                return ProcessResult(backspaceCount: backspaces, replacement: replacement)
            }
        }

        return nil
    }

    /// Find correct vowel position for tone
    /// Vietnamese rules: same as TelexEngine
    private func findTonePosition() -> Int? {
        var vowelPositions: [Int] = []
        for i in 0..<buffer.count {
            if VietnameseData.isVowel(buffer[i]) {
                vowelPositions.append(i)
            }
        }

        guard !vowelPositions.isEmpty else { return nil }

        if vowelPositions.count == 1 {
            return vowelPositions[0]
        }

        // Priority 1: Modified vowels (ă, â, ê, ô, ơ, ư) always get the tone
        for pos in vowelPositions {
            let lower = Character(buffer[pos].lowercased())
            if VietnameseData.modifiedVowels.contains(lower) {
                return pos
            }
            if let base = VietnameseData.tonedToBase[lower],
               VietnameseData.modifiedVowels.contains(base) {
                return pos
            }
        }

        // Check if syllable ends with consonant
        let lastVowelIdx = vowelPositions.last!
        let hasEndingConsonant = lastVowelIdx < buffer.count - 1

        if hasEndingConsonant {
            return lastVowelIdx
        } else {
            return vowelPositions.count >= 2 ? vowelPositions[vowelPositions.count - 2] : lastVowelIdx
        }
    }

    private mutating func trimBuffer() {
        while buffer.count > maxBufferSize {
            buffer.removeFirst()
        }
    }

    mutating func reset() {
        buffer.removeAll()
    }
}
