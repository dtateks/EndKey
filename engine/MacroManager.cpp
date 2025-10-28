//
//  MacroManager.cpp
//  EndKey
//
//  Created by EndKey Refactoring on 2025.
//  Copyright © 2025 Tuyen Mai. All rights reserved.
//

#include "MacroManager.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

// MacroManager::Impl structure with RAII pattern
struct MacroManager::Impl {
    // Macro storage
    std::unordered_map<std::wstring, MacroEntry> macros;
    std::unordered_map<Uint16, bool> breakCharacters;

    // Configuration
    bool macroEnabled;
    bool macroInEnglishMode;
    bool autoCapitalizeMacro;

    // Skip macro state
    bool skipNextMacro;
    bool forceSkipNextMacro;

    // Performance optimization
    mutable std::unordered_map<std::wstring, std::wstring> expansionCache;
    mutable bool cacheValid;

    // Constructor with RAII initialization
    Impl() : macroEnabled(true), macroInEnglishMode(false), autoCapitalizeMacro(true),
             skipNextMacro(false), forceSkipNextMacro(false), cacheValid(true) {
        try {
            initializeBreakCharacters();
            setupDefaultMacros();
        } catch (const std::exception& e) {
            throw std::runtime_error("Failed to initialize MacroManager: " + std::string(e.what()));
        }
    }

    // Destructor - automatic cleanup via STL containers
    ~Impl() = default;

    // Initialize break characters
    void initializeBreakCharacters() {
        // Control keys
        breakCharacters[KEY_RETURN] = true;
        breakCharacters[KEY_SPACE] = true;
        breakCharacters[KEY_TAB] = true;
        breakCharacters[KEY_ESC] = true;

        // Numbers
        for (Uint16 key = KEY_1; key <= KEY_0; ++key) {
            breakCharacters[key] = true;
        }

        // Punctuation and symbols
        breakCharacters[KEY_COMMA] = true;
        breakCharacters[KEY_DOT] = true;
        breakCharacters[KEY_SLASH] = true;
        breakCharacters[KEY_SEMICOLON] = true;
        breakCharacters[KEY_QUOTE] = true;
        breakCharacters[KEY_BACK_SLASH] = true;
        breakCharacters[KEY_MINUS] = true;
        breakCharacters[KEY_EQUALS] = true;
        breakCharacters[KEY_LEFT_BRACKET] = true;
        breakCharacters[KEY_RIGHT_BRACKET] = true;
        breakCharacters[KEY_BACKQUOTE] = true;
    }

    // Setup default macros
    void setupDefaultMacros() {
        // Common abbreviations
        addMacro(L"btw", L"by the way", true);
        addMacro(L"lol", L"laughing out loud", true);
        addMacro(L"omg", L"oh my god", true);
        addMacro(L"ttyl", L"talk to you later", true);
        addMacro(L"afaik", L"as far as I know", true);
        addMacro(L"imo", L"in my opinion", true);
        addMacro(L"btw", L"by the way", true);
        addMacro(L"fyi", L"for your information", true);

        // Vietnamese-specific macros
        addMacro(L"ko", L"không", false);
        addMacro(L"k", L"không", false);
        addMacro(L"dc", L"được", false);
        addMacro(L"ntn", L"như thế nào", false);
        addMacro(L"j", L"gì", false);
        addMacro(L"vs", L"với", false);
        addMacro(L"mn", L"mọi người", false);
        addMacro(L"ad", L"admin", false);

        // Technical terms
        addMacro(L"api", L"Application Programming Interface", true);
        addMacro(L"ui", L"User Interface", true);
        addMacro(L"ux", L"User Experience", true);
        addMacro(L"sql", L"Structured Query Language", true);
    }

    // Helper methods
    std::wstring normalizeKeyword(const std::wstring& keyword) const {
        std::wstring result = keyword;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }

    std::wstring applyCapitalization(const std::wstring& text, bool firstLetterOnly = false) const {
        if (text.empty()) return text;

        std::wstring result = text;

        if (firstLetterOnly) {
            // Capitalize first letter only
            if (!result.empty() && iswlower(result[0])) {
                result[0] = towupper(result[0]);
            }
        } else {
            // Capitalize first letter of each word
            bool capitalizeNext = true;
            for (size_t i = 0; i < result.size(); ++i) {
                if (capitalizeNext && iswlower(result[i])) {
                    result[i] = towupper(result[i]);
                    capitalizeNext = false;
                } else if (iswspace(result[i])) {
                    capitalizeNext = true;
                }
            }
        }

        return result;
    }

