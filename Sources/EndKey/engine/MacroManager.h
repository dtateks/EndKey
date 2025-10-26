//
//  MacroManager.h
//  EndKey
//
//  Created by EndKey Refactoring on 2025.
//  Copyright © 2025 Tuyen Mai. All rights reserved.
//

#ifndef MacroManager_h
#define MacroManager_h

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include "DataType.h"

struct MacroEntry {
    std::wstring keyword;
    std::wstring expansion;
    bool autoCapitalize;

    MacroEntry(const std::wstring& key, const std::wstring& exp, bool cap = false)
        : keyword(key), expansion(exp), autoCapitalize(cap) {}
};

class MacroManager {
public:
    MacroManager();
    ~MacroManager();

    // Core macro functionality
    void addMacro(const std::wstring& keyword, const std::wstring& expansion, bool autoCapitalize = false);
    bool removeMacro(const std::wstring& keyword);
    bool hasMacro(const std::wstring& keyword) const;

    // Macro expansion
    std::wstring expandMacro(const std::wstring& keyword, bool isCaps = false, bool isAllCaps = false) const;
    bool shouldExpand(const std::wstring& currentWord, Uint16 keyCode) const;

    // Configuration
    void setMacroEnabled(bool enabled);
    bool isMacroEnabled() const;

    void setMacroInEnglishMode(bool enabled);
    bool isMacroInEnglishModeEnabled() const;

    void setAutoCapitalizeMacro(bool enabled);
    bool isAutoCapitalizeMacroEnabled() const;

    // Word boundary detection
    bool isMacroBreakCharacter(Uint16 keyCode) const;
    void setMacroBreakCharacter(Uint16 keyCode, bool isBreak);

    // Skip macro functionality
    void skipNextMacro();
    void forceSkipNextMacro();
    bool shouldSkipMacro() const;
    void resetMacroSkip();

    // Macro file operations
    bool loadMacrosFromFile(const std::string& filePath);
    bool saveMacrosToFile(const std::string& filePath) const;

    // Built-in macro management
    void initializeDefaultMacros();
    void clearAllMacros();

    // Macro statistics
    size_t getMacroCount() const;
    std::vector<std::wstring> getAllMacroKeywords() const;
    std::vector<MacroEntry> getAllMacros() const;

    // Case handling
    std::wstring applyCapitalization(const std::wstring& text, bool firstLetterOnly = false) const;
    std::wstring toUpperCase(const std::wstring& text) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    // Helper methods
    bool isValidMacroKeyword(const std::wstring& keyword) const;
    std::wstring normalizeKeyword(const std::wstring& keyword) const;
    bool matchesKeyword(const std::wstring& word, const std::wstring& keyword) const;

    // Character case conversion
    std::wstring capitalizeFirstLetter(const std::wstring& text) const;
    std::wstring capitalizeWord(const std::wstring& text) const;

    // Default macro definitions
    void setupDefaultMacros();

    // Break character management
    void initializeBreakCharacters();

    // Macro validation
    bool isValidMacroEntry(const MacroEntry& entry) const;

    // Performance optimization
    mutable std::unordered_map<std::wstring, std::wstring> cache_;
    bool isCacheValid(const std::wstring& keyword) const;
    void invalidateCache();
};

#endif /* MacroManager_h */