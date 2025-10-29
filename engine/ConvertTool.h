//
//  ConvertTool.h
//  EndKey
//
//  Created by Refactoring on 10/26/25.
//  Copyright © 2025 EndKey. All rights reserved.
//

#ifndef ConvertTool_h
#define ConvertTool_h

#include "DataType.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>

namespace EndKey {
    namespace Engine {

        /**
         * ConvertTool handles Unicode and encoding conversions
         * Provides efficient character mapping and code table conversions
         */
        class ConvertTool {
        public:
            enum class CodeTable {
                // ENCODING REMOVAL: Unicode-only (0) - legacy options kept for compatibility
                Unicode = 0,
                TCVN3 = 1,   // Deprecated
                VNI_Windows = 2  // Deprecated
            };

            struct ConversionConfig {
                // ENCODING REMOVAL: Force Unicode-only
                CodeTable sourceTable = CodeTable::Unicode; // Always Unicode
                CodeTable targetTable = CodeTable::Unicode; // Always Unicode
                bool rememberCodePerApp = false;
                bool fixRecommendBrowser = true;
                bool dontAlertWhenCompleted = false;
                bool toAllCaps = false;
                bool toAllNonCaps = false;
                bool toCapsFirstLetter = false;
                bool toCapsEachWord = false;
                bool removeMark = false;
                int hotKey = 0;
            };

        public:
            ConvertTool();
            ~ConvertTool();

            // Configuration
            void setConversionConfig(const ConversionConfig& config);
            const ConversionConfig& getConversionConfig() const;

            // Core conversion operations (ENCODING REMOVAL: Unicode-only)
            Uint16 convertCharacter(Uint16 character, CodeTable from, CodeTable to) const; // Ignores from/to, returns Unicode
            std::vector<Uint16> convertText(const std::vector<Uint16>& text,
                                          CodeTable from, CodeTable to) const; // Ignores from/to, returns Unicode
            std::string convertToUtf8(const std::vector<Uint16>& text, CodeTable source) const; // Ignores source
            std::vector<Uint16> convertFromUtf8(const std::string& text, CodeTable target) const; // Ignores target

            // Character code mappings
            Uint32 getCharacterCode(const Uint32& data) const;
            Uint16 keyCodeToCharacter(const Uint32& keyCode) const;

            // Code table management (ENCODING REMOVAL: Unicode-only)
            void setCodeTable(CodeTable table); // Ignores input, always sets Unicode
            CodeTable getCurrentCodeTable() const; // Always returns Unicode
            bool hasCodeTable(CodeTable table) const; // Only returns true for Unicode

            // Batch conversion (ENCODING REMOVAL: Unicode-only)
            void convertInPlace(std::vector<Uint16>& text, CodeTable from, CodeTable to) const; // Ignores from/to
            std::vector<std::vector<Uint16>> convertBatch(
                const std::vector<std::vector<Uint16>>& texts,
                CodeTable from, CodeTable to) const; // Ignores from/to

            // Performance optimization
            void preloadConversionTables();
            void optimizeForCodeTable(CodeTable table);
            void clearCache();

            // Character analysis
            bool isVietnameseCharacter(Uint16 character) const;
            bool isToneMark(Uint16 character) const;
            bool isCombiningCharacter(Uint16 character) const;

            // Statistics
            struct ConversionStats {
                size_t totalConversions = 0;
                size_t cacheHits = 0;
                size_t cacheMisses = 0;
                std::unordered_map<CodeTable, size_t> tableUsage;
                double averageConversionTime = 0.0;
            };

            const ConversionStats& getStatistics() const;
            void resetStatistics();

            // Memory management
            size_t getMemoryUsage() const;
            void compactMemory();

        private:
            ConversionConfig config_;
            mutable ConversionStats stats_;

            // Conversion tables (lazy loaded)
            mutable std::unique_ptr<std::unordered_map<Uint16, std::vector<Uint16>>> codeTables_[3];
            mutable std::unique_ptr<std::unordered_map<Uint32, Uint32>> characterMaps_;
            mutable bool tablesLoaded_ = false;

            // Caching
            mutable std::unordered_map<Uint64, Uint16> conversionCache_;
            static constexpr size_t MAX_CACHE_SIZE = 20000;

            // Unicode compound mark mappings
            static constexpr Uint16 UNICODE_COMPOUND_MARK[] = {
                // Tone marks
                0x0300, 0x0301, 0x0302, 0x0303, 0x0309,
                // Breve and horn
                0x0306, 0x031B,
                // Dot below
                0x0323
            };

            // Helper methods
            void loadConversionTables() const;
            void loadUnicodeTable();
            void loadTCVN3Table();
            void loadVNIWindowsTable();

            // Character classification
            bool isLatinCharacter(Uint16 character) const;
            bool isCombiningDiacritic(Uint16 character) const;

            // Cache management
            void evictOldestCacheEntries();
            Uint64 makeCacheKey(Uint16 character, CodeTable from, CodeTable to) const;

            // Performance optimization
            void optimizeCacheForCodeTable(CodeTable table);
            void preloadFrequentCharacters(CodeTable table);

            // Memory optimization
            void cleanupUnusedTables();
            void compactConversionTables();
        };

    } // namespace Engine
} // namespace EndKey

// Legacy compatibility
extern bool convertToolDontAlertWhenCompleted;
extern bool convertToolToAllCaps;
extern bool convertToolToAllNonCaps;
extern bool convertToolToCapsFirstLetter;
extern bool convertToolToCapsEachWord;
extern bool convertToolRemoveMark;
extern Uint8 convertToolFromCode;
extern Uint8 convertToolToCode;
extern int convertToolHotKey;

std::string convertUtil(const std::string& sourceString);

#endif /* ConvertTool_h */
