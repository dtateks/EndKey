//
//  EngineCore.h
//  EndKey
//
//  Created by Hive Coder Agent on 10/26/25.
//  Copyright © 2025 EndKey. All rights reserved.
//

#ifndef EngineCore_h
#define EngineCore_h

#include "interfaces/IEngineCore.h"
#include <memory>
#include <vector>

namespace EndKey {
    namespace Core {
        /**
         * Main Engine Implementation
         * Coordinates all processing components using dependency injection
         */
        class EngineCore : public IEngineCore {
        private:
            std::unique_ptr<IConfigurationManager> configManager;
            std::unique_ptr<IVietnameseProcessor> vietnameseProcessor;
            std::unique_ptr<IMacroProcessor> macroProcessor;
            std::unique_ptr<IInputProcessor> inputProcessor;

            // Internal state
            bool initialized;
            vKeyHookState currentHookState;
            std::vector<Uint32> typingBuffer;
            bool skipNextMacroFlag;
            bool forceSkipNextMacroFlag;

            // Helper methods
            void initializeHookState();
            HoolCodeState processMainKey(const Uint16& data, const bool& otherControlKey);
            HoolCodeState processEnglishKey(const Uint16& data, const bool& isCaps, const bool& otherControlKey);
            void updateHookState(HoolCodeState result, const std::vector<Uint32>& newChars, int backspaceCount = 0);
            bool shouldProcessInEnglishMode() const;

        public:
            EngineCore(
                std::unique_ptr<IConfigurationManager> configMgr,
                std::unique_ptr<IVietnameseProcessor> vietProcessor,
                std::unique_ptr<IMacroProcessor> macroProc,
                std::unique_ptr<IInputProcessor> inputProc
            );
            virtual ~EngineCore() = default;

            // IEngineCore interface
            bool initialize() override;
            void processKeyEvent(
                const vKeyEvent& event,
                const vKeyEventState& state,
                const Uint16& data,
                const Uint8& capsStatus = 0,
                const bool& otherControlKey = false
            ) override;
            vKeyHookState getProcessingResult() override;
            void startNewSession() override;
            Uint32 getCharacterCode(const Uint32& data) override;

            // Component access
            IConfigurationManager* getConfigurationManager() override;
            IVietnameseProcessor* getVietnameseProcessor() override;
            IMacroProcessor* getMacroProcessor() override;

            void shutdown() override;
            bool isReady() const override;

            // Engine-specific methods for backward compatibility
            void vEnglishMode(const vKeyEventState& state, const Uint16& data, const bool& isCaps, const bool& otherControlKey);
            void vTempOffSpellChecking();
            void vSetCheckSpelling();
            void vSkipMacroNextBreak();
            void vForceSkipMacroNextBreak();
        };

        /**
         * Engine Factory Implementation
         * Creates all engine components with proper dependencies
         */
        class EngineFactory : public IEngineFactory {
        public:
            EngineFactory();
            virtual ~EngineFactory() = default;

            std::unique_ptr<IEngineCore> createEngine() override;
            std::unique_ptr<IConfigurationManager> createConfigurationManager() override;
            std::unique_ptr<IVietnameseProcessor> createVietnameseProcessor() override;
            std::unique_ptr<IMacroProcessor> createMacroProcessor() override;
            std::unique_ptr<IInputProcessor> createInputProcessor() override;
        };
    }
}

#endif /* EngineCore_h */