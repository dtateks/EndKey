//
//  VietnameseProcessor.h
//  EndKey
//
//  Created by EndKey Refactoring on 2025.
//  Copyright © 2025 Tuyen Mai. All rights reserved.
//

#ifndef VietnameseProcessor_h
#define VietnameseProcessor_h

#include <memory>
#include <vector>
#include <string>
#include "DataType.h"

class VietnameseProcessor {
public:
    VietnameseProcessor();
    ~VietnameseProcessor();

    // Core Vietnamese processing methods
    bool processToneMark(Uint32& character, Uint8 toneKey, int inputType);
    bool processCharacterMark(Uint32& character, Uint8 markKey, int inputType);
    bool isVowelCharacter(Uint32 character);
    bool isValidVietnameseSequence(const std::vector<Uint32>& word);

    // Modern orthography support
    void setModernOrthography(bool enabled);
    bool getModernOrthography() const;

    // Character conversion utilities
    Uint32 convertCharacterEncoding(Uint32 character, int fromCodeTable, int toCodeTable);
    std::wstring convertStringEncoding(const std::wstring& input, int fromCodeTable, int toCodeTable);

    // Spelling and validation
    bool isValidVietnameseWord(const std::vector<Uint32>& word);
    bool hasInvalidConsonants(const std::vector<Uint32>& word);

    // Quick consonant replacements
    bool applyQuickStartConsonant(std::vector<Uint32>& word);
    bool applyQuickEndConsonant(std::vector<Uint32>& word);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    // Helper methods
    bool isConsonant(Uint32 character);
    bool isVowel(Uint32 character);
    bool canApplyTone(Uint32 character, Uint8 toneType);
    bool canApplyMark(Uint32 character, Uint8 markType);

    // Internal character tables
    static const std::vector<Uint8> vowelStartIndices;
    static const std::vector<Uint8> vowelEndIndices;
    static const std::vector<bool> vowelWillSetMark;

    // Tone mark processing
    Uint32 applyToneMark(Uint32 character, Uint8 toneType);
    Uint32 removeToneMark(Uint32 character);
    Uint32 getToneType(Uint32 character);

    // Character mark processing
    Uint32 applyCharacterMark(Uint32 character, Uint8 markType);
    Uint32 removeCharacterMark(Uint32 character);
    Uint8 getCharacterMark(Uint32 character);

    // Character encoding conversion tables
    static const Uint32 unicodeToTcvn3Table[];
    static const Uint32 unicodeToVniTable[];
    static const std::vector<Uint32> tcvn3ToUnicodeTable;
    static const std::vector<Uint32> vniToUnicodeTable;
};

#endif /* VietnameseProcessor_h */