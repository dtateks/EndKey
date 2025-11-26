import Foundation

/// VNI input engine for Vietnamese typing
/// Rules: 1-5 for tones, 6 for circumflex (â,ê,ô), 7 for breve/horn (ă,ơ,ư), 9 for đ
struct VNIEngine: InputEngine {
    private(set) var buffer: [Character] = []
    private var state = SyllableState()
    private let maxBufferSize = 7

    mutating func processKey(_ char: Character) -> ProcessResult {
        // 1. Check for d9 → đ
        if char == "9" {
            if let lastChar = buffer.last, lastChar.lowercased() == "d" {
                return handleD9()
            }
        }

        // 2. Check for tone marks (1-5)
        if let digit = char.wholeNumberValue, (1...5).contains(digit) {
            return handleToneKey(char, Character("\(digit)"))
        }

        // 3. Check for vowel modifiers (6=circumflex, 7=breve/horn)
        if char == "6" || char == "7" {
            return handleVowelModifier(char)
        }

        // 4. Regular character - add to buffer
        buffer.append(char)
        state.rawInput.append(RawInputEntry(rawChar: char, outputChar: char, position: buffer.count - 1))
        state.escapeMode = false
        trimBuffer()

        return .passthrough
    }

    // MARK: - Tone Handling

    /// Handle tone key press with escape sequence support
    private mutating func handleToneKey(_ char: Character, _ toneKey: Character) -> ProcessResult {
        // Check for escape: same tone pressed twice → revert to raw (e.g., a11 → a1)
        if state.toneApplied == toneKey && !state.escapeMode {
            return handleEscapeSequence(char)
        }

        // Check for tone change: different tone pressed
        if state.toneApplied != nil && state.toneApplied != toneKey {
            return handleToneChange(toneKey)
        }

        // Apply new tone
        if let result = applyTone(toneKey) {
            state.toneApplied = toneKey
            state.escapeMode = false
            return result
        }

        // No vowel to apply tone, treat as regular number
        return addRegularChar(char)
    }

    /// Escape sequence: double tone key → raw text (a11 → a1)
    private mutating func handleEscapeSequence(_ char: Character) -> ProcessResult {
        guard let tonePos = state.lastTonePosition, tonePos < buffer.count else {
            return addRegularChar(char)
        }

        let tonedChar = buffer[tonePos]
        let baseVowel = VietnameseData.getBaseVowel(tonedChar)

        // Replace toned with base vowel
        let finalChar = tonedChar.isUppercase ? Character(baseVowel.uppercased()) : baseVowel
        buffer[tonePos] = finalChar

        state.toneApplied = nil
        state.lastTonePosition = nil
        state.escapeMode = true

        let backspaces = buffer.count - tonePos
        let replacement = String(buffer[tonePos...])

        return ProcessResult(backspaceCount: backspaces, replacement: replacement)
    }

    /// Handle tone change (e.g., a1 + 2 → à)
    private mutating func handleToneChange(_ newTone: Character) -> ProcessResult {
        guard let tonePos = state.lastTonePosition, tonePos < buffer.count else {
            return addRegularChar(Character(String(newTone)))
        }

        let currentChar = buffer[tonePos]
        let baseVowel = VietnameseData.getBaseVowel(currentChar)

        guard let toneMap = VietnameseData.toneMarks[baseVowel],
              let toned = toneMap[newTone] else {
            return addRegularChar(Character(String(newTone)))
        }

        let finalChar = currentChar.isUppercase ? Character(toned.uppercased()) : toned
        buffer[tonePos] = finalChar
        state.toneApplied = newTone

        let backspaces = buffer.count - tonePos
        let replacement = String(buffer[tonePos...])

        return ProcessResult(backspaceCount: backspaces, replacement: replacement)
    }

    // MARK: - Vowel Modifier Handling

    /// Apply vowel modifier (6 for ^, 7 for breve/horn)
    private mutating func handleVowelModifier(_ modifier: Character) -> ProcessResult {
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

        return addRegularChar(modifier)
    }

    // MARK: - D9 Handling

    private mutating func handleD9() -> ProcessResult {
        let lastChar = buffer.last!
        let isUppercase = lastChar.isUppercase
        let finalChar: Character = isUppercase ? "Đ" : "đ"
        buffer.removeLast()
        buffer.append(finalChar)
        return ProcessResult(backspaceCount: 1, replacement: String(finalChar))
    }

    // MARK: - Tone Application

    /// Apply tone mark to the most suitable vowel
    private mutating func applyTone(_ toneKey: Character) -> ProcessResult? {
        guard let vowelIndex = findTonePosition() else { return nil }

        let vowel = buffer[vowelIndex]
        let baseVowel = VietnameseData.getBaseVowel(vowel)

        guard let toneMap = VietnameseData.toneMarks[baseVowel],
              let toned = toneMap[toneKey] else { return nil }

        let finalChar = vowel.isUppercase ? Character(toned.uppercased()) : toned
        buffer[vowelIndex] = finalChar
        state.lastTonePosition = vowelIndex

        let backspaces = buffer.count - vowelIndex
        let replacement = String(buffer[vowelIndex...])

        return ProcessResult(backspaceCount: backspaces, replacement: replacement)
    }

    /// Find correct vowel position for tone (same rules as Telex)
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

        // Priority 1: Modified vowels always get the tone
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

        // Check for triphthongs and diphthongs
        let vowelString = vowelPositions.map { Character(buffer[$0].lowercased()) }
            .map { VietnameseData.getBaseVowel($0) }
            .map { String($0) }
            .joined()

        // Triphthongs: tone on middle vowel
        if vowelPositions.count >= 3 && VietnameseData.triphthongs.contains(vowelString) {
            return vowelPositions[1]
        }

        // Check 2-vowel patterns
        if vowelPositions.count >= 2 {
            let last2 = String(vowelString.suffix(2))

            if VietnameseData.diphthongsToneSecond.contains(last2) {
                return vowelPositions[vowelPositions.count - 1]
            }

            if VietnameseData.diphthongsToneFirst.contains(last2) {
                return vowelPositions[vowelPositions.count - 2]
            }
        }

        // Default: check if syllable ends with consonant
        let lastVowelIdx = vowelPositions.last!
        let hasEndingConsonant = lastVowelIdx < buffer.count - 1

        if hasEndingConsonant {
            return lastVowelIdx
        } else {
            return vowelPositions.count >= 2 ? vowelPositions[vowelPositions.count - 2] : lastVowelIdx
        }
    }

    // MARK: - Helpers

    private mutating func addRegularChar(_ char: Character) -> ProcessResult {
        buffer.append(char)
        state.rawInput.append(RawInputEntry(rawChar: char, outputChar: char, position: buffer.count - 1))
        trimBuffer()
        return .passthrough
    }

    private mutating func trimBuffer() {
        while buffer.count > maxBufferSize {
            buffer.removeFirst()
            if !state.rawInput.isEmpty {
                state.rawInput.removeFirst()
            }
        }
    }

    mutating func reset() {
        buffer.removeAll()
        state.reset()
    }
}
