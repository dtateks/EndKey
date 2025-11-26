import Foundation

/// Telex input engine for Vietnamese typing
/// Rules: aa→â, aw→ă, dd→đ, s/f/r/x/j for tones
struct TelexEngine: InputEngine {
    private(set) var buffer: [Character] = []
    private let maxBufferSize = 7 // Enough for longest Vietnamese syllable

    mutating func processKey(_ char: Character) -> ProcessResult {
        let lowerChar = Character(char.lowercased())

        // 1. Check for tone marks (s, f, r, x, j)
        if let toneKey = VietnameseData.telexTones[lowerChar] {
            if let result = applyTone(toneKey) {
                return result
            }
            // If no vowel to apply tone, treat as regular char
        }

        // 2. Check for dd → đ
        if lowerChar == "d" {
            if let lastChar = buffer.last, lastChar.lowercased() == "d" {
                let isUppercase = char.isUppercase || lastChar.isUppercase
                let finalChar: Character = isUppercase ? "Đ" : "đ"
                buffer.removeLast()
                buffer.append(finalChar)
                return ProcessResult(backspaceCount: 1, replacement: String(finalChar))
            }
        }

        // 3. Check for vowel transformations (aa, aw, ow, uw, ee, oo)
        if let lastChar = buffer.last {
            let lastLower = Character(lastChar.lowercased())
            let combo = String([lastLower, lowerChar])

            if let transformed = VietnameseData.vowelTransforms[combo] {
                let isUppercase = lastChar.isUppercase
                let finalChar = isUppercase ? Character(transformed.uppercased()) : transformed
                buffer.removeLast()
                buffer.append(finalChar)
                return ProcessResult(backspaceCount: 1, replacement: String(finalChar))
            }
        }

        // 4. Regular character - add to buffer
        buffer.append(char)
        trimBuffer()

        return .passthrough
    }

    /// Apply tone to the most suitable vowel in buffer
    private mutating func applyTone(_ toneKey: Character) -> ProcessResult? {
        // Find best vowel position using Vietnamese tone placement rules
        guard let vowelIndex = findTonePosition() else { return nil }

        let vowel = buffer[vowelIndex]
        let baseVowel = VietnameseData.getBaseVowel(vowel)

        guard let toneMap = VietnameseData.toneMarks[baseVowel],
              let toned = toneMap[toneKey] else { return nil }

        let finalChar = vowel.isUppercase ? Character(toned.uppercased()) : toned
        buffer[vowelIndex] = finalChar

        // Calculate backspaces needed and build replacement string
        let backspaces = buffer.count - vowelIndex
        let replacement = String(buffer[vowelIndex...])

        return ProcessResult(backspaceCount: backspaces, replacement: replacement)
    }

    /// Find the correct vowel position for tone placement
    /// Vietnamese rules:
    /// 1. Modified vowels (ă, â, ê, ô, ơ, ư) always get the tone
    /// 2. If syllable ends with consonant → tone on last vowel
    /// 3. If no ending consonant and 2+ vowels → tone on penultimate vowel
    private func findTonePosition() -> Int? {
        var vowelPositions: [Int] = []
        for i in 0..<buffer.count {
            if VietnameseData.isVowel(buffer[i]) {
                vowelPositions.append(i)
            }
        }

        guard !vowelPositions.isEmpty else { return nil }

        // Single vowel: easy case
        if vowelPositions.count == 1 {
            return vowelPositions[0]
        }

        // Priority 1: Modified vowels (ă, â, ê, ô, ơ, ư) always get the tone
        for pos in vowelPositions {
            let lower = Character(buffer[pos].lowercased())
            if VietnameseData.modifiedVowels.contains(lower) {
                return pos
            }
            // Check if already toned from a modified base
            if let base = VietnameseData.tonedToBase[lower],
               VietnameseData.modifiedVowels.contains(base) {
                return pos
            }
        }

        // Check if syllable ends with consonant (after last vowel)
        let lastVowelIdx = vowelPositions.last!
        let hasEndingConsonant = lastVowelIdx < buffer.count - 1

        if hasEndingConsonant {
            // Ends with consonant → tone on last vowel
            return lastVowelIdx
        } else {
            // No ending consonant → tone on penultimate vowel (or last if only one)
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
