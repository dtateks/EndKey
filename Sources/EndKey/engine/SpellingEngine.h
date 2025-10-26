//
//  SpellingEngine.h
//  EndKey
//
//  Created by Refactoring on 10/26/25.
//  Copyright © 2025 EndKey. All rights reserved.
//

#ifndef SpellingEngine_h
#define SpellingEngine_h

#include "DataType.h"
#include <vector>
#include <unordered_map>
#include <memory>

namespace EndKey {
    namespace Engine {

        /**
         * SpellingEngine handles Vietnamese orthography and tone mark rules
         * Provides efficient character conversion and spelling validation
         */
        class SpellingEngine {
        public:
            struct SpellingConfig {
                bool checkSpelling = true;
                bool useModernOrthography = true;
                bool allowConsonantZFWJ = false;
                bool quickStartConsonant = false;
                bool quickEndConsonant = false;
                bool restoreIfWrongSpelling = true;
                bool tempOffSpelling = false;
            };

            struct ToneMarkInfo {
                Uint16 baseChar;
                Uint16 toneChar;
                Uint8 toneType;  // 0: none, 1: acute, 2: grave, 3: hook, 4: tilde, 5: dot
                bool isValid;
            };

        public:
            SpellingEngine();
            ~SpellingEngine();

            // Configuration
            void setSpellingConfig(const SpellingConfig& config);
            const SpellingConfig& getSpellingConfig() const;

            // Core spelling operations
            bool isValidVietnameseWord(const std::vector<Uint16>& word) const;
            ToneMarkInfo applyToneMark(Uint16 character, Uint8 toneType) const;
            Uint16 findVowelForMark(const std::vector<Uint16>& word, Uint8 markType) const;

            // Character conversion
            Uint16 convertCharacter(Uint16 input, Uint8 toneType) const;
            std::vector<Uint16> convertWord(const std::vector<Uint16>& word) const;

            // Vowel and consonant detection
            bool isVowel(Uint16 character) const;
            bool isConsonant(Uint16 character) const;
            bool isValidConsonantPair(Uint16 first, Uint16 second) const;

            // Modern orthography
            bool shouldUseModernOrthography(const std::vector<Uint16>& word) const;
            void applyModernOrthography(std::vector<Uint16>& word) const;

            // Performance optimization
            void preloadCommonWords();
            void clearCache();
            size_t getCacheSize() const;

            // Statistics
            struct SpellingStats {
                size_t totalWordsChecked = 0;
                size_t invalidWordsDetected = 0;
                size_t cacheHits = 0;
                size_t cacheMisses = 0;
            };

            const SpellingStats& getStatistics() const;
            void resetStatistics();

        private:
            SpellingConfig config_;
            mutable SpellingStats stats_;

            // Caching
            mutable std::unordered_map<Uint64, bool> wordValidityCache_;
            mutable std::unordered_map<Uint32, ToneMarkInfo> toneMarkCache_;
            static constexpr size_t MAX_CACHE_SIZE = 10000;

            // Character classification tables (lazy loaded)
            mutable std::unique_ptr<std::unordered_set<Uint16>> vowelSet_;
            mutable std::unique_ptr<std::unordered_set<Uint16>> consonantSet_;
            mutable bool tablesLoaded_ = false;

            // Quick consonant mappings
            std::unordered_map<Uint16, std::vector<Uint16>> quickStartConsonant_;
            std::unordered_map<Uint16, std::vector<Uint16>> quickEndConsonant_;

            // Helper methods
            void loadCharacterTables() const;
            bool isValidVowelSequence(const std::vector<Uint16>& vowels) const;
            bool isValidConsonantSequence(const std::vector<Uint16>& consonants) const;
            Uint8 detectToneType(Uint16 character) const;

            // Cache management
            void evictOldestCacheEntries();
            bool isCacheFull() const;

            // Quick consonant processing
            std::vector<Uint16> applyQuickStartConsonant(Uint16 character) const;
            std::vector<Uint16> applyQuickEndConsonant(Uint16 character) const;
        };

    } // namespace Engine
} // namespace EndKey

#endif /* SpellingEngine_h */