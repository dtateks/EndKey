//
//  VietnameseOptimized.h
//  EndKey
//
//  Created by Refactoring on 10/26/25.
//  Copyright © 2025 EndKey. All rights reserved.
//

#ifndef VietnameseOptimized_h
#define VietnameseOptimized_h

#include "DataType.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <array>

namespace EndKey {
    namespace Engine {

        /**
         * Optimized Vietnamese character processing with efficient data structures
         * Replaces hardcoded maps with lazy-loaded cached classes
         */
        class VietnameseOptimized {
        public:
            // Vowel and consonant structure definitions
            struct VowelSequence {
                std::vector<Uint16> sequence;
                bool isValid = false;
                Uint16 primaryVowel = 0;
                Uint8 tonePosition = 0;
            };

            struct ConsonantPair {
                Uint16 first;
                Uint16 second;
                bool isValid = false;
                bool isStartPair = true;
            };

            struct CharacterInfo {
                Uint16 baseChar;
                Uint16 combinedChar;
                Uint8 toneType;      // 0: none, 1-5: tone marks
                Uint8 markType;      // 0: none, 1-6: diacritic marks
                bool isVowel = false;
                bool isConsonant = false;
                bool canTakeTone = false;
                bool canTakeMark = false;
            };

        public:
            VietnameseOptimized();
            ~VietnameseOptimized();

            // Character lookup and conversion
            CharacterInfo getCharacterInfo(Uint16 character) const;
            Uint16 getVowelWithMark(Uint16 baseChar, Uint8 markType) const;
            Uint16 getVowelWithTone(Uint16 baseChar, Uint8 toneType) const;
            Uint16 getCombinedCharacter(Uint16 baseChar, Uint8 markType, Uint8 toneType) const;

            // Vowel sequence analysis
            VowelSequence analyzeVowelSequence(const std::vector<Uint16>& characters) const;
            bool isValidVowelSequence(const std::vector<Uint16>& characters) const;
            Uint16 findPrimaryVowel(const std::vector<Uint16>& characters) const;
            Uint8 findTonePosition(const std::vector<Uint16>& characters) const;

            // Consonant validation
            bool isValidStartConsonant(Uint16 consonant) const;
            bool isValidEndConsonant(Uint16 consonant) const;
            bool isValidConsonantPair(Uint16 first, Uint16 second) const;
            std::vector<ConsonantPair> getValidConsonantPairs() const;

            // Word validation
            bool isValidVietnameseWord(const std::vector<Uint16>& word) const;
            bool hasValidSyllableStructure(const std::vector<Uint16>& word) const;

            // Double key processing (â, ê, ô, etc.)
            Uint16 processDoubleKey(Uint16 baseKey) const;
            bool isDoubleKeyCharacter(Uint16 character) const;

            // Quick consonant processing
            std::vector<Uint16> processQuickStartConsonant(Uint16 input) const;
            std::vector<Uint16> processQuickEndConsonant(Uint16 input) const;

            // Performance optimization
            void preloadCommonCharacters();
            void preloadCommonWords();
            void optimizeForInputMethod(int inputType);
            void clearCache();

            // Statistics
            struct ProcessingStats {
                size_t characterLookups = 0;
                size_t cacheHits = 0;
                size_t cacheMisses = 0;
                size_t vowelSequencesAnalyzed = 0;
                size_t wordsValidated = 0;
                double averageLookupTime = 0.0;
            };

            const ProcessingStats& getStatistics() const;
            void resetStatistics();

            // Memory management
            size_t getMemoryUsage() const;
            size_t getCacheSize() const;
            void compactMemory();

        private:
            // Data structures for character mappings
            mutable std::unique_ptr<std::unordered_map<Uint16, CharacterInfo>> characterMap_;
            mutable std::unique_ptr<std::unordered_set<Uint16>> vowelSet_;
            mutable std::unique_ptr<std::unordered_set<Uint16>> consonantSet_;
            mutable std::unique_ptr<std::unordered_map<Uint16, std::vector<VowelSequence>>> vowelSequences_;
            mutable std::unique_ptr<std::unordered_map<Uint16, std::vector<ConsonantPair>>> consonantPairs_;

            // Caching
            mutable std::unordered_map<Uint64, CharacterInfo> characterCache_;
            mutable std::unordered_map<Uint64, VowelSequence> vowelSequenceCache_;
            mutable std::unordered_map<Uint32, bool> wordValidationCache_;

            static constexpr size_t MAX_CHARACTER_CACHE = 50000;
            static constexpr size_t MAX_SEQUENCE_CACHE = 10000;
            static constexpr size_t MAX_WORD_CACHE = 20000;

            // Performance tracking
            mutable ProcessingStats stats_;
            mutable bool tablesLoaded_ = false;

            // Double key mappings
            static constexpr std::array<std::pair<Uint16, Uint16>, 2> DOUBLE_KEYS = {{
                {KEY_A, 0xE2},  // a -> â
                {KEY_E, 0xEA}   // e -> ê
            }};

            // Helper methods
            void loadCharacterTables() const;
            void loadVowelSequences() const;
            void loadConsonantPairs() const;
            void loadQuickConsonantMappings() const;

            // Cache management
            void evictOldestCharacterCache();
            void evictOldestSequenceCache();
            void evictOldestWordCache();

            // Hash functions for caching
            Uint64 makeCharacterCacheKey(Uint16 character, Uint8 markType, Uint8 toneType) const;
            Uint64 makeSequenceCacheKey(const std::vector<Uint16>& sequence) const;
            Uint32 makeWordCacheKey(const std::vector<Uint16>& word) const;

            // Lazy loading optimization
            void loadFrequentlyUsedCharacters() const;
            void loadCommonVowelSequences() const;
            void loadCommonConsonantPairs() const;

            // Memory optimization
            void cleanupUnusedData();
            void optimizeCacheHitRatio();
            void compactDataStructures();

            // Character analysis helpers
            bool isVowelCharacter(Uint16 character) const;
            bool isConsonantCharacter(Uint16 character) const;
            Uint8 getCharacterToneType(Uint16 character) const;
            Uint8 getCharacterMarkType(Uint16 character) const;

            // Quick consonant mappings (lazy loaded)
            mutable std::unique_ptr<std::unordered_map<Uint16, std::vector<Uint16>>> quickStartConsonant_;
            mutable std::unique_ptr<std::unordered_map<Uint16, std::vector<Uint16>>> quickEndConsonant_;
        };

    } // namespace Engine
} // namespace EndKey

// Legacy compatibility with original Vietnamese.h
extern std::unique_ptr<EndKey::Engine::VietnameseOptimized> gVietnameseOptimized;

// Legacy function wrappers
inline Uint16 keyCodeToCharacter(const Uint32& keyCode) {
    if (!gVietnameseOptimized) {
        gVietnameseOptimized = std::make_unique<EndKey::Engine::VietnameseOptimized>();
    }
    auto info = gVietnameseOptimized->getCharacterInfo(static_cast<Uint16>(keyCode));
    return info.combinedChar ? info.combinedChar : info.baseChar;
}

#endif /* VietnameseOptimized_h */