//
//  EngineLegacy.cpp
//  EndKey
//
//  Created by EndKey Refactoring on 2025.
//  Copyright © 2025 Tuyen Mai. All rights reserved.
//

#include "EngineLegacy.h"
#include <algorithm>

// Global EngineCore instance
std::shared_ptr<EngineCore> gEngineCore = nullptr;

// Legacy global variables (these now act as proxies to EngineCore)
int vLanguage = 1;
int vInputType = 0;
int vFreeMark = 1;
int vCodeTable = 0;
int vSwitchKeyStatus = 0;
int vCheckSpelling = 1;
int vUseModernOrthography = 1;
int vQuickTelex = 1;
int vRestoreIfWrongSpelling = 1;
int vFixRecommendBrowser = 1;
int vUseMacro = 1;
int vUseMacroInEnglishMode = 0;
int vAutoCapsMacro = 1;
int vUseSmartSwitchKey = 1;
int vUpperCaseFirstChar = 0;
int vTempOffSpelling = 1;
int vAllowConsonantZFWJ = 1;
int vQuickStartConsonant = 1;
int vQuickEndConsonant = 1;
int vRememberCode = 1;
int vOtherLanguage = 1;
int vTempOffEndKey = 1;
int vDoubleSpacePeriod = 1;

// Legacy global state
vKeyHookState HookState;

// Initialize legacy engine
void initializeLegacyEngine() {
    if (!gEngineCore) {
        gEngineCore = std::make_shared<EngineCore>();

        // Initialize with current legacy configuration
        EngineConfig config;
        config.language = vLanguage;
        config.inputType = vInputType;
        config.freeMark = vFreeMark;
        config.codeTable = vCodeTable;
        config.switchKeyStatus = vSwitchKeyStatus;
        config.checkSpelling = vCheckSpelling;
        config.useModernOrthography = vUseModernOrthography;
        config.quickTelex = vQuickTelex;
        config.restoreIfWrongSpelling = vRestoreIfWrongSpelling;
        config.fixRecommendBrowser = vFixRecommendBrowser;
        config.useMacro = vUseMacro;
        config.useMacroInEnglishMode = vUseMacroInEnglishMode;
        config.autoCapsMacro = vAutoCapsMacro;
        config.useSmartSwitchKey = vUseSmartSwitchKey;
        config.upperCaseFirstChar = vUpperCaseFirstChar;
        config.tempOffSpelling = vTempOffSpelling;
        config.allowConsonantZFWJ = vAllowConsonantZFWJ;
        config.quickStartConsonant = vQuickStartConsonant;
        config.quickEndConsonant = vQuickEndConsonant;
        config.rememberCode = vRememberCode;
        config.otherLanguage = vOtherLanguage;
        config.tempOffEndKey = vTempOffEndKey;
        config.doubleSpacePeriod = vDoubleSpacePeriod;

        gEngineCore->initialize(config);
    }
}

// Shutdown legacy engine
void shutdownLegacyEngine() {
    if (gEngineCore) {
        gEngineCore->shutdown();
        gEngineCore.reset();
    }
}

// Legacy API implementation
void* vKeyInit() {
    initializeLegacyEngine();
    return gEngineCore.get();
}

Uint32 getCharacterCode(const Uint32& data) {
    if (!gEngineCore) {
        initializeLegacyEngine();
    }
    return gEngineCore->getCharacterCode(data);
}

void vKeyHandleEvent(const vKeyEvent& event,
                     const vKeyEventState& state,
                     const Uint16& data,
                     const Uint8& capsStatus,
                     const bool& otherControlKey) {
    if (!gEngineCore) {
        initializeLegacyEngine();
    }

    // Sync legacy config to EngineCore before processing
    syncLegacyConfigToEngineCore();

    // Process the event
    HookState = gEngineCore->processKeyEvent(event, state, data, capsStatus, otherControlKey);

    // Sync EngineCore state back to legacy variables
    syncEngineCoreToLegacyConfig();
}

