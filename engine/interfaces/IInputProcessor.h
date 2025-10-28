//
//  IInputProcessor.h
//  EndKey
//
//  Created by Hive Coder Agent on 10/26/25.
//  Copyright © 2025 EndKey. All rights reserved.
//

#ifndef IInputProcessor_h
#define IInputProcessor_h

#include "DataType.h"

namespace EndKey {
    namespace Core {
        /**
         * Interface for processing input events
         * Follows Single Responsibility Principle
         */
        class IInputProcessor {
        public:
            virtual ~IInputProcessor() = default;

            /**
             * Process a keyboard event
             * @param event The keyboard event type
             * @param state The event state (down/up)
             * @param data The key code
             * @param capsStatus Caps lock status
             * @param otherControlKey Other modifier keys
             * @return Processing result
             */
            virtual HoolCodeState processKeyEvent(
                const vKeyEvent& event,
                const vKeyEventState& state,
                const Uint16& data,
                const Uint8& capsStatus = 0,
                const bool& otherControlKey = false
            ) = 0;

            /**
             * Start a new input session
             */
            virtual void startNewSession() = 0;

            /**
             * Reset the processor state
             */
            virtual void reset() = 0;

            /**
             * Check if processor is active
             */
            virtual bool isActive() const = 0;
        };
    }
}

#endif /* IInputProcessor_h */