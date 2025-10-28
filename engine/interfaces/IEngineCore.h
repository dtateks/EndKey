//
//  IEngineCore.h
//  EndKey
//
//  Created by Hive Coder Agent on 10/26/25.
//  Copyright © 2025 EndKey. All rights reserved.
//

#ifndef IEngineCore_h
#define IEngineCore_h

#include "DataType.h"
#include "IInputProcessor.h"
#include "IVietnameseProcessor.h"
#include "IMacroProcessor.h"
#include "IConfigurationManager.h"

namespace EndKey {
    namespace Core {
        /**
         * Main engine interface following Dependency Inversion Principle
         * Coordinates all processing components
         */
        class IEngineCore {
        public:
            virtual ~IEngineCore() = default;

            /**
             * Initialize the engine with all components
             * @return true if initialization successful
             */
            virtual bool initialize() = 0;

            /**
             * Main entry point for key event processing
             * @param event The event type
             * @param state The event state
             * @param data The key code
             * @param capsStatus Caps lock status
             * @param otherControlKey Other modifier keys
             */
            virtual void processKeyEvent(
                const vKeyEvent& event,
                const vKeyEventState& state,
                const Uint16& data,
                const Uint8& capsStatus = 0,
                const bool& otherControlKey = false
            ) = 0;

            /**
             * Get processing result for the current event
             * @return Hook state with processing result
             */
            virtual vKeyHookState getProcessingResult() = 0;

            /**
             * Start a new typing session
             */
            virtual void startNewSession() = 0;

            /**
             * Convert character data to actual character
             * @param data Character data with masks
             * @return Unicode character
             */
            virtual Uint32 getCharacterCode(const Uint32& data) = 0;

            /**
             * Get configuration manager
             */
            virtual IConfigurationManager* getConfigurationManager() = 0;

            /**
             * Get Vietnamese processor
             */
            virtual IVietnameseProcessor* getVietnameseProcessor() = 0;

            /**
             * Get macro processor
             */
            virtual IMacroProcessor* getMacroProcessor() = 0;

            /**
             * Shutdown the engine and cleanup resources
             */
            virtual void shutdown() = 0;

            /**
             * Check if engine is initialized and ready
             */
            virtual bool isReady() const = 0;
        };

        /**
         * Factory interface for creating engine components
         * Follows Factory Pattern for dependency injection
         */
        class IEngineFactory {
        public:
            virtual ~IEngineFactory() = default;

            virtual std::unique_ptr<IEngineCore> createEngine() = 0;
            virtual std::unique_ptr<IConfigurationManager> createConfigurationManager() = 0;
            virtual std::unique_ptr<IVietnameseProcessor> createVietnameseProcessor() = 0;
            virtual std::unique_ptr<IMacroProcessor> createMacroProcessor() = 0;
            virtual std::unique_ptr<IInputProcessor> createInputProcessor() = 0;
        };
    }
}

#endif /* IEngineCore_h */