void startNewSession() {
    if (!gEngineCore) {
        initializeLegacyEngine();
    }
    gEngineCore->startNewSession();
}

void vEnglishMode(const vKeyEventState& state, const Uint16& data, const bool& isCaps, const bool& otherControlKey) {
    if (!gEngineCore) {
        initializeLegacyEngine();
    }

    // Legacy English mode - disable Vietnamese processing
    gEngineCore->setVietnameseMode(false);
}

void vTempOffSpellChecking() {
    if (!gEngineCore) {
        initializeLegacyEngine();
    }

    vTempOffSpelling = 1;
    syncLegacyConfigToEngineCore();
}

void vSetCheckSpelling() {
    if (!gEngineCore) {
        initializeLegacyEngine();
    }

    vTempOffSpelling = 0;
    syncLegacyConfigToEngineCore();
}

void vSkipMacroNextBreak() {
    if (!gEngineCore) {
        initializeLegacyEngine();
    }
    gEngineCore->skipMacroNextBreak();
}

void vForceSkipMacroNextBreak() {
    if (!gEngineCore) {
        initializeLegacyEngine();
    }
    gEngineCore->forceSkipMacroNextBreak();
}

// Utility functions
wstring utf8ToWideString(const string& str) {
    // Use ConvertTool for conversion if available
    if (gEngineCore) {
        try {
            auto& convertTool = gEngineCore->getConvertTool();
            // ConvertTool implementation would go here
            // For now, use basic conversion
            std::wstring result;
            result.reserve(str.length());
            for (size_t i = 0; i < str.length(); ) {
                char c = str[i];
                if ((c & 0x80) == 0) {
                    // ASCII character
                    result += static_cast<wchar_t>(c);
                    i++;
                } else if ((c & 0xE0) == 0xC0) {
                    // 2-byte UTF-8
                    if (i + 1 < str.length()) {
                        wchar_t wc = ((str[i] & 0x1F) << 6) | (str[i + 1] & 0x3F);
                        result += wc;
                        i += 2;
                    } else {
                        i++;
                    }
                } else if ((c & 0xF0) == 0xE0) {
                    // 3-byte UTF-8
                    if (i + 2 < str.length()) {
                        wchar_t wc = ((str[i] & 0x0F) << 12) | ((str[i + 1] & 0x3F) << 6) | (str[i + 2] & 0x3F);
                        result += wc;
                        i += 3;
                    } else {
                        i++;
                    }
                } else {
                    // Invalid UTF-8, skip
                    i++;
                }
            }
            return result;
        } catch (const std::exception&) {
            // Fall back to basic conversion
        }
    }

    // Basic UTF-8 to wide string conversion
    std::wstring result;
    result.reserve(str.length());
    for (char c : str) {
        result += static_cast<wchar_t>(static_cast<unsigned char>(c));
    }
    return result;
}

string wideStringToUtf8(const wstring& str) {
    // Use ConvertTool for conversion if available
    if (gEngineCore) {
        try {
            auto& convertTool = gEngineCore->getConvertTool();
            // ConvertTool implementation would go here
            // For now, use basic conversion
            std::string result;
            result.reserve(str.length() * 3); // Worst case for UTF-8
            for (wchar_t wc : str) {
                if (wc < 0x80) {
                    result += static_cast<char>(wc);
                } else if (wc < 0x800) {
                    result += static_cast<char>(0xC0 | (wc >> 6));
                    result += static_cast<char>(0x80 | (wc & 0x3F));
                } else {
                    result += static_cast<char>(0xE0 | (wc >> 12));
                    result += static_cast<char>(0x80 | ((wc >> 6) & 0x3F));
                    result += static_cast<char>(0x80 | (wc & 0x3F));
                }
            }
            return result;
        } catch (const std::exception&) {
            // Fall back to basic conversion
        }
    }

    // Basic wide string to UTF-8 conversion
    std::string result;
    result.reserve(str.length());
    for (wchar_t wc : str) {
        if (wc < 256) {
            result += static_cast<char>(wc);
        } else {
            result += '?'; // Replacement character for non-ASCII
        }
    }
    return result;
}

