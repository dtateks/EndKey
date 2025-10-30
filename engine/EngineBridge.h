//
//  EngineBridge.h
//  EndKey
//
//  Created by Hive Coder Agent on 10/26/25.
//  Copyright © 2025 EndKey. All rights reserved.
//

#ifndef EngineBridge_h
#define EngineBridge_h

/**
 * @file EngineBridge.h
 * @brief Backward compatibility bridge for existing EndKey API
 *
 * This file provides backward compatibility with the existing EndKey API
 * while using the new clean architecture underneath.
 *
 * Legacy global variables and functions are mapped to the new architecture
 * components to ensure existing code continues to work without modifications.
 */

#include "EngineCore.h"
#include "DataType.h"
#include <memory>

// Forward declarations for backward compatibility
namespace EndKey {
    namespace Legacy {
        class EngineBridge;
    }
}

/**
 * Global engine instance for backward compatibility
 */
static std::unique_ptr<EngineCore> g_engineInstance = nullptr;

namespace EndKey {
    namespace Legacy {

        /**
         * Bridge class to maintain backward compatibility
         * Maps legacy API calls to new architecture
         */
        class EngineBridge {
        private:
            bool initialized;

        public:
            EngineBridge();
            ~EngineBridge() = default;

            bool initialize();
            void shutdown();
            bool isInitialized() const { return initialized; }

            // Legacy API mapping
            void mapLegacyVariables();
            void syncLegacyToNew();
            void syncNewToLegacy();
        };

    }
}

//=============================================================================
// LEGACY API COMPATIBILITY LAYER
// These functions maintain the exact same API as the original Engine.h
//=============================================================================

/**
 * Initialize global engine instance
 * Call this function first to receive data pointer
 */
void* vKeyInit();

/**
 * Convert engine character to real character
 */
Uint32 getCharacterCode(const Uint32& data);

/**
 * MAIN entry point for each key
 * event: mouse or keyboard event
 * state: additional state for event
 * data: key code
 * isCaps: caplock is on or shift key is pressing
 * otherControlKey: ctrl, option,... is pressing
 */
void vKeyHandleEvent(const vKeyEvent& event,
                     const vKeyEventState& state,
                     const Uint16& data,
                     const Uint8& capsStatus = 0,
                     const bool& otherControlKey = false);

/**
 * Start a new word
 */
void startNewSession();

/**
 * do some task in english mode (use for macro)
 */
void vEnglishMode(const vKeyEventState& state, const Uint16& data, const bool& isCaps, const bool& otherControlKey);

/**
 * temporarily turn off spell checking
 */
void vTempOffSpellChecking();

/**
 * reset spelling value
 */
void vSetCheckSpelling();

/**
 * Skip macro and quick consonant replacement on next word break (Space, punctuation, etc.)
 * Only activates if currently typing a word (_index > 0)
 */
void vSkipMacroNextBreak();

/**
 * Force skip macro on next word break WITHOUT checking _index
 * Used when called AFTER vKeyHandleEvent has already reset _index = 0
 */
void vForceSkipMacroNextBreak();

/**
 * some utils function
 */
std::wstring utf8ToWideString(const std::string& str);
std::string wideStringToUtf8(const std::wstring& str);

//=============================================================================
// LEGACY GLOBAL VARIABLES
// These are mapped to the new configuration manager
//=============================================================================

// Language and Input
extern int vLanguage;        // 0: English, 1: Vietnamese
extern int vInputType;       // 0: Telex, 1: VNI
extern int vFreeMark;        // 0: No, 1: Yes
extern int vCodeTable;       // ENCODING REMOVAL: Unicode-only (0) - legacy variable kept for compatibility
extern int vSwitchKeyStatus; // first 8 bit: keycode, bit 8: Control, etc.

// Vietnamese Processing
extern int vCheckSpelling;              // 0: No, 1: Yes
extern int vUseModernOrthography;       // 0: òa, úy, 1: oà uý
extern int vQuickTelex;                 // 0: No, 1: Yes (cc=ch, gg=gi, etc.)
extern int vRestoreIfWrongSpelling;     // 0: No, 1: Yes

// Macro Settings
extern int vUseMacro;                   // Macro on or off
extern int vUseMacroInEnglishMode;      // Still use macro in English mode
extern int vAutoCapsMacro;              // Auto capitalize macro expansions

// Smart Switch Settings
extern int vUseSmartSwitchKey;          // auto switch language when switch app
extern int vRememberCode;               // Auto remember table code per app

// Typing Settings
extern int vUpperCaseFirstChar;         // Auto uppercase first character
extern int vTempOffSpelling;            // Temporarily turn off spelling with Ctrl
extern int vAllowConsonantZFWJ;         // Allow Z, F, W, J consonants
extern int vQuickStartConsonant;        // f->ph, j->gi, w->qu
extern int vQuickEndConsonant;          // g->ng, h->nh, k->ch
extern int vOtherLanguage;              // Turn off for other languages
extern int vTempOffEndKey;              // Temporarily turn off EndKey
extern int vDoubleSpacePeriod;          // Double space -> period

// Advanced Settings
extern int vFixRecommendBrowser;        // Fix browser's address, excel, etc.

//=============================================================================
// UTILITY MACROS
//=============================================================================

// Legacy macros for backward compatibility
#ifndef LOBYTE
#define LOBYTE(data) (data & 0xFF)
#endif

#ifndef HIBYTE
#define HIBYTE(data) ((data>>8) & 0xFF)
#endif

#define GET_SWITCH_KEY(data) (data & 0xFF)
#define HAS_CONTROL(data) ((data & 0x100) ? 1 : 0)
#define HAS_OPTION(data) ((data & 0x200) ? 1 : 0)
#define HAS_COMMAND(data) ((data & 0x400) ? 1 : 0)
#define HAS_SHIFT(data) ((data & 0x800) ? 1 : 0)
#define GET_BOOL(data) (data ? 1 : 0)
#define HAS_BEEP(data) (data & 0x8000)
#define SET_SWITCH_KEY(data, key) data = (data & 0xFF) | key
#define SET_CONTROL_KEY(data, val) data|=val<<8;
#define SET_OPTION_KEY(data, val) data|=val<<9;
#define SET_COMMAND_KEY(data, val) data|=val<<10;

#endif /* EngineBridge_h */