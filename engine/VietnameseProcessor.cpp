//
//  VietnameseProcessor.cpp
//  EndKey
//
//  Created by EndKey Refactoring on 2025.
//  Copyright © 2025 Tuyen Mai. All rights reserved.
//

#include "VietnameseProcessor.h"
#include <algorithm>
#include <stdexcept>

// VietnameseProcessor::Impl structure with RAII pattern
struct VietnameseProcessor::Impl {
    // Configuration
    bool modernOrthographyEnabled;

    // Character processing tables
    std::vector<std::vector<Uint32>> vowelTable;
    std::vector<std::vector<Uint32>> consonantTable;
    std::vector<std::vector<Uint32>> toneTable;
    std::vector<std::vector<Uint32>> markTable;

    // Conversion tables
    std::unordered_map<Uint32, Uint32> unicodeToTcvn3Map;
    std::unordered_map<Uint32, Uint32> unicodeToVniMap;
    std::unordered_map<Uint32, Uint32> tcvn3ToUnicodeMap;
    std::unordered_map<Uint32, Uint32> vniToUnicodeMap;

    // Performance optimization
    std::unordered_map<std::string, bool> cacheValidWords;
    std::unordered_map<Uint32, Uint32> cacheCharacterConversion;

    // Constructor with RAII initialization
    Impl() : modernOrthographyEnabled(true) {
        try {
            initializeCharacterTables();
            initializeConversionTables();
            initializeQuickConsonantRules();
        } catch (const std::exception& e) {
            throw std::runtime_error("Failed to initialize VietnameseProcessor: " + std::string(e.what()));
        }
    }

    // Destructor - automatic cleanup via STL containers
    ~Impl() {
        // STL containers automatically clean up
    }

    // Initialize character processing tables
    void initializeCharacterTables() {
        // Initialize vowel table (simplified version)
        vowelTable = {
            {L'a', L'à', L'á', L'ả', L'ã', L'ạ', L'ă', L'ằ', L'ắ', L'ẳ', L'ẵ', L'ặ', L'â', L'ầ', L'ấ', L'ẩ', L'ẫ', L'ậ'},
            {L'e', L'è', L'é', L'ẻ', L'ẽ', L'ẹ', L'ê', L'ề', L'ế', L'ể', L'ễ', L'ệ'},
            {L'i', L'ì', L'í', L'ỉ', L'ĩ', L'ị'},
            {L'o', L'ò', L'ó', L'ỏ', L'õ', L'ọ', L'ô', L'ồ', L'ố', L'ổ', L'ỗ', L'ộ', L'ơ', L'ờ', L'ớ', L'ở', L'ỡ', L'ợ'},
            {L'u', L'ù', L'ú', L'ủ', L'ũ', L'ụ', L'ư', L'ừ', L'ứ', L'ử', L'ữ', L'ự'},
            {L'y', L'ỳ', L'ý', L'ỷ', L'ỹ', L'ỵ'},
            {L'A', L'À', L'Á', L'Ả', L'Ã', L'Ạ', L'Ă', L'Ằ', L'Ắ', L'Ẳ', L'Ẵ', L'Ặ', L'Â', L'Ầ', L'Ấ', L'Ẩ', L'Ẫ', L'Ậ'},
            {L'E', L'È', L'É', L'Ẻ', L'Ẽ', L'Ẹ', L'Ê', L'Ề', L'Ế', L'Ể', L'Ễ', L'Ệ'},
            {L'I', L'Ì', L'Í', L'Ỉ', L'Ĩ', L'Ị'},
            {L'O', L'Ò', L'Ó', L'Ỏ', L'Õ', L'Ọ', L'Ô', L'Ồ', L'Ố', L'Ổ', L'Ỗ', L'Ộ', L'Ơ', L'Ờ', L'Ớ', L'Ở', L'Ỡ', L'Ợ'},
            {L'U', L'Ù', L'Ú', L'Ủ', L'Ũ', L'Ụ', L'Ư', L'Ừ', L'Ứ', L'Ử', L'Ữ', L'Ự'},
            {L'Y', L'Ỳ', L'Ý', L'Ỷ', L'Ỹ', L'Ỵ'}
        };

        // Initialize consonant table
        consonantTable = {
            {L'b', L'c', L'd', L'đ', L'g', L'h', L'k', L'l', L'm', L'n', L'n', L'p', L'q', L'r', L's', L't', L't', L'v', L'x'},
            {L'B', L'C', L'D', L'Đ', L'G', L'H', L'K', L'L', L'M', L'N', L'N', L'P', L'Q', L'R', L'S', L'T', L'T', L'V', L'X'}
        };
    }

