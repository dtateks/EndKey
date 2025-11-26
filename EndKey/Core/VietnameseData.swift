import Foundation

/// Vietnamese character data and mappings for input processing
struct VietnameseData {
    // Base vowels that can receive diacritics
    static let baseVowels: Set<Character> = ["a", "e", "i", "o", "u", "y"]
    static let modifiedVowels: Set<Character> = ["ă", "â", "ê", "ô", "ơ", "ư"]

    // Tone marks mapping: base vowel -> (tone key -> toned character)
    // Tone keys: 1=sắc, 2=huyền, 3=hỏi, 4=ngã, 5=nặng
    static let toneMarks: [Character: [Character: Character]] = [
        "a": ["1": "á", "2": "à", "3": "ả", "4": "ã", "5": "ạ"],
        "ă": ["1": "ắ", "2": "ằ", "3": "ẳ", "4": "ẵ", "5": "ặ"],
        "â": ["1": "ấ", "2": "ầ", "3": "ẩ", "4": "ẫ", "5": "ậ"],
        "e": ["1": "é", "2": "è", "3": "ẻ", "4": "ẽ", "5": "ẹ"],
        "ê": ["1": "ế", "2": "ề", "3": "ể", "4": "ễ", "5": "ệ"],
        "i": ["1": "í", "2": "ì", "3": "ỉ", "4": "ĩ", "5": "ị"],
        "o": ["1": "ó", "2": "ò", "3": "ỏ", "4": "õ", "5": "ọ"],
        "ô": ["1": "ố", "2": "ồ", "3": "ổ", "4": "ỗ", "5": "ộ"],
        "ơ": ["1": "ớ", "2": "ờ", "3": "ở", "4": "ỡ", "5": "ợ"],
        "u": ["1": "ú", "2": "ù", "3": "ủ", "4": "ũ", "5": "ụ"],
        "ư": ["1": "ứ", "2": "ừ", "3": "ử", "4": "ữ", "5": "ự"],
        "y": ["1": "ý", "2": "ỳ", "3": "ỷ", "4": "ỹ", "5": "ỵ"]
    ]

    // Telex: letter keys for tones
    static let telexTones: [Character: Character] = [
        "s": "1", // sắc
        "f": "2", // huyền
        "r": "3", // hỏi
        "x": "4", // ngã
        "j": "5"  // nặng
    ]

    // Vowel transformations (Telex + VNI)
    static let vowelTransforms: [String: Character] = [
        // Telex: doubling
        "aa": "â", "ee": "ê", "oo": "ô",
        "aw": "ă", "ow": "ơ", "uw": "ư",
        // VNI: number modifiers
        "a6": "â", "e6": "ê", "o6": "ô",
        "a7": "ă", "o7": "ơ", "u7": "ư",
        "a8": "ă", "o8": "ơ", "u8": "ư" // Some VNI variants use 8
    ]

    // Reverse mapping: toned vowel -> base vowel (for tone replacement)
    static let tonedToBase: [Character: Character] = [
        "á": "a", "à": "a", "ả": "a", "ã": "a", "ạ": "a",
        "ắ": "ă", "ằ": "ă", "ẳ": "ă", "ẵ": "ă", "ặ": "ă",
        "ấ": "â", "ầ": "â", "ẩ": "â", "ẫ": "â", "ậ": "â",
        "é": "e", "è": "e", "ẻ": "e", "ẽ": "e", "ẹ": "e",
        "ế": "ê", "ề": "ê", "ể": "ê", "ễ": "ê", "ệ": "ê",
        "í": "i", "ì": "i", "ỉ": "i", "ĩ": "i", "ị": "i",
        "ó": "o", "ò": "o", "ỏ": "o", "õ": "o", "ọ": "o",
        "ố": "ô", "ồ": "ô", "ổ": "ô", "ỗ": "ô", "ộ": "ô",
        "ớ": "ơ", "ờ": "ơ", "ở": "ơ", "ỡ": "ơ", "ợ": "ơ",
        "ú": "u", "ù": "u", "ủ": "u", "ũ": "u", "ụ": "u",
        "ứ": "ư", "ừ": "ư", "ử": "ư", "ữ": "ư", "ự": "ư",
        "ý": "y", "ỳ": "y", "ỷ": "y", "ỹ": "y", "ỵ": "y"
    ]

    /// Get base vowel from a potentially toned character
    static func getBaseVowel(_ char: Character) -> Character {
        let lower = Character(char.lowercased())
        return tonedToBase[lower] ?? lower
    }

    /// Check if character is a Vietnamese vowel (base or modified)
    static func isVowel(_ char: Character) -> Bool {
        let lower = Character(char.lowercased())
        return baseVowels.contains(lower) || modifiedVowels.contains(lower) || tonedToBase[lower] != nil
    }
}
