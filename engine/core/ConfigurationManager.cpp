//
//  ConfigurationManager.cpp
//  EndKey
//
//  Created by Hive Coder Agent on 10/26/25.
//  Copyright © 2025 EndKey. All rights reserved.
//

#include "ConfigurationManager.h"
#include <fstream>
#include <iostream>
#include <algorithm>

namespace EndKey {
    namespace Core {

        ConfigurationManager::ConfigurationManager() {
            initializeDefaults();
        }

        void ConfigurationManager::initializeDefaults() {
            // Language and Input
            language = 1; // Vietnamese by default
            inputType = vKeyInputType::vTelex;
            codeTable = 0; // Unicode

            // Vietnamese Processing
            freeMarkEnabled = true;
            spellCheckingEnabled = true;
            modernOrthographyEnabled = true;
            quickTelexEnabled = false;
            restoreIfWrongSpellingEnabled = true;

            // Macro Settings
            macroEnabled = true;
            macroInEnglishModeEnabled = false;
            autoCapsMacroEnabled = true;

            // Smart Switch Settings
            smartSwitchKeyEnabled = false;
            rememberCodeEnabled = true;

            // Typing Settings
            upperCaseFirstCharEnabled = false;
            doubleSpacePeriodEnabled = false;
            tempOffEndKeyEnabled = true;

            // Advanced Settings
            fixRecommendBrowserEnabled = true;
            tempOffSpellingEnabled = true;
            allowConsonantZFWJEnabled = false;
            quickStartConsonantEnabled = false;
            quickEndConsonantEnabled = false;
            otherLanguageEnabled = false;

            // State Management
            switchKeyStatus = 0;
            englishMode = false;

            applicationSettings.clear();
        }

        // Language and Input Settings
        void ConfigurationManager::setLanguage(int lang) {
            if (lang >= 0 && lang <= 1) {
                language = lang;
                englishMode = (lang == 0); // 0: English, 1: Vietnamese
            }
        }

        int ConfigurationManager::getLanguage() const {
            return language;
        }

        void ConfigurationManager::setInputType(vKeyInputType type) {
            if (type >= vKeyInputType::vTelex && type <= vKeyInputType::vSimpleTelex2) {
                inputType = type;
            }
        }

        vKeyInputType ConfigurationManager::getInputType() const {
            return inputType;
        }

        void ConfigurationManager::setCodeTable(int table) {
            // ENCODING REMOVAL: Force Unicode-only (0)
            // Ignore input parameter and always use Unicode
            codeTable = 0;
        }

        int ConfigurationManager::getCodeTable() const {
            // ENCODING REMOVAL: Always return Unicode (0)
            return 0;
        }

        // Vietnamese Processing Settings
        void ConfigurationManager::setFreeMarkEnabled(bool enabled) {
            freeMarkEnabled = enabled;
        }

        bool ConfigurationManager::isFreeMarkEnabled() const {
            return freeMarkEnabled;
        }

        void ConfigurationManager::setSpellCheckingEnabled(bool enabled) {
            spellCheckingEnabled = enabled;
        }

        bool ConfigurationManager::isSpellCheckingEnabled() const {
            return spellCheckingEnabled;
        }

        void ConfigurationManager::setModernOrthographyEnabled(bool enabled) {
            modernOrthographyEnabled = enabled;
        }

        bool ConfigurationManager::isModernOrthographyEnabled() const {
            return modernOrthographyEnabled;
        }

        void ConfigurationManager::setQuickTelexEnabled(bool enabled) {
            quickTelexEnabled = enabled;
        }

        bool ConfigurationManager::isQuickTelexEnabled() const {
            return quickTelexEnabled;
        }

        void ConfigurationManager::setRestoreIfWrongSpellingEnabled(bool enabled) {
            restoreIfWrongSpellingEnabled = enabled;
        }

        bool ConfigurationManager::isRestoreIfWrongSpellingEnabled() const {
            return restoreIfWrongSpellingEnabled;
        }

        // Macro Settings
        void ConfigurationManager::setMacroEnabled(bool enabled) {
            macroEnabled = enabled;
        }

        bool ConfigurationManager::isMacroEnabled() const {
            return macroEnabled;
        }

        void ConfigurationManager::setMacroInEnglishModeEnabled(bool enabled) {
            macroInEnglishModeEnabled = enabled;
        }

        bool ConfigurationManager::isMacroInEnglishModeEnabled() const {
            return macroInEnglishModeEnabled;
        }

        void ConfigurationManager::setAutoCapsMacroEnabled(bool enabled) {
            autoCapsMacroEnabled = enabled;
        }

        bool ConfigurationManager::isAutoCapsMacroEnabled() const {
            return autoCapsMacroEnabled;
        }

        // Smart Switch Settings
        void ConfigurationManager::setSmartSwitchKeyEnabled(bool enabled) {
            smartSwitchKeyEnabled = enabled;
        }

        bool ConfigurationManager::isSmartSwitchKeyEnabled() const {
            return smartSwitchKeyEnabled;
        }