    // Initialize conversion tables
    void initializeConversionTables() {
        // Unicode to TCVN3 conversion (simplified)
        unicodeToTcvn3Map = {
            {L'à', 192}, {L'á', 193}, {L'ả', 194}, {L'ã', 195}, {L'ạ', 196},
            {L'À', 200}, {L'Á', 201}, {L'Ả', 202}, {L'Ã', 203}, {L'Ạ', 204},
            // Add more mappings as needed
        };

        // Unicode to VNI conversion (simplified)
        unicodeToVniMap = {
            {L'à', 181}, {L'á', 182}, {L'ả', 183}, {L'ã', 184}, {L'ạ', 185},
            {L'À', 186}, {L'Á', 187}, {L'Ả', 188}, {L'Ã', 189}, {L'Ạ', 190},
            // Add more mappings as needed
        };

        // Create reverse mappings
        for (const auto& pair : unicodeToTcvn3Map) {
            tcvn3ToUnicodeMap[pair.second] = pair.first;
        }
        for (const auto& pair : unicodeToVniMap) {
            vniToUnicodeMap[pair.second] = pair.first;
        }
    }

    // Initialize quick consonant replacement rules
    void initializeQuickConsonantRules() {
        // Quick start consonant replacements
        // f -> ph, j -> gi, w -> qu
        // Quick end consonant replacements
        // g -> ng, h -> nh, k -> ch
    }

    // Helper methods
    bool isVowelCharacter(Uint32 character) {
        for (const auto& vowelRow : vowelTable) {
            if (std::find(vowelRow.begin(), vowelRow.end(), character) != vowelRow.end()) {
                return true;
            }
        }
        return false;
    }

    bool isConsonant(Uint32 character) {
        for (const auto& consonantRow : consonantTable) {
            if (std::find(consonantRow.begin(), consonantRow.end(), character) != consonantRow.end()) {
                return true;
            }
        }
        return false;
    }

    Uint32 applyToneMark(Uint32 character, Uint8 toneType) {
        if (!isVowelCharacter(character)) {
            return character;
        }

        // Find the vowel row containing this character
        for (auto& vowelRow : vowelTable) {
            auto it = std::find(vowelRow.begin(), vowelRow.end(), character);
            if (it != vowelRow.end()) {
                size_t baseIndex = 0;

                // Find the base character (no tone)
                if (it - vowelRow.begin() < 6) {
                    baseIndex = 0;
                } else if (it - vowelRow.begin() < 12) {
                    baseIndex = 6;
                } else if (it - vowelRow.begin() < 18) {
                    baseIndex = 12;
                }

                // Apply the new tone
                if (baseIndex + toneType < vowelRow.size()) {
                    return vowelRow[baseIndex + toneType];
                }
                break;
            }
        }

        return character;
    }

    Uint32 getToneType(Uint32 character) {
        if (!isVowelCharacter(character)) {
            return 0;
        }

        for (const auto& vowelRow : vowelTable) {
            auto it = std::find(vowelRow.begin(), vowelRow.end(), character);
            if (it != vowelRow.end()) {
                size_t index = it - vowelRow.begin();
                return index % 6;
            }
        }

        return 0;
    }

    bool isValidVietnameseWord(const std::vector<Uint32>& word) {
        if (word.empty()) return false;

        std::wstring wstr(word.begin(), word.end());

        // Check cache first
        auto cacheIt = cacheValidWords.find(std::string(wstr.begin(), wstr.end()));
        if (cacheIt != cacheValidWords.end()) {
            return cacheIt->second;
        }

        // Basic validation: must contain at least one vowel
        bool hasVowel = false;
        for (Uint32 charCode : word) {
            if (isVowelCharacter(charCode)) {
                hasVowel = true;
                break;
            }
        }

        bool isValid = hasVowel && !hasInvalidConsonantSequences(word);

        // Cache the result
        cacheValidWords[std::string(wstr.begin(), wstr.end())] = isValid;

        return isValid;
    }

    bool hasInvalidConsonantSequences(const std::vector<Uint32>& word) {
        // Check for invalid consonant sequences
        // This is a simplified implementation
        return false;
    }
};

// VietnameseProcessor public implementation
VietnameseProcessor::VietnameseProcessor() : pImpl(std::make_unique<Impl>()) {
}

VietnameseProcessor::~VietnameseProcessor() = default;

bool VietnameseProcessor::processToneMark(Uint32& character, Uint8 toneKey, int inputType) {
    if (!pImpl) return false;

    // Map input key to tone type based on input method
    Uint8 toneType = 0;
    switch (inputType) {
        case 0: // Telex
            switch (toneKey) {
                case KEY_S: toneType = 1; break; // Sắc
                case KEY_F: toneType = 2; break; // Huyền
                case KEY_R: toneType = 3; break; // Hỏi
                case KEY_X: toneType = 4; break; // Ngã
                case KEY_J: toneType = 5; break; // Nặng
                default: return false;
            }
            break;
        case 1: // VNI
            if (toneKey >= KEY_1 && toneKey <= KEY_5) {
                toneType = toneKey - KEY_1 + 1;
            } else {
                return false;
            }
            break;
        default:
            return false;
    }

    character = pImpl->applyToneMark(character, toneType);
    return true;
}

