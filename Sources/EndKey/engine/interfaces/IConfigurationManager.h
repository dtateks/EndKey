//
//  IConfigurationManager.h
//  EndKey
//
//  Created by Hive Coder Agent on 10/26/25.
//  Copyright © 2025 EndKey. All rights reserved.
//

#ifndef IConfigurationManager_h
#define IConfigurationManager_h

#include "DataType.h"
#include <string>
#include <map>

namespace EndKey {
    namespace Core {
        /**
         * Interface for configuration management
         * Centralized settings and preferences
         */
        class IConfigurationManager {
        public:
            virtual ~IConfigurationManager() = default;

            // Language and Input Settings
            virtual void setLanguage(int language) = 0; // 0: English, 1: Vietnamese
            virtual int getLanguage() const = 0;

            virtual void setInputType(vKeyInputType type) = 0; // 0: Telex, 1: VNI, etc.
            virtual vKeyInputType getInputType() const = 0;

            virtual void setCodeTable(int codeTable) = 0; // 0: Unicode, 1: TCVN3, 2: VNI-Windows
            virtual int getCodeTable() const = 0;

            // Vietnamese Processing Settings
            virtual void setFreeMarkEnabled(bool enabled) = 0;
            virtual bool isFreeMarkEnabled() const = 0;

            virtual void setSpellCheckingEnabled(bool enabled) = 0;
            virtual bool isSpellCheckingEnabled() const = 0;

            virtual void setModernOrthographyEnabled(bool enabled) = 0;
            virtual bool isModernOrthographyEnabled() const = 0;

            virtual void setQuickTelexEnabled(bool enabled) = 0;
            virtual bool isQuickTelexEnabled() const = 0;

            virtual void setRestoreIfWrongSpellingEnabled(bool enabled) = 0;
            virtual bool isRestoreIfWrongSpellingEnabled() const = 0;

            // Macro Settings
            virtual void setMacroEnabled(bool enabled) = 0;
            virtual bool isMacroEnabled() const = 0;

            virtual void setMacroInEnglishModeEnabled(bool enabled) = 0;
            virtual bool isMacroInEnglishModeEnabled() const = 0;

            virtual void setAutoCapsMacroEnabled(bool enabled) = 0;
            virtual bool isAutoCapsMacroEnabled() const = 0;

            // Smart Switch Settings
            virtual void setSmartSwitchKeyEnabled(bool enabled) = 0;
            virtual bool isSmartSwitchKeyEnabled() const = 0;

            virtual void setRememberCodeEnabled(bool enabled) = 0;
            virtual bool isRememberCodeEnabled() const = 0;

            // Typing Settings
            virtual void setUpperCaseFirstCharEnabled(bool enabled) = 0;
            virtual bool isUpperCaseFirstCharEnabled() const = 0;

            virtual void setDoubleSpacePeriodEnabled(bool enabled) = 0;
            virtual bool isDoubleSpacePeriodEnabled() const = 0;

            virtual void setTempOffEndKeyEnabled(bool enabled) = 0;
            virtual bool isTempOffEndKeyEnabled() const = 0;

            // Advanced Settings
            virtual void setFixRecommendBrowserEnabled(bool enabled) = 0;
            virtual bool isFixRecommendBrowserEnabled() const = 0;

            virtual void setTempOffSpellingEnabled(bool enabled) = 0;
            virtual bool isTempOffSpellingEnabled() const = 0;

            virtual void setAllowConsonantZFWJEnabled(bool enabled) = 0;
            virtual bool isAllowConsonantZFWJEnabled() const = 0;

            virtual void setQuickStartConsonantEnabled(bool enabled) = 0;
            virtual bool isQuickStartConsonantEnabled() const = 0;

            virtual void setQuickEndConsonantEnabled(bool enabled) = 0;
            virtual bool isQuickEndConsonantEnabled() const = 0;

            virtual void setOtherLanguageEnabled(bool enabled) = 0;
            virtual bool isOtherLanguageEnabled() const = 0;

            // State Management
            virtual void setSwitchKeyStatus(int status) = 0;
            virtual int getSwitchKeyStatus() const = 0;

            virtual void setEnglishMode(bool enabled) = 0;
            virtual bool isEnglishMode() const = 0;

            // Configuration Persistence
            virtual bool loadConfiguration() = 0;
            virtual bool saveConfiguration() = 0;
            virtual void resetToDefaults() = 0;

            // Application-specific settings
            virtual void setApplicationSetting(const std::string& appId, const std::string& key, const std::string& value) = 0;
            virtual std::string getApplicationSetting(const std::string& appId, const std::string& key) const = 0;
        };
    }
}

#endif /* IConfigurationManager_h */