        void ConfigurationManager::setRememberCodeEnabled(bool enabled) {
            rememberCodeEnabled = enabled;
        }

        bool ConfigurationManager::isRememberCodeEnabled() const {
            return rememberCodeEnabled;
        }

        // Typing Settings
        void ConfigurationManager::setUpperCaseFirstCharEnabled(bool enabled) {
            upperCaseFirstCharEnabled = enabled;
        }

        bool ConfigurationManager::isUpperCaseFirstCharEnabled() const {
            return upperCaseFirstCharEnabled;
        }

        void ConfigurationManager::setDoubleSpacePeriodEnabled(bool enabled) {
            doubleSpacePeriodEnabled = enabled;
        }

        bool ConfigurationManager::isDoubleSpacePeriodEnabled() const {
            return doubleSpacePeriodEnabled;
        }

        void ConfigurationManager::setTempOffEndKeyEnabled(bool enabled) {
            tempOffEndKeyEnabled = enabled;
        }

        bool ConfigurationManager::isTempOffEndKeyEnabled() const {
            return tempOffEndKeyEnabled;
        }

        // Advanced Settings
        void ConfigurationManager::setFixRecommendBrowserEnabled(bool enabled) {
            fixRecommendBrowserEnabled = enabled;
        }

        bool ConfigurationManager::isFixRecommendBrowserEnabled() const {
            return fixRecommendBrowserEnabled;
        }

        void ConfigurationManager::setTempOffSpellingEnabled(bool enabled) {
            tempOffSpellingEnabled = enabled;
        }

        bool ConfigurationManager::isTempOffSpellingEnabled() const {
            return tempOffSpellingEnabled;
        }

        void ConfigurationManager::setAllowConsonantZFWJEnabled(bool enabled) {
            allowConsonantZFWJEnabled = enabled;
        }

        bool ConfigurationManager::isAllowConsonantZFWJEnabled() const {
            return allowConsonantZFWJEnabled;
        }

        void ConfigurationManager::setQuickStartConsonantEnabled(bool enabled) {
            quickStartConsonantEnabled = enabled;
        }

        bool ConfigurationManager::isQuickStartConsonantEnabled() const {
            return quickStartConsonantEnabled;
        }

        void ConfigurationManager::setQuickEndConsonantEnabled(bool enabled) {
            quickEndConsonantEnabled = enabled;
        }

        bool ConfigurationManager::isQuickEndConsonantEnabled() const {
            return quickEndConsonantEnabled;
        }

        void ConfigurationManager::setOtherLanguageEnabled(bool enabled) {
            otherLanguageEnabled = enabled;
        }

        bool ConfigurationManager::isOtherLanguageEnabled() const {
            return otherLanguageEnabled;
        }

        // State Management
        void ConfigurationManager::setSwitchKeyStatus(int status) {
            switchKeyStatus = status;
        }

        int ConfigurationManager::getSwitchKeyStatus() const {
            return switchKeyStatus;
        }

        void ConfigurationManager::setEnglishMode(bool enabled) {
            englishMode = enabled;
            language = enabled ? 0 : 1; // 0: English, 1: Vietnamese
        }

        bool ConfigurationManager::isEnglishMode() const {
            return englishMode;
        }

        // Configuration Persistence
        bool ConfigurationManager::loadConfiguration() {
            try {
                std::string configPath = getConfigFilePath();
                std::ifstream file(configPath);

                if (!file.is_open()) {
                    std::cout << "Config file not found, using defaults" << std::endl;
                    return false;
                }

                // TODO: Implement JSON or INI format parsing
                // For now, just use defaults
                file.close();
                return true;
            } catch (const std::exception& e) {
                std::cerr << "Error loading configuration: " << e.what() << std::endl;
                return false;
            }
        }

        bool ConfigurationManager::saveConfiguration() {
            try {
                std::string configPath = getConfigFilePath();
                std::ofstream file(configPath);

                if (!file.is_open()) {
                    std::cerr << "Cannot create config file" << std::endl;
                    return false;
                }

                // TODO: Implement JSON or INI format writing
                file.close();
                return true;
            } catch (const std::exception& e) {
                std::cerr << "Error saving configuration: " << e.what() << std::endl;
                return false;
            }
        }

        void ConfigurationManager::resetToDefaults() {
            initializeDefaults();
        }

        // Application-specific settings
        void ConfigurationManager::setApplicationSetting(const std::string& appId, const std::string& key, const std::string& value) {
            applicationSettings[appId][key] = value;
        }

        std::string ConfigurationManager::getApplicationSetting(const std::string& appId, const std::string& key) const {
            auto appIt = applicationSettings.find(appId);
            if (appIt != applicationSettings.end()) {
                auto keyIt = appIt->second.find(key);
                if (keyIt != appIt->second.end()) {
                    return keyIt->second;
                }
            }
            return "";
        }

        std::string ConfigurationManager::getConfigFilePath() const {
            // TODO: Implement proper path resolution based on platform
            return "EndKey.conf";
        }

    }
}