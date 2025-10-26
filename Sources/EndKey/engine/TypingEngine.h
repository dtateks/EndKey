//
//  TypingEngine.h
//  EndKey
//
//  Created by Refactoring on 10/26/25.
//  Copyright © 2025 EndKey. All rights reserved.
//

#ifndef TypingEngine_h
#define TypingEngine_h

#include "DataType.h"
#include <vector>
#include <memory>

namespace EndKey {
    namespace Engine {

        /**
         * TypingEngine handles keystroke processing and input method coordination
         * Manages typing state, character buffers, and input method switching
         */
        class TypingEngine {
        public:
            struct TypingState {
                std::vector<Uint32> typingWord;
                std::vector<Uint32> longWordHelper;
                std::vector<std::vector<Uint32>> typingStates;
                Uint8 index = 0;
                bool isActive = false;
            };

            struct InputConfig {
                int inputType;      // Telex, VNI, Simple Telex
                int language;       // English, Vietnamese
                int codeTable;      // Unicode, TCVN3, VNI-Windows
                bool freeMark;
                bool checkSpelling;
                bool useModernOrthography;
            };

        public:
            TypingEngine();
            ~TypingEngine();

            // Core typing operations
            void processKey(const vKeyEvent& event,
                          const vKeyEventState& state,
                          const Uint16& data,
                          const Uint8& capsStatus = 0,
                          const bool& otherControlKey = false);

            void startNewSession();
            void resetTypingState();

            // Input method configuration
            void setInputConfig(const InputConfig& config);
            const InputConfig& getInputConfig() const;

            // State management
            const TypingState& getCurrentState() const;
            bool isTyping() const { return currentState_.index > 0; }

            // Character processing
            Uint32 getCharacterCode(const Uint32& data) const;
            bool shouldBreakWord(const Uint16& keyCode) const;

            // Memory optimization
            void optimizeMemory();
            size_t getMemoryUsage() const;

        private:
            TypingState currentState_;
            InputConfig config_;

            // Character code mappings
            std::vector<Uint8> charKeyCode_;
            std::vector<Uint8> breakCode_;
            std::vector<Uint8> macroBreakCode_;

            // Input method processing tables
            static constexpr Uint16 PROCESSING_CHAR[][11] = {
                {KEY_S, KEY_F, KEY_R, KEY_X, KEY_J, KEY_A, KEY_O, KEY_E, KEY_W, KEY_D, KEY_Z}, // Telex
                {KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0}, // VNI
                {KEY_S, KEY_F, KEY_R, KEY_X, KEY_J, KEY_A, KEY_O, KEY_E, KEY_W, KEY_D, KEY_Z}, // Simple Telex 1
                {KEY_S, KEY_F, KEY_R, KEY_X, KEY_J, KEY_A, KEY_O, KEY_E, KEY_W, KEY_D, KEY_Z}  // Simple Telex 2
            };

            // Helper methods
            void initializeCharacterMappings();
            bool isMarkKey(const Uint16& keyCode) const;
            bool isBracketKey(const Uint16& key) const;
            void handleBackspace();
            void updateTypingState();

            // Memory management
            void cleanupUnusedStates();
            void compactTypingStates();
        };

    } // namespace Engine
} // namespace EndKey

#endif /* TypingEngine_h */