    std::wstring toUpperCase(const std::wstring& text) const {
        std::wstring result = text;
        std::transform(result.begin(), result.end(), result.begin(), ::towupper);
        return result;
    }

    bool isValidMacroKeyword(const std::wstring& keyword) const {
        if (keyword.empty() || keyword.length() > 50) {
            return false;
        }

        // Check if keyword contains only valid characters
        for (wchar_t ch : keyword) {
            if (!iswalnum(ch) && ch != L'_' && ch != L'-') {
                return false;
            }
        }

        return true;
    }

    void invalidateCache() {
        expansionCache.clear();
        cacheValid = false;
    }

    bool shouldSkipMacro() const {
        return skipNextMacro || forceSkipNextMacro;
    }
};

// MacroManager public implementation
MacroManager::MacroManager() : pImpl(std::make_unique<Impl>()) {
}

MacroManager::~MacroManager() = default;

void MacroManager::addMacro(const std::wstring& keyword, const std::wstring& expansion, bool autoCapitalize) {
    if (!pImpl) return;

    std::wstring normalizedKeyword = pImpl->normalizeKeyword(keyword);

    if (!pImpl->isValidMacroKeyword(normalizedKeyword)) {
        return;
    }

    MacroEntry entry(normalizedKeyword, expansion, autoCapitalize);
    pImpl->macros[normalizedKeyword] = entry;
    pImpl->invalidateCache();
}

bool MacroManager::removeMacro(const std::wstring& keyword) {
    if (!pImpl) return false;

    std::wstring normalizedKeyword = pImpl->normalizeKeyword(keyword);
    auto it = pImpl->macros.find(normalizedKeyword);

    if (it != pImpl->macros.end()) {
        pImpl->macros.erase(it);
        pImpl->invalidateCache();
        return true;
    }

    return false;
}

bool MacroManager::hasMacro(const std::wstring& keyword) const {
    if (!pImpl) return false;

    std::wstring normalizedKeyword = pImpl->normalizeKeyword(keyword);
    return pImpl->macros.find(normalizedKeyword) != pImpl->macros.end();
}

std::wstring MacroManager::expandMacro(const std::wstring& keyword, bool isCaps, bool isAllCaps) const {
    if (!pImpl || !pImpl->macroEnabled || pImpl->shouldSkipMacro()) {
        return L"";
    }

    std::wstring normalizedKeyword = pImpl->normalizeKeyword(keyword);

    // Check cache first
    std::wstring cacheKey = normalizedKeyword + L"_" + (isCaps ? L"1" : L"0") + L"_" + (isAllCaps ? L"1" : L"0");
    auto cacheIt = pImpl->expansionCache.find(cacheKey);
    if (cacheIt != pImpl->expansionCache.end()) {
        return cacheIt->second;
    }

    auto it = pImpl->macros.find(normalizedKeyword);
    if (it == pImpl->macros.end()) {
        return L"";
    }

    std::wstring result = it->second.expansion;

    // Apply capitalization
    if (isAllCaps) {
        result = pImpl->toUpperCase(result);
    } else if (isCaps && it->second.autoCapitalize) {
        result = pImpl->applyCapitalization(result, true);
    }

    // Cache the result
    pImpl->expansionCache[cacheKey] = result;

    return result;
}

bool MacroManager::shouldExpand(const std::wstring& currentWord, Uint16 keyCode) const {
    if (!pImpl || !pImpl->macroEnabled || pImpl->shouldSkipMacro()) {
        return false;
    }

    // Check if this is a break character
    auto breakIt = pImpl->breakCharacters.find(keyCode);
    if (breakIt == pImpl->breakCharacters.end() || !breakIt->second) {
        return false;
    }

    // Check if current word matches a macro
    return hasMacro(currentWord);
}

void MacroManager::setMacroEnabled(bool enabled) {
    if (pImpl) {
        pImpl->macroEnabled = enabled;
    }
}

bool MacroManager::isMacroEnabled() const {
    return pImpl ? pImpl->macroEnabled : false;
}

void MacroManager::setMacroInEnglishMode(bool enabled) {
    if (pImpl) {
        pImpl->macroInEnglishMode = enabled;
    }
}