// Configuration synchronization functions
void syncLegacyConfigToEngineCore() {
    if (!gEngineCore) return;

    EngineConfig config;
    config.language = vLanguage;
    config.inputType = vInputType;
    config.freeMark = vFreeMark;
    config.codeTable = vCodeTable;
    config.switchKeyStatus = vSwitchKeyStatus;
    config.checkSpelling = vCheckSpelling;
    config.useModernOrthography = vUseModernOrthography;
    config.quickTelex = vQuickTelex;
    config.restoreIfWrongSpelling = vRestoreIfWrongSpelling;
    config.fixRecommendBrowser = vFixRecommendBrowser;
    config.useMacro = vUseMacro;
    config.useMacroInEnglishMode = vUseMacroInEnglishMode;
    config.autoCapsMacro = vAutoCapsMacro;
    config.useSmartSwitchKey = vUseSmartSwitchKey;
    config.upperCaseFirstChar = vUpperCaseFirstChar;
    config.tempOffSpelling = vTempOffSpelling;
    config.allowConsonantZFWJ = vAllowConsonantZFWJ;
    config.quickStartConsonant = vQuickStartConsonant;
    config.quickEndConsonant = vQuickEndConsonant;
    config.rememberCode = vRememberCode;
    config.otherLanguage = vOtherLanguage;
    config.tempOffEndKey = vTempOffEndKey;
    config.doubleSpacePeriod = vDoubleSpacePeriod;

    gEngineCore->initialize(config);
}

void syncEngineCoreToLegacyConfig() {
    if (!gEngineCore) return;

    const EngineConfig& config = gEngineCore->getConfig();

    // Update legacy variables from EngineCore configuration
    updateLegacyVariable(vLanguage, config.language);
    updateLegacyVariable(vInputType, config.inputType);
    updateLegacyVariable(vFreeMark, config.freeMark);
    updateLegacyVariable(vCodeTable, config.codeTable);
    updateLegacyVariable(vSwitchKeyStatus, config.switchKeyStatus);
    updateLegacyVariable(vCheckSpelling, config.checkSpelling);
    updateLegacyVariable(vUseModernOrthography, config.useModernOrthography);
    updateLegacyVariable(vQuickTelex, config.quickTelex);
    updateLegacyVariable(vRestoreIfWrongSpelling, config.restoreIfWrongSpelling);
    updateLegacyVariable(vFixRecommendBrowser, config.fixRecommendBrowser);
    updateLegacyVariable(vUseMacro, config.useMacro);
    updateLegacyVariable(vUseMacroInEnglishMode, config.useMacroInEnglishMode);
    updateLegacyVariable(vAutoCapsMacro, config.autoCapsMacro);
    updateLegacyVariable(vUseSmartSwitchKey, config.useSmartSwitchKey);
    updateLegacyVariable(vUpperCaseFirstChar, config.upperCaseFirstChar);
    updateLegacyVariable(vTempOffSpelling, config.tempOffSpelling);
    updateLegacyVariable(vAllowConsonantZFWJ, config.allowConsonantZFWJ);
    updateLegacyVariable(vQuickStartConsonant, config.quickStartConsonant);
    updateLegacyVariable(vQuickEndConsonant, config.quickEndConsonant);
    updateLegacyVariable(vRememberCode, config.rememberCode);
    updateLegacyVariable(vOtherLanguage, config.otherLanguage);
    updateLegacyVariable(vTempOffEndKey, config.tempOffEndKey);
    updateLegacyVariable(vDoubleSpacePeriod, config.doubleSpacePeriod);

    // Update state variables
    vLanguage = gEngineCore->isVietnameseMode() ? 1 : 0;
}

void updateLegacyVariable(int& variable, int newValue) {
    variable = newValue;
}