//
//  TypingEngine.cpp
//  EndKey
//
//  Created by Refactoring on 10/26/25.
//  Copyright © 2025 EndKey. All rights reserved.
//

#include "TypingEngine.h"
#include "VietnameseOptimized.h"
#include "SpellingEngine.h"
#include "MacroEngine.h"
#include "ConvertTool.h"
#include <chrono>
#include <algorithm>

namespace EndKey {
    namespace Engine {

        TypingEngine::TypingEngine() {
            initializeCharacterMappings();
            currentState_.typingWord.reserve(MAX_BUFF);
            currentState_.typingStates.reserve(50);
        }

        TypingEngine::~TypingEngine() {
            // Smart pointers will handle cleanup
        }

        void TypingEngine::initializeCharacterMappings() {
            // Initialize character key mappings
            charKeyCode_ = {
                KEY_BACKQUOTE, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0, KEY_MINUS, KEY_EQUALS,
                KEY_LEFT_BRACKET, KEY_RIGHT_BRACKET, KEY_BACK_SLASH,
                KEY_SEMICOLON, KEY_QUOTE, KEY_COMMA, KEY_DOT, KEY_SLASH
            };

            // Initialize break codes
            breakCode_ = {
                KEY_ESC, KEY_TAB, KEY_ENTER, KEY_RETURN, KEY_LEFT, KEY_RIGHT, KEY_DOWN, KEY_UP, KEY_COMMA, KEY_DOT,
                KEY_SLASH, KEY_SEMICOLON, KEY_QUOTE, KEY_BACK_SLASH, KEY_MINUS, KEY_EQUALS, KEY_BACKQUOTE, KEY_TAB
#if _WIN32
                , VK_INSERT, VK_HOME, VK_END, VK_DELETE, VK_PRIOR, VK_NEXT, VK_SNAPSHOT, VK_PRINT, VK_SELECT, VK_HELP,
                VK_EXECUTE, VK_NUMLOCK, VK_SCROLL
#endif
            };

            // Initialize macro break codes
            macroBreakCode_ = {
                // Control keys
                KEY_RETURN, KEY_SPACE, KEY_TAB,
                // Numbers (Shift + number = !, @, #, $, %, ^, &, *, (, ))
                KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0,
                // Punctuation and symbols
                KEY_COMMA, KEY_DOT, KEY_SLASH, KEY_SEMICOLON, KEY_QUOTE,
                KEY_BACK_SLASH, KEY_MINUS, KEY_EQUALS,
                KEY_LEFT_BRACKET, KEY_RIGHT_BRACKET, KEY_BACKQUOTE
            };
        }

        void TypingEngine::processKey(const vKeyEvent& event,
                                     const vKeyEventState& state,
                                     const Uint16& data,
                                     const Uint8& capsStatus,
                                     const bool& otherControlKey) {

            auto start_time = std::chrono::high_resolution_clock::now();

            // Early exit for non-typing events
            if (event != vKeyEvent::KeyDown) {
                return;
            }

            // Handle backspace specially
            if (data == KEY_BACKSPACE) {
                handleBackspace();
                updateTypingState();
                return;
            }

            // Check if this should break the current word
            if (shouldBreakWord(data)) {
                startNewSession();
            }

            // Process the character based on current configuration
            Uint32 processedChar = getCharacterCode(data);

            // Apply caps lock and shift
            if (capsStatus || (state & vKeyEventState::Shift)) {
                // Convert to uppercase if needed
                if (processedChar >= 'a' && processedChar <= 'z') {
                    processedChar = processedChar - 'a' + 'A';
                }
            }

            // Add to typing buffer
            if (currentState_.index < MAX_BUFF) {
                currentState_.typingWord[currentState_.index++] = processedChar;
            } else {
                // Handle overflow with long word helper
                currentState_.longWordHelper.push_back(processedChar);
            }

            updateTypingState();

            // Track performance
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

            // Log if processing takes too long (should be sub-millisecond)
            if (duration.count() > 1000) {
                // Performance warning would go here in debug mode
            }
        }

        void TypingEngine::handleBackspace() {
            if (currentState_.index > 0) {
                currentState_.index--;
            } else if (!currentState_.longWordHelper.empty()) {
                currentState_.longWordHelper.pop_back();
            }

            // Update typing states for undo functionality
            if (!currentState_.typingStates.empty()) {
                currentState_.typingStates.pop_back();
            }
        }

