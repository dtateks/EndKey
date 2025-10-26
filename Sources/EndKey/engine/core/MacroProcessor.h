//
//  MacroProcessor.h
//  EndKey
//
//  Created by Hive Coder Agent on 10/26/25.
//  Copyright © 2025 EndKey. All rights reserved.
//

#ifndef MacroProcessor_h
#define MacroProcessor_h

#include "interfaces/IMacroProcessor.h"
#include "interfaces/IConfigurationManager.h"
#include <memory>
#include <map>
#include <string>
#include <vector>

namespace EndKey {
    namespace Core {
        /**
         * Macro Processor Implementation
         * Handles text expansion and macro replacement
         */
        class MacroProcessor : public IMacroProcessor {
        private:
            IConfigurationManager* configManager;

            // Macro storage
            std::map<std::string, std::string> macros;
            std::string macroStoragePath;

            // Settings
            bool macroEnabled;
            bool macroInEnglishModeEnabled;
            bool autoCapsEnabled;

            // State management
            bool skipNextMacroFlag;
            bool forceSkipNextMacroFlag;

            // Helper methods
            std::string bufferToString(const std::vector<Uint32>& buffer) const;
            std::vector<Uint32> stringToBuffer(const std::string& str) const;
            std::string capitalizeMacro(const std::string& macroText, bool allCaps = false) const;
            bool isWordBreak(const Uint16& keyCode) const;
            bool shouldSkipMacro() const;
            void resetSkipFlags();

            std::string getDefaultMacroPath() const;
            bool loadMacrosFromFile(const std::string& filePath);
            bool saveMacrosToFile(const std::string& filePath);
            void initializeDefaultMacros();

        public:
            explicit MacroProcessor(IConfigurationManager* configMgr);
            virtual ~MacroProcessor() = default;

            // IMacroProcessor interface
            std::string findMacro(const std::vector<Uint32>& buffer) override;
            std::string expandMacro(const std::string& macroKey, bool isCaps = false) override;
            bool addMacro(const std::string& key, const std::string& value) override;
            bool removeMacro(const std::string& key) override;
            bool loadMacros() override;
            bool saveMacros() override;

            void setMacroEnabled(bool enabled) override;
            void setMacroInEnglishModeEnabled(bool enabled) override;
            void setAutoCapsEnabled(bool enabled) override;

            bool isMacroEnabled() const override;
            void skipNextMacro() override;
            void forceSkipNextMacro() override;

            // Additional macro-specific methods
            std::vector<std::string> getAllMacroKeys() const;
            bool hasMacro(const std::string& key) const;
            std::string getMacroValue(const std::string& key) const;
            size_t getMacroCount() const;
            void clearAllMacros();

            // Export/Import functionality
            bool exportMacros(const std::string& filePath);
            bool importMacros(const std::string& filePath, bool merge = false);

            // Macro validation
            bool isValidMacroKey(const std::string& key) const;
            bool isValidMacroValue(const std::string& value) const;
        };
    }
}

#endif /* MacroProcessor_h */