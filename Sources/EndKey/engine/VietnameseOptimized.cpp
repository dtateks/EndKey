//
//  VietnameseOptimized.cpp
//  EndKey
//
//  Created by Refactoring on 10/26/25.
//  Copyright © 2025 EndKey. All rights reserved.
//

#include "VietnameseOptimized.h"
#include <chrono>
#include <algorithm>
#include <iostream>

namespace EndKey {
    namespace Engine {

        // Global instance for legacy compatibility
        std::unique_ptr<VietnameseOptimized> gVietnameseOptimized = nullptr;

        VietnameseOptimized::VietnameseOptimized() : stats_(), tablesLoaded_(false) {
            // Reserve cache capacity
            characterCache_.reserve(MAX_CHARACTER_CACHE / 4);
            vowelSequenceCache_.reserve(MAX_SEQUENCE_CACHE / 4);
            wordValidationCache_.reserve(MAX_WORD_CACHE / 4);

            // Initialize quick consonant mappings
            loadQuickConsonantMappings();
        }

        VietnameseOptimized::~VietnameseOptimized() = default;

        VietnameseOptimized::CharacterInfo VietnameseOptimized::getCharacterInfo(Uint16 character) const {
            stats_.characterLookups++;

            // Check cache first
            Uint64 cacheKey = makeCharacterCacheKey(character, 0, 0);
            auto cacheIt = characterCache_.find(cacheKey);
            if (cacheIt != characterCache_.end()) {
                stats_.cacheHits++;
                return cacheIt->second;
            }

            stats_.cacheMisses++;

            CharacterInfo info = {character, character, 0, 0, false, false, false, false};

            if (!tablesLoaded_) {
                loadCharacterTables();
            }

            // Check if character is vowel
            if (vowelSet_->find(character) != vowelSet_->end()) {
                info.isVowel = true;
                info.canTakeTone = true;
                info.canTakeMark = true;
            }
            // Check if character is consonant
            else if (consonantSet_->find(character) != consonantSet_->end()) {
                info.isConsonant = true;
            }

            // Cache the result
            if (characterCache_.size() >= MAX_CHARACTER_CACHE) {
                evictOldestCharacterCache();
            }
            characterCache_[cacheKey] = info;

            return info;
        }

        Uint16 VietnameseOptimized::getVowelWithMark(Uint16 baseChar, Uint8 markType) const {
            if (markType == 0 || !isVowelCharacter(baseChar)) {
                return baseChar;
            }

            // Check cache
            Uint64 cacheKey = makeCharacterCacheKey(baseChar, markType, 0);
            auto cacheIt = characterCache_.find(cacheKey);
            if (cacheIt != characterCache_.end()) {
                return cacheIt->second.combinedChar;
            }

            // Apply mark logic based on mark type
            Uint16 result = baseChar;
            switch (markType) {
                case 1: // Horn (ươ, ơ)
                    if (baseChar == KEY_U) result = 0x01B0; // ư
                    else if (baseChar == KEY_O) result = 0x01A1; // ơ
                    break;
                case 2: // Breve (ă)
                    if (baseChar == KEY_A) result = 0x0103; // ă
                    break;
                case 3: // Circumflex (â, ê, ô)
                    if (baseChar == KEY_A) result = 0x00E2; // â
                    else if (baseChar == KEY_E) result = 0x00EA; // ê
                    else if (baseChar == KEY_O) result = 0x00F4; // ô
                    break;
                case 4: // D stroke (đ)
                    if (baseChar == KEY_D) result = 0x0111; // đ
                    break;
            }

            // Cache the result
            if (characterCache_.size() >= MAX_CHARACTER_CACHE) {
                evictOldestCharacterCache();
            }
            CharacterInfo info = {baseChar, result, 0, markType, true, false, false, true};
            characterCache_[cacheKey] = info;

            return result;
        }