        void TypingEngine::startNewSession() {
            // Save current state for potential undo
            if (currentState_.index > 0) {
                std::vector<Uint32> stateCopy;
                stateCopy.assign(currentState_.typingWord.begin(),
                               currentState_.typingWord.begin() + currentState_.index);
                currentState_.typingStates.push_back(stateCopy);
            }

            // Reset current typing state
            currentState_.index = 0;
            currentState_.longWordHelper.clear();

            // Limit typing states to prevent memory bloat
            if (currentState_.typingStates.size() > 50) {
                currentState_.typingStates.erase(currentState_.typingStates.begin());
            }
        }

        void TypingEngine::resetTypingState() {
            currentState_.index = 0;
            currentState_.longWordHelper.clear();
            currentState_.typingStates.clear();
            currentState_.typingWord.clear();
            currentState_.isActive = false;
        }

        bool TypingEngine::shouldBreakWord(const Uint16& keyCode) const {
            return std::find(breakCode_.begin(), breakCode_.end(), keyCode) != breakCode_.end();
        }

        bool TypingEngine::isMarkKey(const Uint16& keyCode) const {
            if (config_.inputType == 1) { // VNI
                return keyCode == KEY_1 || keyCode == KEY_2 || keyCode == KEY_3 ||
                       keyCode == KEY_4 || keyCode == KEY_5;
            } else { // Telex variants
                return keyCode == KEY_S || keyCode == KEY_F || keyCode == KEY_R ||
                       keyCode == KEY_J || keyCode == KEY_X;
            }
        }

        bool TypingEngine::isBracketKey(const Uint16& key) const {
            return key == KEY_LEFT_BRACKET || key == KEY_RIGHT_BRACKET;
        }

        Uint32 TypingEngine::getCharacterCode(const Uint32& data) const {
            // Use ConvertTool for character code conversion
            static ConvertTool converter;
            return converter.getCharacterCode(data);
        }

        void TypingEngine::setInputConfig(const InputConfig& config) {
            config_ = config;
        }

        const TypingEngine::InputConfig& TypingEngine::getInputConfig() const {
            return config_;
        }

        const TypingEngine::TypingState& TypingEngine::getCurrentState() const {
            return currentState_;
        }

        void TypingEngine::updateTypingState() {
            currentState_.isActive = (currentState_.index > 0 || !currentState_.longWordHelper.empty());
        }

        void TypingEngine::optimizeMemory() {
            // Clean up unused typing states
            cleanupUnusedStates();

            // Compact typing states to reduce memory fragmentation
            compactTypingStates();

            // Shrink vectors if they're using excessive capacity
            if (currentState_.typingStates.capacity() > currentState_.typingStates.size() * 2) {
                currentState_.typingStates.shrink_to_fit();
            }

            if (currentState_.longWordHelper.capacity() > currentState_.longWordHelper.size() * 2) {
                currentState_.longWordHelper.shrink_to_fit();
            }
        }

        void TypingEngine::cleanupUnusedStates() {
            // Remove old states that are unlikely to be needed
            const size_t maxStates = 20;
            if (currentState_.typingStates.size() > maxStates) {
                auto eraseBegin = currentState_.typingStates.begin();
                auto eraseEnd = currentState_.typingStates.begin() + (currentState_.typingStates.size() - maxStates);
                currentState_.typingStates.erase(eraseBegin, eraseEnd);
            }
        }

        void TypingEngine::compactTypingStates() {
            // Remove empty states and merge similar ones
            currentState_.typingStates.erase(
                std::remove_if(currentState_.typingStates.begin(), currentState_.typingStates.end(),
                    [](const std::vector<Uint32>& state) {
                        return state.empty();
                    }),
                currentState_.typingStates.end()
            );
        }

        size_t TypingEngine::getMemoryUsage() const {
            size_t total = 0;

            // Main typing word buffer
            total += currentState_.typingWord.capacity() * sizeof(Uint32);

            // Long word helper
            total += currentState_.longWordHelper.capacity() * sizeof(Uint32);

            // Typing states
            for (const auto& state : currentState_.typingStates) {
                total += state.capacity() * sizeof(Uint32);
            }
            total += currentState_.typingStates.capacity() * sizeof(std::vector<Uint32>);

            // Character mappings
            total += charKeyCode_.capacity() * sizeof(Uint8);
            total += breakCode_.capacity() * sizeof(Uint8);
            total += macroBreakCode_.capacity() * sizeof(Uint8);

            return total;
        }

    } // namespace Engine
} // namespace EndKey