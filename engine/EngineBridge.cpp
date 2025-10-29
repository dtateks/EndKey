//
//  EngineBridge.cpp
//  EndKey
//
//  Created by Hive Coder Agent on 10/26/25.
//  Copyright © 2025 EndKey. All rights reserved.
//

#include "EngineBridge.h"
#include "core/EngineCore.h"
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

        EngineBridge::EngineBridge() : initialized(false),
            configManager(nullptr), vietnameseProcessor(nullptr),
            macroProcessor(nullptr), inputProcessor(nullptr) {
        }

        bool EngineBridge::initialize() {
            try {
                // Create factory
                EndKey::Core::EngineFactory factory;

                // Create engine instance
                g_engineInstance = factory.createEngine();
                if (!g_engineInstance || !g_engineInstance->initialize()) {
                    std::cerr << "Failed to initialize engine core" << std::endl;
                    return false;
                }

                // Get component references
                configManager = g_engineInstance->getConfigurationManager();
                vietnameseProcessor = g_engineInstance->getVietnameseProcessor();
                macroProcessor = g_engineInstance->getMacroProcessor();
                inputProcessor = nullptr; // Will be set by engine core

                initialized = true;

                // Map legacy variables to new configuration
                mapLegacyVariables();

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
            if (!configManager) return;

            // Sync legacy variables to configuration manager
            syncLegacyToNew();
        }

        void EngineBridge::syncLegacyToNew() {
            if (!configManager) return;

            // Language and Input
            configManager->setLanguage(vLanguage);
            configManager->setInputType(static_cast<EndKey::Core::vKeyInputType>(vInputType));
            // ENCODING REMOVAL: Always use Unicode (0), ignore vCodeTable value
            configManager->setCodeTable(0);
            configManager->setSwitchKeyStatus(vSwitchKeyStatus);

            // Vietnamese Processing
            configManager->setFreeMarkEnabled(vFreeMark != 0);
            configManager->setSpellCheckingEnabled(vCheckSpelling != 0);
            configManager->setModernOrthographyEnabled(vUseModernOrthography != 0);
            configManager->setQuickTelexEnabled(vQuickTelex != 0);
            configManager->setRestoreIfWrongSpellingEnabled(vRestoreIfWrongSpelling != 0);

            // Macro Settings
            configManager->setMacroEnabled(vUseMacro != 0);
            configManager->setMacroInEnglishModeEnabled(vUseMacroInEnglishMode != 0);
            configManager->setAutoCapsMacroEnabled(vAutoCapsMacro != 0);

            // Smart Switch Settings
            configManager->setSmartSwitchKeyEnabled(vUseSmartSwitchKey != 0);
            configManager->setRememberCodeEnabled(vRememberCode != 0);

            // Typing Settings
            configManager->setUpperCaseFirstCharEnabled(vUpperCaseFirstChar != 0);
            configManager->setTempOffSpellingEnabled(vTempOffSpelling != 0);
            configManager->setAllowConsonantZFWJEnabled(vAllowConsonantZFWJ != 0);
            configManager->setQuickStartConsonantEnabled(vQuickStartConsonant != 0);
            configManager->setQuickEndConsonantEnabled(vQuickEndConsonant != 0);
            configManager->setOtherLanguageEnabled(vOtherLanguage != 0);
            configManager->setTempOffEndKeyEnabled(vTempOffEndKey != 0);
            configManager->setDoubleSpacePeriodEnabled(vDoubleSpacePeriod != 0);

            // Advanced Settings
            configManager->setFixRecommendBrowserEnabled(vFixRecommendBrowser != 0);

            // State Management
            configManager->setEnglishMode(vLanguage == 0);
        }

        void EngineBridge::syncNewToLegacy() {
            if (!configManager) return;

            // Language and Input
            vLanguage = configManager->getLanguage();
            vInputType = static_cast<int>(configManager->getInputType());
            // ENCODING REMOVAL: Always use Unicode (0), ignore configManager value
            vCodeTable = 0;
            vSwitchKeyStatus = configManager->getSwitchKeyStatus();

            // Vietnamese Processing
            vFreeMark = configManager->isFreeMarkEnabled() ? 1 : 0;
            vCheckSpelling = configManager->isSpellCheckingEnabled() ? 1 : 0;
            vUseModernOrthography = configManager->isModernOrthographyEnabled() ? 1 : 0;
            vQuickTelex = configManager->isQuickTelexEnabled() ? 1 : 0;
            vRestoreIfWrongSpelling = configManager->isRestoreIfWrongSpellingEnabled() ? 1 : 0;

            // Macro Settings
            vUseMacro = configManager->isMacroEnabled() ? 1 : 0;
            vUseMacroInEnglishMode = configManager->isMacroInEnglishModeEnabled() ? 1 : 0;
            vAutoCapsMacro = configManager->isAutoCapsMacroEnabled() ? 1 : 0;

            // Smart Switch Settings
            vUseSmartSwitchKey = configManager->isSmartSwitchKeyEnabled() ? 1 : 0;
            vRememberCode = configManager->isRememberCodeEnabled() ? 1 : 0;

            // Typing Settings
            vUpperCaseFirstChar = configManager->isUpperCaseFirstCharEnabled() ? 1 : 0;
            vTempOffSpelling = configManager->isTempOffSpellingEnabled() ? 1 : 0;
            vAllowConsonantZFWJ = configManager->isAllowConsonantZFWJEnabled() ? 1 : 0;
            vQuickStartConsonant = configManager->isQuickStartConsonantEnabled() ? 1 : 0;
            vQuickEndConsonant = configManager->isQuickEndConsonantEnabled() ? 1 : 0;
            vOtherLanguage = configManager->isOtherLanguageEnabled() ? 1 : 0;
            vTempOffEndKey = configManager->isTempOffEndKeyEnabled() ? 1 : 0;
            vDoubleSpacePeriod = configManager->isDoubleSpacePeriodEnabled() ? 1 : 0;

            // Advanced Settings
            vFixRecommendBrowser = configManager->isFixRecommendBrowserEnabled() ? 1 : 0;
        }
    }
}

