//
//  EngineLegacy.h
//  EndKey
//
//  Created by EndKey Refactoring on 2025.
//  Copyright © 2025 Tuyen Mai. All rights reserved.
//

#ifndef EngineLegacy_h
#define EngineLegacy_h

// This file provides backward compatibility for the original Engine.h API
// It bridges the old global variable interface to the new EngineCore architecture

#include "Engine.h"
#include "EngineCore.h"
#include <memory>

// Global EngineCore instance (singleton pattern)
extern std::shared_ptr<EngineCore> gEngineCore;

// Legacy API compatibility functions
void initializeLegacyEngine();
void shutdownLegacyEngine();

// Global variable mappings (redirect to EngineCore)
extern int vLanguage;
extern int vInputType;
extern int vFreeMark;
extern int vCodeTable;
extern int vSwitchKeyStatus;
extern int vCheckSpelling;
extern int vUseModernOrthography;
extern int vQuickTelex;
extern int vRestoreIfWrongSpelling;
extern int vFixRecommendBrowser;
extern int vUseMacro;
extern int vUseMacroInEnglishMode;
extern int vAutoCapsMacro;
extern int vUseSmartSwitchKey;
extern int vUpperCaseFirstChar;
extern int vTempOffSpelling;
extern int vAllowConsonantZFWJ;
extern int vQuickStartConsonant;
extern int vQuickEndConsonant;
extern int vRememberCode;
extern int vOtherLanguage;
extern int vTempOffEndKey;
extern int vDoubleSpacePeriod;

// Legacy global state
extern vKeyHookState HookState;

// Legacy API functions
void* vKeyInit();
Uint32 getCharacterCode(const Uint32& data);
void vKeyHandleEvent(const vKeyEvent& event,
                     const vKeyEventState& state,
                     const Uint16& data,
                     const Uint8& capsStatus = 0,
                     const bool& otherControlKey = false);
void startNewSession();
void vEnglishMode(const vKeyEventState& state, const Uint16& data, const bool& isCaps, const bool& otherControlKey);
void vTempOffSpellChecking();
void vSetCheckSpelling();
void vSkipMacroNextBreak();
void vForceSkipMacroNextBreak();

// Utility functions
wstring utf8ToWideString(const string& str);
string wideStringToUtf8(const wstring& str);

// Configuration synchronization functions
void syncLegacyConfigToEngineCore();
void syncEngineCoreToLegacyConfig();
void updateLegacyVariable(int& variable, int newValue);

#endif /* EngineLegacy_h */