        Uint16 VietnameseOptimized::getVowelWithTone(Uint16 baseChar, Uint8 toneType) const {
            if (toneType == 0 || !isVowelCharacter(baseChar)) {
                return baseChar;
            }

            // Check cache
            Uint64 cacheKey = makeCharacterCacheKey(baseChar, 0, toneType);
            auto cacheIt = characterCache_.find(cacheKey);
            if (cacheIt != characterCache_.end()) {
                return cacheIt->second.combinedChar;
            }

            // Apply tone marks
            Uint16 result = applyToneMark(baseChar, toneType);

            // Cache the result
            if (characterCache_.size() >= MAX_CHARACTER_CACHE) {
                evictOldestCharacterCache();
            }
            CharacterInfo info = {baseChar, result, toneType, 0, true, false, true, false};
            characterCache_[cacheKey] = info;

            return result;
        }

        Uint16 VietnameseOptimized::getCombinedCharacter(Uint16 baseChar, Uint8 markType, Uint8 toneType) const {
            Uint16 result = baseChar;

            // Apply mark first
            if (markType > 0) {
                result = getVowelWithMark(result, markType);
            }

            // Then apply tone
            if (toneType > 0) {
                result = getVowelWithTone(result, toneType);
            }

            return result;
        }

        VietnameseOptimized::VowelSequence VietnameseOptimized::analyzeVowelSequence(const std::vector<Uint16>& characters) const {
            stats_.vowelSequencesAnalyzed++;

            VowelSequence sequence;
            sequence.sequence.reserve(characters.size());

            // Extract vowels from input
            for (Uint16 charCode : characters) {
                if (isVowelCharacter(charCode)) {
                    sequence.sequence.push_back(charCode);
                }
            }

            if (sequence.sequence.empty()) {
                sequence.isValid = false;
                return sequence;
            }

            // Simple validation - more complex rules can be added
            sequence.isValid = isValidVowelSequence(sequence.sequence);
            if (sequence.isValid && !sequence.sequence.empty()) {
                sequence.primaryVowel = sequence.sequence[sequence.sequence.size() / 2]; // Middle vowel
                sequence.tonePosition = 0; // First vowel by default
            }

            return sequence;
        }

        bool VietnameseOptimized::isValidVowelSequence(const std::vector<Uint16>& characters) const {
            if (characters.empty() || characters.size() > 3) {
                return false;
            }

            // Basic vowel sequence validation
            for (Uint16 charCode : characters) {
                if (!isVowelCharacter(charCode)) {
                    return false;
                }
            }

            return true;
        }

        Uint16 VietnameseOptimized::findPrimaryVowel(const std::vector<Uint16>& characters) const {
            auto sequence = analyzeVowelSequence(characters);
            return sequence.primaryVowel;
        }

        Uint8 VietnameseOptimized::findTonePosition(const std::vector<Uint16>& characters) const {
            auto sequence = analyzeVowelSequence(characters);
            return sequence.tonePosition;
        }

        bool VietnameseOptimized::isValidStartConsonant(Uint16 consonant) const {
            if (!tablesLoaded_) {
                loadCharacterTables();
            }
            return consonantSet_->find(consonant) != consonantSet_->end();
        }

        bool VietnameseOptimized::isValidEndConsonant(Uint16 consonant) const {
            // Vietnamese end consonants: p, t, c, m, n, ng, nh, ch
            static const std::unordered_set<Uint16> endConsonants = {
                'p', 't', 'c', 'm', 'n',
                'P', 'T', 'C', 'M', 'N'
            };
            return endConsonants.find(consonant) != endConsonants.end();
        }

        bool VietnameseOptimized::isValidConsonantPair(Uint16 first, Uint16 second) const {
            if (!tablesLoaded_) {
                loadConsonantPairs();
            }

            auto pairIt = consonantPairs_->find(first);
            if (pairIt != consonantPairs_->end()) {
                for (const auto& pair : pairIt->second) {
                    if (pair.second == second && pair.isValid) {
                        return true;
                    }
                }
            }
            return false;
        }

        std::vector<VietnameseOptimized::ConsonantPair> VietnameseOptimized::getValidConsonantPairs() const {
            std::vector<ConsonantPair> pairs;

            if (!tablesLoaded_) {
                loadConsonantPairs();
            }

            for (const auto& pairList : *consonantPairs_) {
                for (const auto& pair : pairList.second) {
                    if (pair.isValid) {
                        pairs.push_back(pair);
                    }
                }
            }

            return pairs;
        }