//=============================================================================
// LEGACY API FUNCTION IMPLEMENTATIONS
//=============================================================================

void* vKeyInit() {
    try {
        if (!g_bridgeInstance) {
            g_bridgeInstance = std::make_unique<EndKey::Legacy::EngineBridge>();
        }

        if (!g_bridgeInstance->isInitialized()) {
            if (!g_bridgeInstance->initialize()) {
                return nullptr;
            }
        }

        // Return pointer to maintain API compatibility
        // In the new architecture, this returns a handle to the engine
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
        // Sync legacy variables to engine before processing
        if (g_bridgeInstance) {
            g_bridgeInstance->syncLegacyToNew();
        }

        g_engineInstance->processKeyEvent(event, state, data, capsStatus, otherControlKey);

        // Sync engine state back to legacy variables
        if (g_bridgeInstance) {
            g_bridgeInstance->syncNewToLegacy();
        }
    }
}

void startNewSession() {
    if (g_engineInstance) {
        g_engineInstance->startNewSession();
    }
}

void vEnglishMode(const vKeyEventState& state, const Uint16& data, const bool& isCaps, const bool& otherControlKey) {
    if (g_engineInstance) {
        // Cast to EngineCore to access specific methods
        auto* engineCore = static_cast<EndKey::Core::EngineCore*>(g_engineInstance.get());
        if (engineCore) {
            engineCore->vEnglishMode(state, data, isCaps, otherControlKey);
        }
    }
}

void vTempOffSpellChecking() {
    if (g_engineInstance) {
        auto* engineCore = static_cast<EndKey::Core::EngineCore*>(g_engineInstance.get());
        if (engineCore) {
            engineCore->vTempOffSpellChecking();
        }
    }
}

void vSetCheckSpelling() {
    if (g_engineInstance) {
        auto* engineCore = static_cast<EndKey::Core::EngineCore*>(g_engineInstance.get());
        if (engineCore) {
            engineCore->vSetCheckSpelling();
        }
    }
}

void vSkipMacroNextBreak() {
    if (g_engineInstance) {
        auto* engineCore = static_cast<EndKey::Core::EngineCore*>(g_engineInstance.get());
        if (engineCore) {
            engineCore->vSkipMacroNextBreak();
        }
    }
}

void vForceSkipMacroNextBreak() {
    if (g_engineInstance) {
        auto* engineCore = static_cast<EndKey::Core::EngineCore*>(g_engineInstance.get());
        if (engineCore) {
            engineCore->vForceSkipMacroNextBreak();
        }
    }
}

std::wstring utf8ToWideString(const std::string& str) {
    return EndKey::Core::VietnameseProcessor::utf8ToWideString(str);
}

std::string wideStringToUtf8(const std::wstring& str) {
    return EndKey::Core::VietnameseProcessor::wideStringToUtf8(str);
}