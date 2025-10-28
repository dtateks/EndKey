//
//  ConfigurationManager.h
//  EndKey
//
//  Created by Hive Coder Agent on 10/26/25.
//  Copyright © 2025 EndKey. All rights reserved.
//

#ifndef ConfigurationManager_h
#define ConfigurationManager_h

#include "interfaces/IConfigurationManager.h"
#include <memory>
#include <map>

namespace EndKey {
    namespace Core {
        /**
         * Configuration Manager Implementation
         * Centralizes all settings and provides persistence
         */
        class ConfigurationManager : public IConfigurationManager {
        private:
            // Language and Input
            int language;
            vKeyInputType inputType;
            int codeTable;

            // Vietnamese Processing
            bool freeMarkEnabled;
            bool spellCheckingEnabled;
            bool modernOrthographyEnabled;
            bool quickTelexEnabled;
            bool restoreIfWrongSpellingEnabled;

            // Macro Settings
            bool macroEnabled;
            bool macroInEnglishModeEnabled;
            bool autoCapsMacroEnabled;

            // Smart Switch Settings
            bool smartSwitchKeyEnabled;
            bool rememberCodeEnabled;

            // Typing Settings
            bool upperCaseFirstCharEnabled;
            bool doubleSpacePeriodEnabled;
            bool tempOffEndKeyEnabled;

            // Advanced Settings
            bool fixRecommendBrowserEnabled;
            bool tempOffSpellingEnabled;
            bool allowConsonantZFWJEnabled;
            bool quickStartConsonantEnabled;
            bool quickEndConsonantEnabled;
            bool otherLanguageEnabled;

            // State Management
            int switchKeyStatus;
            bool englishMode;

            // Application-specific settings
            std::map<std::string, std::map<std::string, std::string>> applicationSettings;

        public:
            ConfigurationManager();
            virtual ~ConfigurationManager() = default;

            // Language and Input Settings
            void setLanguage(int language) override;
            int getLanguage() const override;

            void setInputType(vKeyInputType type) override;
            vKeyInputType getInputType() const override;

            void setCodeTable(int codeTable) override;
            int getCodeTable() const override;

            // Vietnamese Processing Settings
            void setFreeMarkEnabled(bool enabled) override;
            bool isFreeMarkEnabled() const override;

            void setSpellCheckingEnabled(bool enabled) override;
            bool isSpellCheckingEnabled() const override;

            void setModernOrthographyEnabled(bool enabled) override;
            bool isModernOrthographyEnabled() const override;

            void setQuickTelexEnabled(bool enabled) override;
            bool isQuickTelexEnabled() const override;

            void setRestoreIfWrongSpellingEnabled(bool enabled) override;
            bool isRestoreIfWrongSpellingEnabled() const override;

            // Macro Settings
            void setMacroEnabled(bool enabled) override;
            bool isMacroEnabled() const override;

            void setMacroInEnglishModeEnabled(bool enabled) override;
            bool isMacroInEnglishModeEnabled() const override;

            void setAutoCapsMacroEnabled(bool enabled) override;
            bool isAutoCapsMacroEnabled() const override;

            // Smart Switch Settings
            void setSmartSwitchKeyEnabled(bool enabled) override;
            bool isSmartSwitchKeyEnabled() const override;

            void setRememberCodeEnabled(bool enabled) override;
            bool isRememberCodeEnabled() const override;

            // Typing Settings
            void setUpperCaseFirstCharEnabled(bool enabled) override;
            bool isUpperCaseFirstCharEnabled() const override;

            void setDoubleSpacePeriodEnabled(bool enabled) override;
            bool isDoubleSpacePeriodEnabled() const override;

            void setTempOffEndKeyEnabled(bool enabled) override;
            bool isTempOffEndKeyEnabled() const override;

            // Advanced Settings
            void setFixRecommendBrowserEnabled(bool enabled) override;
            bool isFixRecommendBrowserEnabled() const override;

            void setTempOffSpellingEnabled(bool enabled) override;
            bool isTempOffSpellingEnabled() const override;

            void setAllowConsonantZFWJEnabled(bool enabled) override;
            bool isAllowConsonantZFWJEnabled() const override;

            void setQuickStartConsonantEnabled(bool enabled) override;
            bool isQuickStartConsonantEnabled() const override;

            void setQuickEndConsonantEnabled(bool enabled) override;
            bool isQuickEndConsonantEnabled() const override;

            void setOtherLanguageEnabled(bool enabled) override;
            bool isOtherLanguageEnabled() const override;

            // State Management
            void setSwitchKeyStatus(int status) override;
            int getSwitchKeyStatus() const override;

            void setEnglishMode(bool enabled) override;
            bool isEnglishMode() const override;

            // Configuration Persistence
            bool loadConfiguration() override;
            bool saveConfiguration() override;
            void resetToDefaults() override;

            // Application-specific settings
            void setApplicationSetting(const std::string& appId, const std::string& key, const std::string& value) override;
            std::string getApplicationSetting(const std::string& appId, const std::string& key) const override;

        private:
            void initializeDefaults();
            std::string getConfigFilePath() const;
        };
    }
}

#endif /* ConfigurationManager_h */