        bool VietnameseOptimized::isValidVietnameseWord(const std::vector<Uint16>& word) const {
            if (word.empty()) {
                return true;
            }

            stats_.wordsValidated++;

            // Check cache first
            Uint32 cacheKey = makeWordCacheKey(word);
            auto cacheIt = wordValidationCache_.find(cacheKey);
            if (cacheIt != wordValidationCache_.end()) {
                return cacheIt->second;
            }

            bool isValid = hasValidSyllableStructure(word);

            // Cache the result
            if (wordValidationCache_.size() >= MAX_WORD_CACHE) {
                evictOldestWordCache();
            }
            wordValidationCache_[cacheKey] = isValid;

            return isValid;
        }

        bool VietnameseOptimized::hasValidSyllableStructure(const std::vector<Uint16>& word) const {
            if (word.empty()) {
                return false;
            }

            // Extract vowels and consonants
            std::vector<Uint16> vowels;
            std::vector<Uint16> consonants;

            for (Uint16 charCode : word) {
                if (isVowelCharacter(charCode)) {
                    vowels.push_back(charCode);
                } else if (isConsonantCharacter(charCode)) {
                    consonants.push_back(charCode);
                }
            }

            // Must have at least one vowel
            if (vowels.empty()) {
                return false;
            }

            // Validate vowel sequence
            if (!isValidVowelSequence(vowels)) {
                return false;
            }

            return true;
        }

        Uint16 VietnameseOptimized::processDoubleKey(Uint16 baseKey) const {
            for (const auto& doubleKey : DOUBLE_KEYS) {
                if (doubleKey.first == baseKey) {
                    return doubleKey.second;
                }
            }
            return baseKey;
        }

        bool VietnameseOptimized::isDoubleKeyCharacter(Uint16 character) const {
            for (const auto& doubleKey : DOUBLE_KEYS) {
                if (doubleKey.second == character) {
                    return true;
                }
            }
            return false;
        }

        std::vector<Uint16> VietnameseOptimized::processQuickStartConsonant(Uint16 input) const {
            if (!quickStartConsonant_) {
                return {input};
            }

            auto it = quickStartConsonant_->find(input);
            if (it != quickStartConsonant_->end()) {
                return it->second;
            }

            return {input};
        }

        std::vector<Uint16> VietnameseOptimized::processQuickEndConsonant(Uint16 input) const {
            if (!quickEndConsonant_) {
                return {input};
            }

            auto it = quickEndConsonant_->find(input);
            if (it != quickEndConsonant_->end()) {
                return it->second;
            }

            return {input};
        }

        void VietnameseOptimized::preloadCommonCharacters() {
            if (!tablesLoaded_) {
                loadCharacterTables();
            }

            // Preload frequently used characters
            std::vector<Uint16> commonChars = {
                'a', 'e', 'i', 'o', 'u', 'y',
                L'ă', L'â', L'ê', L'ô', L'ơ', L'ư',
                L'á', L'à', L'ả', L'ã', L'ạ',
                L'é', L'è', L'ẻ', L'ẽ', L'ẹ',
                L'í', L'ì', L'ỉ', L'ĩ', L'ị',
                L'ó', L'ò', L'ỏ', L'õ', L'ọ',
                L'ú', L'ù', L'ủ', L'ũ', L'ụ',
                L'ý', L'ỳ', L'ỷ', L'ỹ', L'ỵ'
            };

            for (Uint16 charCode : commonChars) {
                getCharacterInfo(charCode);
            }
        }

        void VietnameseOptimized::preloadCommonWords() {
            // Preload frequently used Vietnamese words
            std::vector<std::vector<Uint16>> commonWords = {
                {L'v', L'i', L'ệ', L't'},         // việt
                {L'n', L'a', L'm'},               // nam
                {L'n', L'g', L'ư', L'ờ', L'i'},   // người
                {L'g', L'i', L'à'},               // gia
                {L'n', L'h', L'à'}                // nhà
            };

            for (const auto& word : commonWords) {
                isValidVietnameseWord(word);
            }
        }

        void VietnameseOptimized::optimizeForInputMethod(int inputType) {
            // Optimize caches and data structures for specific input method
            preloadCommonCharacters();
            loadCommonVowelSequences();
            loadCommonConsonantPairs();
        }