bool VietnameseProcessor::processCharacterMark(Uint32& character, Uint8 markKey, int inputType) {
    if (!pImpl) return false;

    // Implement character mark processing (circumflex, breve, horn, etc.)
    // This is a simplified implementation
    return true;
}

bool VietnameseProcessor::isVowelCharacter(Uint32 character) {
    return pImpl && pImpl->isVowelCharacter(character);
}

bool VietnameseProcessor::isValidVietnameseSequence(const std::vector<Uint32>& word) {
    return pImpl && pImpl->isValidVietnameseWord(word);
}

void VietnameseProcessor::setModernOrthography(bool enabled) {
    if (pImpl) {
        pImpl->modernOrthographyEnabled = enabled;
    }
}

bool VietnameseProcessor::getModernOrthography() const {
    return pImpl ? pImpl->modernOrthographyEnabled : false;
}

Uint32 VietnameseProcessor::convertCharacterEncoding(Uint32 character, int fromCodeTable, int toCodeTable) {
    if (!pImpl) return character;

    // Simple conversion: Unicode -> Unicode (no change)
    if (fromCodeTable == 0 && toCodeTable == 0) {
        return character;
    }

    // Check cache first
    std::string cacheKey = std::to_string(character) + "_" + std::to_string(fromCodeTable) + "_" + std::to_string(toCodeTable);
    auto cacheIt = pImpl->cacheCharacterConversion.find(std::hash<std::string>{}(cacheKey));
    if (cacheIt != pImpl->cacheCharacterConversion.end()) {
        return cacheIt->second;
    }

    Uint32 result = character;

    // Unicode to TCVN3
    if (fromCodeTable == 0 && toCodeTable == 1) {
        auto it = pImpl->unicodeToTcvn3Map.find(character);
        if (it != pImpl->unicodeToTcvn3Map.end()) {
            result = it->second;
        }
    }
    // Unicode to VNI
    else if (fromCodeTable == 0 && toCodeTable == 2) {
        auto it = pImpl->unicodeToVniMap.find(character);
        if (it != pImpl->unicodeToVniMap.end()) {
            result = it->second;
        }
    }
    // TCVN3 to Unicode
    else if (fromCodeTable == 1 && toCodeTable == 0) {
        auto it = pImpl->tcvn3ToUnicodeMap.find(character);
        if (it != pImpl->tcvn3ToUnicodeMap.end()) {
            result = it->second;
        }
    }
    // VNI to Unicode
    else if (fromCodeTable == 2 && toCodeTable == 0) {
        auto it = pImpl->vniToUnicodeMap.find(character);
        if (it != pImpl->vniToUnicodeMap.end()) {
            result = it->second;
        }
    }

    // Cache the result
    pImpl->cacheCharacterConversion[std::hash<std::string>{}(cacheKey)] = result;

    return result;
}

std::wstring VietnameseProcessor::convertStringEncoding(const std::wstring& input, int fromCodeTable, int toCodeTable) {
    std::wstring result;
    result.reserve(input.size());

    for (Uint32 character : input) {
        result += convertCharacterEncoding(character, fromCodeTable, toCodeTable);
    }

    return result;
}

bool VietnameseProcessor::isValidVietnameseWord(const std::vector<Uint32>& word) {
    return pImpl && pImpl->isValidVietnameseWord(word);
}

bool VietnameseProcessor::hasInvalidConsonants(const std::vector<Uint32>& word) {
    return pImpl && pImpl->hasInvalidConsonantSequences(word);
}

bool VietnameseProcessor::applyQuickStartConsonant(std::vector<Uint32>& word) {
    if (!pImpl || word.empty()) return false;

    // Apply quick start consonant replacements
    // f -> ph, j -> gi, w -> qu
    bool modified = false;

    for (size_t i = 0; i < word.size(); ++i) {
        switch (word[i]) {
            case L'f':
                if (i + 1 < word.size()) {
                    word[i] = L'p';
                    word.insert(word.begin() + i + 1, L'h');
                    modified = true;
                }
                break;
            case L'j':
                word[i] = L'g';
                word.insert(word.begin() + i + 1, L'i');
                modified = true;
                break;
            case L'w':
                word[i] = L'q';
                word.insert(word.begin() + i + 1, L'u');
                modified = true;
                break;
        }
    }

    return modified;
}

bool VietnameseProcessor::applyQuickEndConsonant(std::vector<Uint32>& word) {
    if (!pImpl || word.empty()) return false;

    // Apply quick end consonant replacements
    // g -> ng, h -> nh, k -> ch
    bool modified = false;

    if (word.size() > 1) {
        size_t lastIndex = word.size() - 1;
        switch (word[lastIndex]) {
            case L'g':
                word.insert(word.begin() + lastIndex, L'n');
                modified = true;
                break;
            case L'h':
                word.insert(word.begin() + lastIndex, L'n');
                modified = true;
                break;
            case L'k':
                word[lastIndex] = L'c';
                word.insert(word.begin() + lastIndex, L'h');
                modified = true;
                break;
        }
    }

    return modified;
}