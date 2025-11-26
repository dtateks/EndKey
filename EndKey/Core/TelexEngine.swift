import Foundation

/// Telex input engine for Vietnamese typing
/// Rules: aa→â, aw→ă, dd→đ, s/f/r/x/j for tones, z for undo
struct TelexEngine: InputEngine {
    private(set) var buffer: [Character] = []
    private var state = SyllableState()
    private let maxBufferSize = 7

    mutating func processKey(_ char: Character) -> ProcessResult {
        let lowerChar = Character(char.lowercased())

        // 1. Handle z key - undo last transformation
        if lowerChar == "z" {
            return handleZUndo()
        }

        // 2. Check for tone marks (s, f, r, x, j)
        if VietnameseData.telexToneKeys.contains(lowerChar) {
            return handleToneKey(char, lowerChar)
        }

        // 3. Check for dd → đ
        if lowerChar == "d" {
            if let lastChar = buffer.last, lastChar.lowercased() == "d" {
                return handleDoubleD(char)
            }
        }

        // 4. Check for multi-char transforms (uow → ươ)
        if let result = checkMultiCharTransform(char) {
            return result
        }

        // 5. Check for vowel transformations (aa, aw, ow, uw, ee, oo)
        if let result = checkVowelTransform(char, lowerChar) {
            return result
        }

        // 6. Regular character - add to buffer
        buffer.append(char)
        state.rawInput.append(RawInputEntry(rawChar: char, outputChar: char, position: buffer.count - 1))
        state.escapeMode = false
        trimBuffer()

        return .passthrough
    }

    // MARK: - Tone Handling

    /// Handle tone key press with escape sequence support
    private mutating func handleToneKey(_ char: Character, _ lowerChar: Character) -> ProcessResult {
        // Check for escape: same tone pressed twice → revert to raw
        if state.toneApplied == lowerChar && !state.escapeMode {
            return handleEscapeSequence(char)
        }

        // Check for tone change: different tone pressed
        if state.toneApplied != nil && state.toneApplied != lowerChar {
            return handleToneChange(lowerChar)
        }

        // Apply new tone
        guard let toneKey = VietnameseData.telexTones[lowerChar] else {
            return addRegularChar(char)
        }

        if let result = applyTone(toneKey) {
            state.toneApplied = lowerChar
            state.escapeMode = false
            return result
        }

        // No vowel to apply tone, treat as regular char
        return addRegularChar(char)
    }