        void VietnameseOptimized::clearCache() {
            characterCache_.clear();
            vowelSequenceCache_.clear();
            wordValidationCache_.clear();
            stats_.cacheHits = 0;
            stats_.cacheMisses = 0;
        }

        const VietnameseOptimized::ProcessingStats& VietnameseOptimized::getStatistics() const {
            return stats_;
        }

        void VietnameseOptimized::resetStatistics() {
            stats_ = ProcessingStats{};
        }

        size_t VietnameseOptimized::getMemoryUsage() const {
            size_t total = 0;

            // Character map
            if (characterMap_) {
                total += characterMap_->size() * (sizeof(Uint16) + sizeof(CharacterInfo));
            }

            // Vowel and consonant sets
            if (vowelSet_) {
                total += vowelSet_->size() * sizeof(Uint16);
            }
            if (consonantSet_) {
                total += consonantSet_->size() * sizeof(Uint16);
            }

            // Caches
            total += characterCache_.size() * (sizeof(Uint64) + sizeof(CharacterInfo));
            total += vowelSequenceCache_.size() * (sizeof(Uint64) + sizeof(VowelSequence));
            total += wordValidationCache_.size() * (sizeof(Uint32) + sizeof(bool));

            return total;
        }

        size_t VietnameseOptimized::getCacheSize() const {
            return characterCache_.size() + vowelSequenceCache_.size() + wordValidationCache_.size();
        }

        void VietnameseOptimized::compactMemory() {
            // Compact caches if they're using excessive capacity
            if (characterCache_.capacity() > characterCache_.size() * 2) {
                characterCache_.reserve(characterCache_.size());
            }
            if (vowelSequenceCache_.capacity() > vowelSequenceCache_.size() * 2) {
                vowelSequenceCache_.reserve(vowelSequenceCache_.size());
            }
            if (wordValidationCache_.capacity() > wordValidationCache_.size() * 2) {
                wordValidationCache_.reserve(wordValidationCache_.size());
            }

            cleanupUnusedData();
        }

        // Private helper methods

        void VietnameseOptimized::loadCharacterTables() const {
            if (tablesLoaded_) {
                return;
            }

            characterMap_ = std::make_unique<std::unordered_map<Uint16, CharacterInfo>>();
            vowelSet_ = std::make_unique<std::unordered_set<Uint16>>();
            consonantSet_ = std::make_unique<std::unordered_set<Uint16>>();

            // Load vowel characters
            std::vector<Uint16> vowels = {
                'a', 'e', 'i', 'o', 'u', 'y',
                L'ă', L'â', L'ê', L'ô', L'ơ', L'ư', L'đ',
                L'Á', L'À', L'Ả', L'Ã', L'Ạ', L'Ắ', L'Ằ', L'Ẳ', L'Ẵ', L'Ặ',
                L'Â', L'Ấ', L'Ầ', L'Ẩ', L'Ẫ', L'Ậ', L'É', L'È', L'Ẻ', L'Ẽ', L'Ẹ',
                L'Ê', L'Ế', L'Ề', L'Ể', L'Ễ', L'Ệ', L'Í', L'Ì', L'Ỉ', L'Ĩ', L'Ị',
                L'Ó', L'Ò', L'Ỏ', L'Õ', L'Ọ', L'Ô', L'Ố', L'Ồ', L'Ổ', L'Ỗ', L'Ộ',
                L'Ơ', L'Ớ', L'Ờ', L'Ở', L'Ỡ', L'Ợ', L'Ú', L'Ù', L'Ủ', L'Ũ', L'Ụ',
                L'Ư', L'Ứ', L'Ừ', L'Ử', L'Ữ', L'Ự', L'Ý', L'Ỳ', L'Ỷ', L'Ỹ', L'Ỵ'
            };

            for (Uint16 vowel : vowels) {
                vowelSet_->insert(vowel);
            }

            // Load consonant characters
            std::vector<Uint16> consonants = {
                'b', 'c', 'd', 'g', 'h', 'k', 'l', 'm', 'n', 'p', 'q', 'r', 's', 't', 'v', 'x',
                'B', 'C', 'D', 'G', 'H', 'K', 'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'V', 'X',
                L'Đ', 'f', 'j', 'w', 'z', 'F', 'J', 'W', 'Z'
            };

            for (Uint16 consonant : consonants) {
                consonantSet_->insert(consonant);
            }

            tablesLoaded_ = true;
        }

