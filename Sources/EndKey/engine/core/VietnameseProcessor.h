//
//  VietnameseProcessor.h
//  EndKey
//
//  Created by Hive Coder Agent on 10/26/25.
//  Copyright © 2025 EndKey. All rights reserved.
//

#ifndef VietnameseProcessor_h
#define VietnameseProcessor_h

#include "interfaces/IVietnameseProcessor.h"
#include "interfaces/IConfigurationManager.h"
#include <memory>
#include <map>
#include <vector>

namespace EndKey {
    namespace Core {
        /**
         * Vietnamese Language Processor Implementation
         * Handles Vietnamese character processing, tone marks, and spelling
         */
        class VietnameseProcessor : public IVietnameseProcessor {
        private:
            IConfigurationManager* configManager;

            // Vietnamese character data structures
            std::map<Uint16, std::vector<std::vector<Uint16>>> vowelMap;
            std::map<Uint16, std::vector<std::vector<Uint32>>> vowelCombineMap;
            std::map<Uint16, std::vector<std::vector<Uint16>>> vowelForMarkMap;
            std::vector<std::vector<Uint16>> consonantDMap;
            std::vector<std::vector<Uint16>> consonantTableMap;
            std::vector<std::vector<Uint16>> endConsonantTableMap;
            std::vector<Uint16> standaloneWBad;
            std::vector<std::vector<Uint16>> doubleWAllowed;
            std::map<Uint32, std::vector<Uint16>> codeTableMap[4];
            std::map<Uint32, std::vector<Uint16>> quickTelexMap;
            std::map<Uint16, std::vector<Uint16>> quickStartConsonantMap;
            std::map<Uint16, std::vector<Uint16>> quickEndConsonantMap;
            std::map<Uint32, Uint32> characterMap;

            vKeyInputType currentInputType;
            bool spellCheckingEnabled;
            bool modernOrthographyEnabled;

            // Processing state
            std::vector<Uint32> processingBuffer;
            int currentIndex;

            // Private methods for Vietnamese processing
            void initializeVietnameseData();
            void initializeCharacterMaps();
            void initializeToneMarkTables();
            void initializeQuickTelexTables();

            HoolCodeState processToneMark(const Uint16& keyCode);
            HoolCodeState processCharacterMark(const Uint16& keyCode);
            HoolCodeState processQuickTelex(const Uint16& keyCode);
            HoolCodeState processQuickStartConsonant(const Uint16& keyCode);
            HoolCodeState processQuickEndConsonant(const Uint16& keyCode);

            bool findAndReplaceCharacter(int& index, const Uint32& newChar);
            bool findAndReplaceCharacterMark(int& index, const Uint32& newChar);
            bool canInsertCharacter(const Uint32& newChar);

            bool isValidVietnameseWord(const std::vector<Uint32>& buffer);
            void applyModernOrthographyToBuffer(std::vector<Uint32>& buffer);

            Uint32 applyToneMark(Uint32 charData, int toneType);
            Uint32 applyCharacterMark(Uint32 charData, int markType);

            bool isVowel(const Uint16& keyCode);
            bool isConsonant(const Uint16& keyCode);
            bool isToneKey(const Uint16& keyCode, vKeyInputType inputType);
            bool isCharacterMarkKey(const Uint16& keyCode);
            bool canProcessDoubleConsonant(const Uint16& keyCode1, const Uint16& keyCode2);

            std::vector<Uint32> convertToUnicode(const std::vector<Uint32>& buffer);

        public:
            explicit VietnameseProcessor(IConfigurationManager* configMgr);
            virtual ~VietnameseProcessor() = default;

            // IVietnameseProcessor interface
            HoolCodeState processCharacter(
                const Uint16& keyCode,
                std::vector<Uint32>& currentBuffer,
                const vKeyInputType& inputType
            ) override;

            Uint32 convertToCharacter(const Uint32& data) override;
            bool checkSpelling(const std::vector<Uint32>& buffer) override;
            void applyModernOrthography(std::vector<Uint32>& buffer) override;

            void setInputType(const vKeyInputType& type) override;
            void setSpellCheckingEnabled(bool enabled) override;
            void setModernOrthographyEnabled(bool enabled) override;

            // Additional Vietnamese-specific methods
            void reset();
            void setCurrentBuffer(const std::vector<Uint32>& buffer);
            const std::vector<Uint32>& getCurrentBuffer() const;

            // Utility methods
            static std::wstring utf8ToWideString(const std::string& str);
            static std::string wideStringToUtf8(const std::wstring& str);
        };
    }
}

#endif /* VietnameseProcessor_h */