bool MacroManager::isMacroInEnglishModeEnabled() const {
    return pImpl ? pImpl->macroInEnglishMode : false;
}

void MacroManager::setAutoCapitalizeMacro(bool enabled) {
    if (pImpl) {
        pImpl->autoCapitalizeMacro = enabled;
    }
}

bool MacroManager::isAutoCapitalizeMacroEnabled() const {
    return pImpl ? pImpl->autoCapitalizeMacro : false;
}

bool MacroManager::isMacroBreakCharacter(Uint16 keyCode) const {
    if (!pImpl) return false;

    auto it = pImpl->breakCharacters.find(keyCode);
    return it != pImpl->breakCharacters.end() && it->second;
}

void MacroManager::setMacroBreakCharacter(Uint16 keyCode, bool isBreak) {
    if (pImpl) {
        pImpl->breakCharacters[keyCode] = isBreak;
    }
}

void MacroManager::skipNextMacro() {
    if (pImpl) {
        pImpl->skipNextMacro = true;
    }
}

void MacroManager::forceSkipNextMacro() {
    if (pImpl) {
        pImpl->forceSkipNextMacro = true;
    }
}

bool MacroManager::shouldSkipMacro() const {
    return pImpl ? pImpl->shouldSkipMacro() : false;
}

void MacroManager::resetMacroSkip() {
    if (pImpl) {
        pImpl->skipNextMacro = false;
        pImpl->forceSkipNextMacro = false;
    }
}

bool MacroManager::loadMacrosFromFile(const std::string& filePath) {
    if (!pImpl) return false;

    try {
        std::wifstream file(filePath);
        if (!file.is_open()) {
            return false;
        }

        std::wstring line;
        while (std::getline(file, line)) {
            // Skip empty lines and comments
            if (line.empty() || line[0] == L'#') {
                continue;
            }

            // Parse macro definition (format: keyword=expansion)
            size_t pos = line.find(L'=');
            if (pos != std::wstring::npos) {
                std::wstring keyword = line.substr(0, pos);
                std::wstring expansion = line.substr(pos + 1);

                // Remove whitespace
                keyword.erase(0, keyword.find_first_not_of(L" \t"));
                keyword.erase(keyword.find_last_not_of(L" \t") + 1);
                expansion.erase(0, expansion.find_first_not_of(L" \t"));
                expansion.erase(expansion.find_last_not_of(L" \t") + 1);

                addMacro(keyword, expansion);
            }
        }

        file.close();
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool MacroManager::saveMacrosToFile(const std::string& filePath) const {
    if (!pImpl) return false;

    try {
        std::wofstream file(filePath);
        if (!file.is_open()) {
            return false;
        }

        // Write header
        file << L"# EndKey Macro Definitions\n";
        file << L"# Format: keyword=expansion\n\n";

        // Write macros
        for (const auto& pair : pImpl->macros) {
            file << pair.second.keyword << L"=" << pair.second.expansion << L"\n";
        }

        file.close();
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

void MacroManager::initializeDefaultMacros() {
    if (pImpl) {
        pImpl->setupDefaultMacros();
    }
}

void MacroManager::clearAllMacros() {
    if (pImpl) {
        pImpl->macros.clear();
        pImpl->invalidateCache();
    }
}

size_t MacroManager::getMacroCount() const {
    return pImpl ? pImpl->macros.size() : 0;
}

std::vector<std::wstring> MacroManager::getAllMacroKeywords() const {
    std::vector<std::wstring> keywords;

    if (pImpl) {
        keywords.reserve(pImpl->macros.size());
        for (const auto& pair : pImpl->macros) {
            keywords.push_back(pair.second.keyword);
        }
    }

    return keywords;
}

std::vector<MacroEntry> MacroManager::getAllMacros() const {
    std::vector<MacroEntry> macroList;

    if (pImpl) {
        macroList.reserve(pImpl->macros.size());
        for (const auto& pair : pImpl->macros) {
            macroList.push_back(pair.second);
        }
    }

    return macroList;
}

std::wstring MacroManager::applyCapitalization(const std::wstring& text, bool firstLetterOnly) const {
    return pImpl ? pImpl->applyCapitalization(text, firstLetterOnly) : text;
}

std::wstring MacroManager::toUpperCase(const std::wstring& text) const {
    return pImpl ? pImpl->toUpperCase(text) : text;
}