        void VietnameseOptimized::loadVowelSequences() const {
            if (!vowelSequences_) {
                vowelSequences_ = std::make_unique<std::unordered_map<Uint16, std::vector<VowelSequence>>>();
            }
        }

        void VietnameseOptimized::loadConsonantPairs() const {
            if (!consonantPairs_) {
                consonantPairs_ = std::make_unique<std::unordered_map<Uint16, std::vector<ConsonantPair>>>();
            }

            // Load common consonant pairs
            (*consonantPairs_)[KEY_T].push_back({KEY_T, KEY_H, true, false});  // th
            (*consonantPairs_)[KEY_N].push_back({KEY_N, KEY_G, true, false});  // ng
            (*consonantPairs_)[KEY_N].push_back({KEY_N, KEY_H, true, false});  // nh
            (*consonantPairs_)[KEY_C].push_back({KEY_C, KEY_H, true, false});  // ch
            (*consonantPairs_)[KEY_G].push_back({KEY_G, KEY_I, true, true});   // gi
            (*consonantPairs_)[KEY_Q].push_back({KEY_Q, KEY_U, true, true});   // qu
        }

        void VietnameseOptimized::loadQuickConsonantMappings() const {
            quickStartConsonant_ = std::make_unique<std::unordered_map<Uint16, std::vector<Uint16>>>();
            quickEndConsonant_ = std::make_unique<std::unordered_map<Uint16, std::vector<Uint16>>>();

            // Quick start consonants
            (*quickStartConsonant_)[KEY_F] = {KEY_P, KEY_H};  // f -> ph
            (*quickStartConsonant_)[KEY_J] = {KEY_G, KEY_I};  // j -> gi
            (*quickStartConsonant_)[KEY_W] = {KEY_Q, KEY_U};  // w -> qu

            // Quick end consonants
            (*quickEndConsonant_)[KEY_G] = {KEY_N, KEY_G};    // g -> ng
            (*quickEndConsonant_)[KEY_H] = {KEY_N, KEY_H};    // h -> nh
            (*quickEndConsonant_)[KEY_K] = {KEY_C, KEY_H};    // k -> ch
        }

        void VietnameseOptimized::evictOldestCharacterCache() {
            // Simple eviction strategy - remove oldest 25%
            size_t entriesToRemove = characterCache_.size() / 4;
            auto it = characterCache_.begin();
            std::advance(it, entriesToRemove);
            characterCache_.erase(characterCache_.begin(), it);
        }

        void VietnameseOptimized::evictOldestSequenceCache() {
            size_t entriesToRemove = vowelSequenceCache_.size() / 4;
            auto it = vowelSequenceCache_.begin();
            std::advance(it, entriesToRemove);
            vowelSequenceCache_.erase(vowelSequenceCache_.begin(), it);
        }

        void VietnameseOptimized::evictOldestWordCache() {
            size_t entriesToRemove = wordValidationCache_.size() / 4;
            auto it = wordValidationCache_.begin();
            std::advance(it, entriesToRemove);
            wordValidationCache_.erase(wordValidationCache_.begin(), it);
        }

        Uint64 VietnameseOptimized::makeCharacterCacheKey(Uint16 character, Uint8 markType, Uint8 toneType) const {
            return (static_cast<Uint64>(character) << 32) | (static_cast<Uint64>(markType) << 16) | toneType;
        }

        Uint64 VietnameseOptimized::makeSequenceCacheKey(const std::vector<Uint16>& sequence) const {
            Uint64 key = 0;
            for (size_t i = 0; i < std::min(sequence.size(), size_t(4)); ++i) {
                key = (key << 16) | sequence[i];
            }
            return key;
        }

        Uint32 VietnameseOptimized::makeWordCacheKey(const std::vector<Uint16>& word) const {
            Uint32 key = 0;
            for (size_t i = 0; i < std::min(word.size(), size_t(2)); ++i) {
                key = (key << 16) | word[i];
            }
            return key;
        }

