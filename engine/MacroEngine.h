//
//  MacroEngine.h
//  EndKey
//
//  Created by Refactoring on 10/26/25.
//  Copyright © 2025 EndKey. All rights reserved.
//

#ifndef MacroEngine_h
#define MacroEngine_h

#include "DataType.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>

namespace EndKey {
    namespace Engine {

        /**
         * MacroEngine handles text expansion and auto-capitalization
         * Provides efficient macro lookup and case conversion
         */
        class MacroEngine {
        public:
            struct MacroConfig {
                bool useMacro = true;
                bool useMacroInEnglishMode = false;
                bool autoCapsMacro = true;
                bool skipMacroNextBreak = false;
                bool forceSkipMacroNextBreak = false;
            };

            struct MacroEntry {
                std::string keyword;
                std::string expansion;
                bool isActive = true;
                uint32_t usageCount = 0;
                double averageCaseRatio = 0.0;  // Track capitalization patterns
            };

            struct MacroMatch {
                std::string keyword;
                std::string expansion;
                bool shouldCapitalize = false;
                bool shouldAllCaps = false;
                bool found = false;
            };

        public:
            MacroEngine();
            ~MacroEngine();

            // Configuration
            void setMacroConfig(const MacroConfig& config);
            const MacroConfig& getMacroConfig() const;

            // Core macro operations
            MacroMatch findMacro(const std::string& input) const;
            bool shouldExpandMacro(const std::string& word) const;
            std::string applyAutoCapitalization(const std::string& text,
                                              const std::string& originalCase) const;

            // Macro management
            void addMacro(const std::string& keyword, const std::string& expansion);
            void removeMacro(const std::string& keyword);
            void updateMacro(const std::string& keyword, const std::string& expansion);
            bool hasMacro(const std::string& keyword) const;

            // Bulk operations
            void loadMacros(const std::unordered_map<std::string, std::string>& macros);
            std::unordered_map<std::string, std::string> getAllMacros() const;
            void clearAllMacros();

            // Character processing
            std::vector<Uint16> processMacroExpansion(const std::string& word,
                                                     bool isFirstWord = false) const;

            // Performance optimization
            void preloadFrequentMacros();
            void optimizeMacroOrder();
            void clearCache();

            // Statistics and analytics
            struct MacroStats {
                size_t totalMacros = 0;
                size_t activeMacros = 0;
                size_t expansionsPerformed = 0;
                size_t cacheHits = 0;
                std::vector<std::pair<std::string, uint32_t>> mostUsedMacros;
            };

            const MacroStats& getStatistics() const;
            void resetStatistics();

            // Memory management
            size_t getMemoryUsage() const;
            void compactMemory();

        private:
            MacroConfig config_;
            mutable MacroStats stats_;

            // Macro storage with efficient lookup
            std::unordered_map<std::string, std::unique_ptr<MacroEntry>> macros_;

            // Trie structure for efficient prefix matching
            struct TrieNode {
                std::unordered_map<char, std::unique_ptr<TrieNode>> children;
                MacroEntry* macroEntry = nullptr;
                bool isEndOfWord = false;
            };

            std::unique_ptr<TrieNode> macroTrie_;

            // Caching
            mutable std::unordered_map<std::string, MacroMatch> macroCache_;
            static constexpr size_t MAX_CACHE_SIZE = 5000;

            // Case analysis
            enum class CaseType {
                lowercase,
                uppercase,
                capitalized,
                mixed_case
            };

            // Helper methods
            void buildTrie();
            MacroMatch searchTrie(const std::string& input) const;
            CaseType analyzeCase(const std::string& text) const;
            std::string applyCasePattern(const std::string& text, CaseType pattern) const;

            // Cache management
            void evictOldestCacheEntries();
            void updateMacroStatistics(const std::string& keyword, const std::string& originalCase);

            // Optimization
            void sortMacrosByFrequency();
            void preloadCommonPatterns();

            // Memory optimization
            void cleanupUnusedMacros();
            void compactTrie();
        };

    } // namespace Engine
} // namespace EndKey

#endif /* MacroEngine_h */