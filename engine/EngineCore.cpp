//
//  EngineCore.cpp
//  EndKey
//
//  Created by EndKey Refactoring on 2025.
//  Copyright © 2025 Tuyen Mai. All rights reserved.
//

#include "EngineCore.h"
#include <algorithm>
#include <stdexcept>
#include <chrono>

// EngineCore::Impl structure with RAII pattern
struct EngineCore::Impl {
    // Core components - use smart pointers for automatic cleanup
    std::unique_ptr<VietnameseProcessor> vietnameseProcessor;
    std::unique_ptr<MacroManager> macroManager;
    std::unique_ptr<SmartSwitchKey> smartSwitchKey;
    std::unique_ptr<ConvertTool> convertTool;

    // Configuration and state
    EngineConfig config;
    EngineState state;
    EngineHookState hookState;

    // Performance monitoring
    uint64_t eventCount;
    uint64_t totalProcessTime;
    bool performanceMonitoringEnabled;
    auto startTime;

    // Initialization flag
    bool initialized;

    // Constructor with RAII initialization
    Impl() : eventCount(0), totalProcessTime(0), performanceMonitoringEnabled(false), initialized(false) {
        try {
            // Initialize components with exception safety
            vietnameseProcessor = std::make_unique<VietnameseProcessor>();
            macroManager = std::make_unique<MacroManager>();
            smartSwitchKey = std::make_unique<SmartSwitchKey>();
            convertTool = std::make_unique<ConvertTool>();

            // Initialize default configuration
            initializeDefaultConfig();

            initialized = true;
        } catch (const std::exception& e) {
            // Automatic cleanup via smart pointers if initialization fails
            throw std::runtime_error("Failed to initialize EngineCore: " + std::string(e.what()));
        }
    }

    // Destructor - automatic cleanup via smart pointers
    ~Impl() {
        cleanup();
    }

    // Initialize default configuration
    void initializeDefaultConfig() {
        config.language = 1; // Vietnamese
        config.inputType = 0; // Telex
        config.freeMark = 1; // Yes
        config.codeTable = 0; // Unicode
        config.switchKeyStatus = 0;
        config.checkSpelling = 1; // Yes
        config.useModernOrthography = 1; // Yes
        config.quickTelex = 1; // Yes
        config.restoreIfWrongSpelling = 1; // Yes
        config.fixRecommendBrowser = 1; // Yes
        config.useMacro = 1; // Yes
        config.useMacroInEnglishMode = 0; // No
        config.autoCapsMacro = 1; // Yes
        config.useSmartSwitchKey = 1; // Yes
        config.upperCaseFirstChar = 0; // No
        config.tempOffSpelling = 1; // Yes
        config.allowConsonantZFWJ = 1; // Yes
        config.quickStartConsonant = 1; // Yes
        config.quickEndConsonant = 1; // Yes
        config.rememberCode = 1; // Yes
        config.otherLanguage = 1; // Yes
        config.tempOffEndKey = 1; // Yes
        config.doubleSpacePeriod = 1; // Yes
    }

    // Apply configuration to components
    void applyConfiguration() {
        if (!vietnameseProcessor || !macroManager || !smartSwitchKey || !convertTool) {
            throw std::runtime_error("Components not initialized");
        }

        vietnameseProcessor->setModernOrthography(config.useModernOrthography == 1);
        macroManager->setMacroEnabled(config.useMacro == 1);
        macroManager->setMacroInEnglishMode(config.useMacroInEnglishMode == 1);
        macroManager->setAutoCapitalizeMacro(config.autoCapsMacro == 1);

        smartSwitchKey->setEnabled(config.useSmartSwitchKey == 1);

        state.isInVietnameseMode = (config.language == 1);
    }

    // Cleanup resources
    void cleanup() {
        // Smart pointers will automatically clean up components
        // Any additional cleanup can be added here
        eventCount = 0;
        totalProcessTime = 0;
        performanceMonitoringEnabled = false;
    }

    // Performance monitoring
    void startPerformanceTimer() {
        if (performanceMonitoringEnabled) {
            startTime = std::chrono::high_resolution_clock::now();
        }
    }

    void endPerformanceTimer() {
        if (performanceMonitoringEnabled) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
            totalProcessTime += duration.count();
            eventCount++;
        }
    }
};

