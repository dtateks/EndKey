//
//  EngineCore.h
//  EndKey
//
//  Created by EndKey Refactoring on 2025.
//  Copyright © 2025 Tuyen Mai. All rights reserved.
//

#ifndef EngineCore_h
#define EngineCore_h

#include <memory>
#include <vector>
#include "DataType.h"
#include "VietnameseProcessor.h"
#include "MacroManager.h"
#include "SmartSwitchKey.h"
#include "ConvertTool.h"

// Configuration structure
struct EngineConfig {
    int language;                    // 0: English, 1: Vietnamese
    int inputType;                   // 0: Telex, 1: VNI
    int freeMark;                    // 0: No, 1: Yes
    int codeTable;                   // 0: Unicode, 1: TCVN3, 2: VNI-Windows
    int switchKeyStatus;            // Key modifier status
    int checkSpelling;              // 0: No, 1: Yes
    int useModernOrthography;       // 0: No, 1: Yes
    int quickTelex;                 // 0: No, 1: Yes
    int restoreIfWrongSpelling;     // 0: No, 1: Yes
    int fixRecommendBrowser;        // 0: No, 1: Yes
    int useMacro;                   // 0: No, 1: Yes
    int useMacroInEnglishMode;      // 0: No, 1: Yes
    int autoCapsMacro;              // 0: No, 1: Yes
    int useSmartSwitchKey;          // 0: No, 1: Yes
    int upperCaseFirstChar;         // 0: No, 1: Yes
    int tempOffSpelling;            // 0: No, 1: Yes
    int allowConsonantZFWJ;         // 0: No, 1: Yes
    int quickStartConsonant;        // 0: No, 1: Yes
    int quickEndConsonant;          // 0: No, 1: Yes
    int rememberCode;               // 0: No, 1: Yes
    int otherLanguage;              // 0: No, 1: Yes
    int tempOffEndKey;              // 0: No, 1: Yes
    int doubleSpacePeriod;          // 0: No, 1: Yes
};

// Engine state structure
struct EngineState {
    std::vector<Uint32> typingWord;
    std::vector<std::vector<Uint32>> typingStates;
    std::vector<Uint32> typingStatesData;
    std::vector<Uint32> longWordHelper;
    size_t currentIndex;
    bool isInVietnameseMode;
    bool isTemporarilyOff;
    bool tempOffSpelling;
    bool skipMacroNextBreak;
    bool forceSkipMacroNextBreak;
    int consecutiveSpaceCount;

    EngineState() : currentIndex(0), isInVietnameseMode(true), isTemporarilyOff(false),
                   tempOffSpelling(false), skipMacroNextBreak(false),
                   forceSkipMacroNextBreak(false), consecutiveSpaceCount(0) {
        typingWord.resize(MAX_BUFF);
    }
};

// Hook state structure
struct EngineHookState {
    std::vector<Uint32> charData;
    std::vector<Uint32> macroData;
    std::vector<Uint32> extCode;
    Uint16 code;
    int backspaceCount;
    int newCharCount;
    std::wstring macroKey;
    bool hasMacro;

    EngineHookState() : code(0), backspaceCount(0), newCharCount(0), hasMacro(false) {
        charData.resize(MAX_BUFF);
        macroData.resize(MAX_BUFF);
        extCode.resize(MAX_BUFF);
    }
};

class EngineCore {
public:
    EngineCore();
    ~EngineCore();

    // Initialization and cleanup
    bool initialize(const EngineConfig& config);
    void shutdown();
    bool isInitialized() const;

    // Core event processing
    EngineHookState processKeyEvent(const vKeyEvent& event,
                                   const vKeyEventState& state,
                                   const Uint16& data,
                                   const Uint8& capsStatus = 0,
                                   const bool& otherControlKey = false);

    // Session management
    void startNewSession();
    void resetSession();
    bool hasActiveSession() const;

    // Configuration management
    void updateConfig(const EngineConfig& config);
    const EngineConfig& getConfig() const;

    // Mode management
    void setVietnameseMode(bool enabled);
    bool isVietnameseMode() const;

    void setTemporarilyOff(bool enabled);
    bool isTemporarilyOff() const;

    void setTempOffSpelling(bool enabled);
    bool isTempOffSpellingEnabled() const;

    // Macro control
    void skipMacroNextBreak();
    void forceSkipMacroNextBreak();
    void resetMacroSkip();

    // Character conversion
    Uint32 getCharacterCode(const Uint32& data) const;
    std::wstring convertStringEncoding(const std::wstring& input, int fromCodeTable, int toCodeTable) const;

    // Component access
    VietnameseProcessor& getVietnameseProcessor();
    MacroManager& getMacroManager();
    SmartSwitchKey& getSmartSwitchKey();
    ConvertTool& getConvertTool();

    // State queries
    size_t getCurrentWordLength() const;
    const std::vector<Uint32>& getCurrentWord() const;
    bool hasPendingMacro() const;

    // Performance monitoring
    void enablePerformanceMonitoring(bool enabled);
    bool isPerformanceMonitoringEnabled() const;
    void getPerformanceMetrics(uint64_t& totalEvents, uint64_t& processTime) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    // Internal processing methods
    bool shouldProcessEvent(const vKeyEvent& event, const vKeyEventState& state) const;
    bool isBreakCharacter(Uint16 keyCode) const;
    bool isMacroTriggerCharacter(Uint16 keyCode) const;

    // Word processing
    void processWordBreak();
    void processCharacter(Uint16 keyCode, bool isCaps, bool otherControlKey);
    void processBackspace();

    // State management
    void saveTypingState();
    void restoreTypingState(size_t stateIndex);
    void clearTypingWord();

    // Output generation
    void generateOutput(EngineHookState& hookState);
    void generateCharacterOutput(Uint32 character, EngineHookState& hookState);
    void generateMacroOutput(const std::wstring& macro, EngineHookState& hookState);

    // Configuration validation
    bool validateConfig(const EngineConfig& config) const;

    // Performance optimization
    mutable uint64_t eventCount_;
    mutable uint64_t totalProcessTime_;
    mutable bool performanceMonitoringEnabled_;
};

#endif /* EngineCore_h */