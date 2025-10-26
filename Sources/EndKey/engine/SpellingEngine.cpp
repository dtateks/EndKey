//
//  SpellingEngine.cpp
//  EndKey
//
//  Created by Refactoring on 10/26/25.
//  Copyright © 2025 EndKey. All rights reserved.
//

#include "SpellingEngine.h"
#include "VietnameseOptimized.h"
#include <chrono>
#include <algorithm>

namespace EndKey {
    namespace Engine {

        SpellingEngine::SpellingEngine() {
            wordValidityCache_.reserve(MAX_CACHE_SIZE / 2);
            toneMarkCache_.reserve(MAX_CACHE_SIZE / 2);
        }

        SpellingEngine::~SpellingEngine() {
            // Smart pointers handle cleanup
        }

        void SpellingEngine::setSpellingConfig(const SpellingConfig& config) {
            config_ = config;
            if (config_.checkSpelling) {
                preloadCommonWords();
            }
        }

        const SpellingEngine::SpellingConfig& SpellingEngine::getSpellingConfig() const {
            return config_;
        }

        bool SpellingEngine::isValidVietnameseWord(const std::vector<Uint16>& word) const {
            if (!config_.checkSpelling || config_.tempOffSpelling) {
                return true;
            }

            if (word.empty()) {
                return true;
            }

            stats_.totalWordsChecked++;

            // Check cache first
            Uint32 cacheKey = 0;
            for (size_t i = 0; i < std::min(word.size(), size_t(4)); ++i) {
                cacheKey = (cacheKey << 16) | word[i];
            }

            auto cacheIt = wordValidityCache_.find(cacheKey);
            if (cacheIt != wordValidityCache_.end()) {
                stats_.cacheHits++;
                return cacheIt->second;
            }

            stats_.cacheMisses++;

            // Validate word structure
            bool isValid = validateWordStructure(word);

            // Cache the result
            if (isCacheFull()) {
                evictOldestCacheEntries();
            }
            wordValidityCache_[cacheKey] = isValid;

            if (!isValid) {
                stats_.invalidWordsDetected++;
            }

            return isValid;
        }

