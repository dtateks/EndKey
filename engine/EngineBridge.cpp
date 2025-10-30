//
//  EngineBridge.cpp
//  EndKey
//
//  Created by Hive Coder Agent on 10/26/25.
//  Copyright © 2025 EndKey. All rights reserved.
//

#include "EngineBridge.h"
#include "EngineCore.h"
#include "ConvertTool.h"
#include <iostream>

//=============================================================================
// LEGACY GLOBAL VARIABLES IMPLEMENTATION
// These are mapped to the new configuration manager
//=============================================================================

// Language and Input
int vLanguage = 1;        // 0: English, 1: Vietnamese
int vInputType = 0;       // 0: Telex, 1: VNI
int vFreeMark = 1;        // 0: No, 1: Yes
int vCodeTable = 0;       // ENCODING REMOVAL: Unicode-only (0) - legacy variable kept for compatibility
int vSwitchKeyStatus = 0; // first 8 bit: keycode, bit 8: Control, etc.

// Vietnamese Processing
int vCheckSpelling = 1;              // 0: No, 1: Yes
int vUseModernOrthography = 1;       // 0: òa, úy, 1: oà uý
int vQuickTelex = 0;                 // 0: No, 1: Yes (cc=ch, gg=gi, etc.)
int vRestoreIfWrongSpelling = 1;     // 0: No, 1: Yes

// Macro Settings
int vUseMacro = 1;                   // Macro on or off
int vUseMacroInEnglishMode = 0;      // Still use macro in English mode
int vAutoCapsMacro = 1;              // Auto capitalize macro expansions

// Smart Switch Settings
int vUseSmartSwitchKey = 0;          // auto switch language when switch app
int vRememberCode = 1;               // Auto remember table code per app

// Typing Settings
int vUpperCaseFirstChar = 0;         // Auto uppercase first character
int vTempOffSpelling = 1;            // Temporarily turn off spelling with Ctrl
int vAllowConsonantZFWJ = 0;         // Allow Z, F, W, J consonants
int vQuickStartConsonant = 0;        // f->ph, j->gi, w->qu
int vQuickEndConsonant = 0;          // g->ng, h->nh, k->ch
int vOtherLanguage = 0;              // Turn off for other languages
int vTempOffEndKey = 1;              // Temporarily turn off EndKey
int vDoubleSpacePeriod = 0;          // Double space -> period

// Advanced Settings
int vFixRecommendBrowser = 1;        // Fix browser's address, excel, etc.

//=============================================================================
// ENGINE BRIDGE IMPLEMENTATION
//=============================================================================

namespace EndKey {
    namespace Legacy {

        EngineBridge::EngineBridge() : initialized(false) {
        }

        bool EngineBridge::initialize() {
            try {
                // Create engine instance directly
                g_engineInstance = std::make_unique<EngineCore>();
                
                EngineConfig config;
                // Convert legacy variables to config
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
                
                if (!g_engineInstance->initialize(config)) {
                    std::cerr << "Failed to initialize engine core" << std::endl;
                    return false;
                }

                initialized = true;
                return true;
            } catch (const std::exception& e) {
                std::cerr << "Engine bridge initialization failed: " << e.what() << std::endl;
                return false;
            }
        }

        void EngineBridge::shutdown() {
            if (g_engineInstance) {
                g_engineInstance->shutdown();
                g_engineInstance.reset();
            }
            initialized = false;
        }

        void EngineBridge::mapLegacyVariables() {
            // No longer needed with direct EngineCore usage
        }

        void EngineBridge::syncLegacyToNew() {
            // No longer needed with direct EngineCore usage
        }

        void EngineBridge::syncNewToLegacy() {
            // No longer needed with direct EngineCore usage
        }
    }
}

//=============================================================================
// LEGACY API FUNCTION IMPLEMENTATIONS
//=============================================================================

void* vKeyInit() {
    try {
        if (!g_engineInstance) {
            g_engineInstance = std::make_unique<EngineCore>();
        }

        EngineConfig config;
        // Convert legacy variables to config
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
        
        if (!g_engineInstance->initialize(config)) {
            std::cerr << "Failed to initialize engine core" << std::endl;
            return nullptr;
        }

        return static_cast<void*>(g_engineInstance.get());
    } catch (const std::exception& e) {
        std::cerr << "vKeyInit failed: " << e.what() << std::endl;
        return nullptr;
    }
}

Uint32 getCharacterCode(const Uint32& data) {
    if (g_engineInstance) {
        return g_engineInstance->getCharacterCode(data);
    }
    return data; // Fallback
}

void vKeyHandleEvent(const vKeyEvent& event,
                     const vKeyEventState& state,
                     const Uint16& data,
                     const Uint8& capsStatus,
                     const bool& otherControlKey) {
    if (g_engineInstance) {
        g_engineInstance->processKeyEvent(event, state, data, capsStatus, otherControlKey);
    }
}

void startNewSession() {
    if (g_engineInstance) {
        g_engineInstance->startNewSession();
    }
}

void vEnglishMode(const vKeyEventState& state, const Uint16& data, const bool& isCaps, const bool& otherControlKey) {
    if (g_engineInstance) {
        g_engineInstance->processEnglishMode(state, data, isCaps, otherControlKey);
    }
}

void vTempOffSpellChecking() {
    if (g_engineInstance) {
        g_engineInstance->setTempOffSpelling(true);
    }
}

void vSetCheckSpelling() {
    if (g_engineInstance) {
        g_engineInstance->setTempOffSpelling(false);
    }
}

void vSkipMacroNextBreak() {
    if (g_engineInstance) {
        g_engineInstance->skipMacroNextBreak();
    }
}

void vForceSkipMacroNextBreak() {
    if (g_engineInstance) {
        g_engineInstance->forceSkipMacroNextBreak();
    }
}

std::wstring utf8ToWideString(const std::string& str) {
    // Use utility function from ConvertTool
    return ConvertTool::utf8ToWideString(str);
}

std::string wideStringToUtf8(const std::wstring& str) {
    // Use utility function from ConvertTool
    return ConvertTool::wideStringToUtf8(str);
}