// EngineCore public implementation
EngineCore::EngineCore() : pImpl(std::make_unique<Impl>()) {
}

EngineCore::~EngineCore() {
    // pImpl automatically cleaned up by unique_ptr destructor
}

bool EngineCore::initialize(const EngineConfig& config) {
    if (!pImpl || !pImpl->initialized) {
        return false;
    }

    try {
        pImpl->config = config;
        pImpl->applyConfiguration();
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

void EngineCore::shutdown() {
    if (pImpl) {
        pImpl->cleanup();
    }
}

bool EngineCore::isInitialized() const {
    return pImpl && pImpl->initialized;
}

EngineHookState EngineCore::processKeyEvent(const vKeyEvent& event,
                                           const vKeyEventState& state,
                                           const Uint16& data,
                                           const Uint8& capsStatus,
                                           const bool& otherControlKey) {
    if (!isInitialized()) {
        return EngineHookState();
    }

    pImpl->startPerformanceTimer();

    EngineHookState result;

    try {
        // Check if we should process this event
        if (!shouldProcessEvent(event, state)) {
            pImpl->endPerformanceTimer();
            return result;
        }

        // Handle different event types
        switch (event) {
            case vKeyEvent::KEY_DOWN:
                processCharacter(data, capsStatus, otherControlKey);
                break;

            case vKeyEvent::KEY_UP:
                // Handle key up if needed
                break;

            case vKeyEvent::BACKSPACE:
                processBackspace();
                break;

            default:
                break;
        }

        // Generate output
        generateOutput(result);

    } catch (const std::exception& e) {
        // Handle exceptions gracefully
        result.code = 0;
    }

    pImpl->endPerformanceTimer();
    return result;
}

void EngineCore::startNewSession() {
    if (!isInitialized()) return;

    pImpl->state.currentIndex = 0;
    pImpl->state.typingWord.clear();
    pImpl->state.typingWord.resize(MAX_BUFF);
    pImpl->state.longWordHelper.clear();
    pImpl->state.typingStates.clear();
    pImpl->state.typingStatesData.clear();
    pImpl->state.consecutiveSpaceCount = 0;
    pImpl->state.skipMacroNextBreak = false;
    pImpl->state.forceSkipMacroNextBreak = false;

    // Reset hook state
    pImpl->hookState = EngineHookState();
}

void EngineCore::processEnglishMode(const vKeyEventState& state, const Uint16& data, const bool& isCaps, const bool& otherControlKey) {
    if (!isInitialized()) return;
    
    // In English mode, process macro but not Vietnamese processing
    if (pImpl->config.useMacro && pImpl->config.useMacroInEnglishMode) {
        // Process macro in English mode
        // TODO: Implement English mode macro processing
    }
    
    // Generate output without Vietnamese processing
    generateOutput(pImpl->hookState);
}

void EngineCore::setVietnameseMode(bool enabled) {
    if (!isInitialized()) return;
    pImpl->state.isInVietnameseMode = enabled;
    pImpl->config.language = enabled ? 1 : 0;
}

bool EngineCore::isVietnameseMode() const {
    return isInitialized() && pImpl->state.isInVietnameseMode;
}

void EngineCore::setTemporarilyOff(bool enabled) {
    if (!isInitialized()) return;
    pImpl->state.isTemporarilyOff = enabled;
}

bool EngineCore::isTemporarilyOff() const {
    return isInitialized() && pImpl->state.isTemporarilyOff;
}

void EngineCore::skipMacroNextBreak() {
    if (!isInitialized()) return;
    if (pImpl->state.currentIndex > 0) {
        pImpl->state.skipMacroNextBreak = true;
    }
}

void EngineCore::forceSkipMacroNextBreak() {
    if (!isInitialized()) return;
    pImpl->state.forceSkipMacroNextBreak = true;
}

Uint32 EngineCore::getCharacterCode(const Uint32& data) const {
    if (!isInitialized() || !pImpl->vietnameseProcessor) {
        return data;
    }

    // Use Vietnamese processor for character conversion
    return pImpl->vietnameseProcessor->convertCharacterEncoding(
        data, pImpl->config.codeTable, 0); // Convert to Unicode
}

VietnameseProcessor& EngineCore::getVietnameseProcessor() {
    if (!isInitialized() || !pImpl->vietnameseProcessor) {
        throw std::runtime_error("VietnameseProcessor not initialized");
    }
    return *pImpl->vietnameseProcessor;
}

MacroManager& EngineCore::getMacroManager() {
    if (!isInitialized() || !pImpl->macroManager) {
        throw std::runtime_error("MacroManager not initialized");
    }
    return *pImpl->macroManager;
}

SmartSwitchKey& EngineCore::getSmartSwitchKey() {
    if (!isInitialized() || !pImpl->smartSwitchKey) {
        throw std::runtime_error("SmartSwitchKey not initialized");
    }
    return *pImpl->smartSwitchKey;
}

ConvertTool& EngineCore::getConvertTool() {
    if (!isInitialized() || !pImpl->convertTool) {
        throw std::runtime_error("ConvertTool not initialized");
    }
    return *pImpl->convertTool;
}

size_t EngineCore::getCurrentWordLength() const {
    return isInitialized() ? pImpl->state.currentIndex : 0;
}

const std::vector<Uint32>& EngineCore::getCurrentWord() const {
    static std::vector<Uint32> empty;
    return isInitialized() ? pImpl->state.typingWord : empty;
}

void EngineCore::enablePerformanceMonitoring(bool enabled) {
    if (isInitialized()) {
        pImpl->performanceMonitoringEnabled = enabled;
    }
}

bool EngineCore::isPerformanceMonitoringEnabled() const {
    return isInitialized() && pImpl->performanceMonitoringEnabled;
}

// Private helper methods
bool EngineCore::shouldProcessEvent(const vKeyEvent& event, const vKeyEventState& state) const {
    if (!isInitialized()) return false;

    // Don't process if temporarily off
    if (pImpl->state.isTemporarilyOff) return false;

    // Don't process if in English mode and macro is disabled
    if (!pImpl->state.isInVietnameseMode && !pImpl->config.useMacroInEnglishMode) {
        return false;
    }

    return true;
}

bool EngineCore::isBreakCharacter(Uint16 keyCode) const {
    // Define break characters that trigger word processing
    static const std::vector<Uint16> breakChars = {
        KEY_SPACE, KEY_ENTER, KEY_RETURN, KEY_TAB, KEY_ESC,
        KEY_COMMA, KEY_DOT, KEY_SEMICOLON, KEY_QUOTE,
        KEY_LEFT_BRACKET, KEY_RIGHT_BRACKET, KEY_MINUS, KEY_EQUALS
    };

    return std::find(breakChars.begin(), breakChars.end(), keyCode) != breakChars.end();
}

void EngineCore::processCharacter(Uint16 keyCode, bool isCaps, bool otherControlKey) {
    if (!isInitialized()) return;

    // Check if this is a break character
    if (isBreakCharacter(keyCode)) {
        processWordBreak();
        return;
    }

    // Add character to current word
    if (pImpl->state.currentIndex < MAX_BUFF) {
        Uint32 charCode = keyCode;
        if (isCaps) {
            charCode |= 0x10000; // Set caps flag
        }

        pImpl->state.typingWord[pImpl->state.currentIndex++] = charCode;

        // Process Vietnamese character if needed
        if (pImpl->state.isInVietnameseMode && pImpl->vietnameseProcessor) {
            // Vietnamese processing logic here
        }
    }
}

void EngineCore::processWordBreak() {
    if (!isInitialized()) return;

    // Check for macro expansion
    if (pImpl->macroManager && pImpl->config.useMacro) {
        // Convert current word to string for macro matching
        std::wstring currentWord;
        for (size_t i = 0; i < pImpl->state.currentIndex; ++i) {
            currentWord += static_cast<wchar_t>(pImpl->state.typingWord[i] & 0xFFFF);
        }

        if (pImpl->macroManager->hasMacro(currentWord)) {
            // Expand macro
            std::wstring expansion = pImpl->macroManager->expandMacro(currentWord);
            // Handle macro expansion
        }
    }

    // Reset for next word
    startNewSession();
}

void EngineCore::generateOutput(EngineHookState& hookState) {
    if (!isInitialized()) return;

    // Copy state to hook state for output
    hookState.charData = pImpl->state.typingWord;
    hookState.backspaceCount = pImpl->hookState.backspaceCount;
    hookState.newCharCount = pImpl->hookState.newCharCount;
    hookState.code = pImpl->hookState.code;
    hookState.hasMacro = pImpl->hookState.hasMacro;
}