    /// Escape sequence: double tone key → raw text
    private mutating func handleEscapeSequence(_ char: Character) -> ProcessResult {
        guard let tonePos = state.lastTonePosition, tonePos < buffer.count else {
            return addRegularChar(char)
        }

        // Find the original raw character at tone position
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

    /// Handle tone change (e.g., as + f → à)
    private mutating func handleToneChange(_ newTone: Character) -> ProcessResult {
        guard let tonePos = state.lastTonePosition,
              tonePos < buffer.count,
              let toneKey = VietnameseData.telexTones[newTone] else {
            return addRegularChar(newTone)
        }

        let currentChar = buffer[tonePos]
        let baseVowel = VietnameseData.getBaseVowel(currentChar)

        guard let toneMap = VietnameseData.toneMarks[baseVowel],
              let toned = toneMap[toneKey] else {
            return addRegularChar(newTone)
        }

        let finalChar = currentChar.isUppercase ? Character(toned.uppercased()) : toned
        buffer[tonePos] = finalChar
        state.toneApplied = newTone

        let backspaces = buffer.count - tonePos
        let replacement = String(buffer[tonePos...])

        return ProcessResult(backspaceCount: backspaces, replacement: replacement)
    }

    // MARK: - Z Undo

    /// Z key removes last transformation
    private mutating func handleZUndo() -> ProcessResult {
        // If tone was applied, remove it
        if let tonePos = state.lastTonePosition, tonePos < buffer.count {
            let tonedChar = buffer[tonePos]
            let baseVowel = VietnameseData.getBaseVowel(tonedChar)
            let finalChar = tonedChar.isUppercase ? Character(baseVowel.uppercased()) : baseVowel
            buffer[tonePos] = finalChar

            state.toneApplied = nil
            state.lastTonePosition = nil

            let backspaces = buffer.count - tonePos
            let replacement = String(buffer[tonePos...])

            return ProcessResult(backspaceCount: backspaces, replacement: replacement)
        }

        // Check for modified vowel to revert (â→a, ư→u, etc)
        if let (pos, base) = findLastModifiedVowel() {
            let vowel = buffer[pos]
            let finalChar = vowel.isUppercase ? Character(base.uppercased()) : base
            buffer[pos] = finalChar

            let backspaces = buffer.count - pos
            let replacement = String(buffer[pos...])

            return ProcessResult(backspaceCount: backspaces, replacement: replacement)
        }

        // No transformation to undo
        buffer.append("z")
        trimBuffer()
        return .passthrough
    }

    /// Find last modified vowel (ă, â, ê, ô, ơ, ư) and its base
    private func findLastModifiedVowel() -> (Int, Character)? {
        let modifiedToBase: [Character: Character] = [
            "ă": "a", "â": "a", "ê": "e", "ô": "o", "ơ": "o", "ư": "u"
        ]

        for i in stride(from: buffer.count - 1, through: 0, by: -1) {
            let lower = Character(buffer[i].lowercased())
            if let base = modifiedToBase[lower] {
                return (i, base)
            }
        }
        return nil
    }

    // MARK: - Vowel Transformations

    /// Check for multi-char transforms like uow → ươ
    private mutating func checkMultiCharTransform(_ char: Character) -> ProcessResult? {
        guard buffer.count >= 2 else { return nil }

        let lowerChar = Character(char.lowercased())
        let last2 = String([buffer[buffer.count - 2], buffer[buffer.count - 1]]).lowercased()
        let combo = last2 + String(lowerChar)

        if let transformed = VietnameseData.multiCharTransforms[combo] {
            // Preserve case of first character
            let isFirstUpper = buffer[buffer.count - 2].isUppercase
            let chars = Array(transformed)
            let finalFirst = isFirstUpper ? Character(chars[0].uppercased()) : chars[0]
            let finalSecond = chars[1]

            buffer.removeLast(2)
            buffer.append(finalFirst)
            buffer.append(finalSecond)

            return ProcessResult(backspaceCount: 2, replacement: String([finalFirst, finalSecond]))
        }

        return nil
    }

    /// Check for vowel transformations (aa, aw, ow, uw, ee, oo)
    private mutating func checkVowelTransform(_ char: Character, _ lowerChar: Character) -> ProcessResult? {
        guard let lastChar = buffer.last else { return nil }

        let lastLower = Character(lastChar.lowercased())
        let combo = String([lastLower, lowerChar])

        if let transformed = VietnameseData.vowelTransforms[combo] {
            let isUppercase = lastChar.isUppercase
            let finalChar = isUppercase ? Character(transformed.uppercased()) : transformed
            buffer.removeLast()
            buffer.append(finalChar)
            return ProcessResult(backspaceCount: 1, replacement: String(finalChar))
        }

        return nil
    }

    // MARK: - Double D

    private mutating func handleDoubleD(_ char: Character) -> ProcessResult {
        let lastChar = buffer.last!
        let isUppercase = char.isUppercase || lastChar.isUppercase
        let finalChar: Character = isUppercase ? "Đ" : "đ"
        buffer.removeLast()
        buffer.append(finalChar)
        return ProcessResult(backspaceCount: 1, replacement: String(finalChar))
    }

    // MARK: - Tone Application

    /// Apply tone to the most suitable vowel in buffer
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

    /// Find the correct vowel position for tone placement
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

            // Diphthongs with tone on second vowel
            if VietnameseData.diphthongsToneSecond.contains(last2) {
                return vowelPositions[vowelPositions.count - 1]
            }

            // Diphthongs with tone on first vowel
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