        bool SpellingEngine::validateWordStructure(const std::vector<Uint16>& word) const {
            if (!tablesLoaded_) {
                loadCharacterTables();
            }

            // Single character words
            if (word.size() == 1) {
                Uint16 charCode = word[0];
                return isVowel(charCode) || isConsonant(charCode);
            }

            // Extract vowels and consonants
            std::vector<Uint16> vowels;
            std::vector<Uint16> consonants;

            for (Uint16 charCode : word) {
                if (isVowel(charCode)) {
                    vowels.push_back(charCode);
                } else if (isConsonant(charCode)) {
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

            // Validate consonant sequences
            if (!isValidConsonantSequence(consonants)) {
                return false;
            }

            return true;
        }

        SpellingEngine::ToneMarkInfo SpellingEngine::applyToneMark(Uint16 character, Uint8 toneType) const {
            ToneMarkInfo info = {character, character, 0, false};

            // Check cache first
            Uint32 cacheKey = (static_cast<Uint32>(character) << 8) | toneType;
            auto cacheIt = toneMarkCache_.find(cacheKey);
            if (cacheIt != toneMarkCache_.end()) {
                return cacheIt->second;
            }

            // Use VietnameseOptimized for character processing
            if (!gVietnameseOptimized) {
                gVietnameseOptimized = std::make_unique<VietnameseOptimized>();
            }

            auto charInfo = gVietnameseOptimized->getCharacterInfo(character);
            if (charInfo.canTakeTone && toneType > 0 && toneType <= 5) {
                info.toneChar = gVietnameseOptimized->getVowelWithTone(character, toneType);
                info.toneType = toneType;
                info.isValid = (info.toneChar != character);
            }

            // Cache the result
            if (isCacheFull()) {
                evictOldestCacheEntries();
            }
            toneMarkCache_[cacheKey] = info;

            return info;
        }

        Uint16 SpellingEngine::findVowelForMark(const std::vector<Uint16>& word, Uint8 markType) const {
            if (!gVietnameseOptimized) {
                gVietnameseOptimized = std::make_unique<VietnameseOptimized>();
            }

            auto sequence = gVietnameseOptimized->analyzeVowelSequence(word);
            if (sequence.isValid && markType > 0) {
                return gVietnameseOptimized->getVowelWithMark(sequence.primaryVowel, markType);
            }

            return 0;
        }

        Uint16 SpellingEngine::convertCharacter(Uint16 input, Uint8 toneType) const {
            auto toneInfo = applyToneMark(input, toneType);
            return toneInfo.isValid ? toneInfo.toneChar : input;
        }

        std::vector<Uint16> SpellingEngine::convertWord(const std::vector<Uint16>& word) const {
            std::vector<Uint16> result;
            result.reserve(word.size());

            for (Uint16 character : word) {
                // Apply any pending tone marks or character conversions
                Uint8 toneType = detectToneType(character);
                Uint16 converted = convertCharacter(character, toneType);
                result.push_back(converted);
            }

            return result;
        }

        bool SpellingEngine::isVowel(Uint16 character) const {
            if (!tablesLoaded_) {
                loadCharacterTables();
            }
            return vowelSet_->find(character) != vowelSet_->end();
        }

        bool SpellingEngine::isConsonant(Uint16 character) const {
            if (!tablesLoaded_) {
                loadCharacterTables();
            }
            return consonantSet_->find(character) != consonantSet_->end();
        }

        bool SpellingEngine::isValidConsonantPair(Uint16 first, Uint16 second) const {
            if (!gVietnameseOptimized) {
                gVietnameseOptimized = std::make_unique<VietnameseOptimized>();
            }

            auto pairs = gVietnameseOptimized->getValidConsonantPairs();
            for (const auto& pair : pairs) {
                if (pair.first == first && pair.second == second && pair.isValid) {
                    return true;
                }
            }
            return false;
        }

        bool SpellingEngine::shouldUseModernOrthography(const std::vector<Uint16>& word) const {
            return config_.useModernOrthography;
        }

        void SpellingEngine::applyModernOrthography(std::vector<Uint16>& word) const {
            if (!config_.useModernOrthography) {
                return;
            }

            // Apply modern orthography rules
            // This would contain specific rules for modern Vietnamese spelling
        }

        void SpellingEngine::preloadCommonWords() {
            // Preload frequently used Vietnamese words
            std::vector<std::vector<Uint16>> commonWords = {
                {L'v', L'i', L'ệ', L't'},         // việt
                {L'n', L'a', L'm'},               // nam
                {L't', L'r', L'o', L'ń', L'g'},    // người
                {L'g', L'i', L'à'},               // gia
                {L'n', L'h', L'à'}                // nhà
            };

            for (const auto& word : commonWords) {
                isValidVietnameseWord(word);
            }
        }

        void SpellingEngine::clearCache() {
            wordValidityCache_.clear();
            toneMarkCache_.clear();
            stats_.cacheHits = 0;
            stats_.cacheMisses = 0;
        }

        size_t SpellingEngine::getCacheSize() const {
            return wordValidityCache_.size() + toneMarkCache_.size();
        }

        const SpellingEngine::SpellingStats& SpellingEngine::getStatistics() const {
            return stats_;
        }

        void SpellingEngine::resetStatistics() {
            stats_ = SpellingStats{};
        }

        void SpellingEngine::loadCharacterTables() const {
            if (tablesLoaded_) {
                return;
            }

            vowelSet_ = std::make_unique<std::unordered_set<Uint16>>();
            consonantSet_ = std::make_unique<std::unordered_set<Uint16>>();

            // Load vowel characters
            std::vector<Uint16> vowels = {
                // Base vowels
                L'a', L'e', L'i', L'o', L'u', L'y',
                // Vietnamese vowels with diacritics
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
                // Base consonants
                L'b', L'c', L'd', L'g', L'h', L'k', L'l', L'm', L'n', L'p', L'q', L'r', L's', L't', L'v', L'x',
                // Capital consonants
                L'B', L'C', L'D', L'G', L'H', L'K', L'L', L'M', L'N', L'P', L'Q', L'R', L'S', L'T', L'V', L'X',
                // Special consonants
                L'Đ', L'f', L'j', L'w', L'z', L'F', L'J', L'W', L'Z'
            };

            for (Uint16 consonant : consonants) {
                consonantSet_->insert(consonant);
            }

            tablesLoaded_ = true;
        }

        bool SpellingEngine::isValidVowelSequence(const std::vector<Uint16>& vowels) const {
            if (vowels.empty() || vowels.size() > 3) {
                return false;
            }

            // Basic vowel sequence validation
            // More complex rules would go here
            return true;
        }

        bool SpellingEngine::isValidConsonantSequence(const std::vector<Uint16>& consonants) const {
            // Vietnamese words can have specific consonant patterns
            // This would contain detailed consonant sequence rules
            return true;
        }

        Uint8 SpellingEngine::detectToneType(Uint16 character) const {
            // Detect tone mark type from character
            // Returns 0-5 for different tone types
            return 0;
        }

        void SpellingEngine::evictOldestCacheEntries() {
            // Simple LRU eviction - remove oldest 25% of entries
            size_t entriesToRemove = std::max(wordValidityCache_.size(), toneMarkCache_.size()) / 4;

            if (wordValidityCache_.size() > MAX_CACHE_SIZE / 2) {
                auto it = wordValidityCache_.begin();
                std::advance(it, std::min(entriesToRemove, wordValidityCache_.size()));
                wordValidityCache_.erase(wordValidityCache_.begin(), it);
            }

            if (toneMarkCache_.size() > MAX_CACHE_SIZE / 2) {
                auto it = toneMarkCache_.begin();
                std::advance(it, std::min(entriesToRemove, toneMarkCache_.size()));
                toneMarkCache_.erase(toneMarkCache_.begin(), it);
            }
        }

        bool SpellingEngine::isCacheFull() const {
            return wordValidityCache_.size() + toneMarkCache_.size() >= MAX_CACHE_SIZE;
        }

    } // namespace Engine
} // namespace EndKey