        void VietnameseOptimized::loadFrequentlyUsedCharacters() const {
            preloadCommonCharacters();
        }

        void VietnameseOptimized::loadCommonVowelSequences() const {
            loadVowelSequences();
        }

        void VietnameseOptimized::loadCommonConsonantPairs() const {
            loadConsonantPairs();
        }

        void VietnameseOptimized::cleanupUnusedData() {
            // Remove old cache entries that haven't been used recently
            // This is a simplified implementation
            if (characterCache_.size() > MAX_CHARACTER_CACHE / 2) {
                evictOldestCharacterCache();
            }
        }

        void VietnameseOptimized::optimizeCacheHitRatio() {
            // Analyze cache performance and adjust sizes accordingly
            double hitRatio = stats_.characterLookups > 0 ?
                static_cast<double>(stats_.cacheHits) / stats_.characterLookups : 0.0;

            if (hitRatio < 0.8) {
                // Low hit ratio, increase cache size
                characterCache_.reserve(std::min(characterCache_.capacity() * 2, MAX_CHARACTER_CACHE));
            }
        }

        void VietnameseOptimized::compactDataStructures() {
            compactMemory();
        }

        bool VietnameseOptimized::isVowelCharacter(Uint16 character) const {
            if (!tablesLoaded_) {
                loadCharacterTables();
            }
            return vowelSet_->find(character) != vowelSet_->end();
        }

        bool VietnameseOptimized::isConsonantCharacter(Uint16 character) const {
            if (!tablesLoaded_) {
                loadCharacterTables();
            }
            return consonantSet_->find(character) != consonantSet_->end();
        }

        Uint8 VietnameseOptimized::getCharacterToneType(Uint16 character) const {
            // Detect tone type from character
            // Returns 0-5 for different tone types
            if (character >= 0x00C0 && character <= 0x00FF) {
                // Latin-1 Supplement with Vietnamese characters
                if (character == 0x00C0 || character == 0x00E0 || // À, à
                    character == 0x00C8 || character == 0x00E8 || // È, è
                    character == 0x00CC || character == 0x00EC || // Ì, ì
                    character == 0x00D2 || character == 0x00F2 || // Ò, ò
                    character == 0x00D9 || character == 0x00F9 || // Ù, ù
                    character == 0x1EA0 || character == 0x1EA1) { // Ạ, ạ
                    return 2; // Grave accent
                }
                // Add more tone detection logic here
            }
            return 0; // No tone
        }

        Uint8 VietnameseOptimized::getCharacterMarkType(Uint16 character) const {
            // Detect mark type from character
            // Returns 0-6 for different mark types
            if (character == 0x0102 || character == 0x0103) return 2; // Breve
            if (character == 0x00C2 || character == 0x00E2 ||
                character == 0x00CA || character == 0x00EA ||
                character == 0x00D4 || character == 0x00F4) return 3; // Circumflex
            if (character == 0x01A0 || character == 0x01A1 ||
                character == 0x01AF || character == 0x01B0) return 1; // Horn
            if (character == 0x0110 || character == 0x0111) return 4; // D stroke
            return 0; // No mark
        }

        Uint16 VietnameseOptimized::applyToneMark(Uint16 baseChar, Uint8 toneType) const {
            // Apply tone mark to base character
            // This is a simplified implementation
            switch (toneType) {
                case 1: // Acute
                    if (baseChar == 'a') return 0x00E1; // á
                    if (baseChar == 'e') return 0x00E9; // é
                    if (baseChar == 'i') return 0x00ED; // í
                    if (baseChar == 'o') return 0x00F3; // ó
                    if (baseChar == 'u') return 0x00FA; // ú
                    if (baseChar == 'y') return 0x00FD; // ý
                    break;
                case 2: // Grave
                    if (baseChar == 'a') return 0x00E0; // à
                    if (baseChar == 'e') return 0x00E8; // è
                    if (baseChar == 'i') return 0x00EC; // ì
                    if (baseChar == 'o') return 0x00F2; // ò
                    if (baseChar == 'u') return 0x00F9; // ù
                    if (baseChar == 'y') return 0x1EF3; // ỳ
                    break;
                // Add more tone mark implementations
            }
            return baseChar;
        }

    } // namespace Engine
} // namespace EndKey