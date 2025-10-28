//
//  InputProcessor.h
//  EndKey
//
//  Created by Hive Coder Agent on 10/26/25.
//  Copyright © 2025 EndKey. All rights reserved.
//

#ifndef InputProcessor_h
#define InputProcessor_h

#include "interfaces/IInputProcessor.h"
#include "interfaces/IConfigurationManager.h"
#include "interfaces/IVietnameseProcessor.h"
#include "interfaces/IMacroProcessor.h"
#include <memory>
#include <vector>

namespace EndKey {
    namespace Core {
        /**
         * Input Processor Implementation
         * Handles keyboard event processing and coordination
         */
        class InputProcessor : public IInputProcessor {
        private:
            IConfigurationManager* configManager;
            IVietnameseProcessor* vietnameseProcessor;
            IMacroProcessor* macroProcessor;

            bool active;
            std::vector<Uint32> internalBuffer;
            std::vector<Uint8> charKeyCode;
            std::vector<Uint8> breakCode;
            std::vector<Uint8> macroBreakCode;

            void initializeKeyMaps();
            bool isBreakKey(const Uint16& keyCode) const;
            bool isMacroBreakKey(const Uint16& keyCode) const;
            bool isCharacterKey(const Uint16& keyCode) const;
            bool shouldProcessAsVietnamese(const Uint16& keyCode) const;

        public:
            InputProcessor(
                IConfigurationManager* configMgr,
                IVietnameseProcessor* vietProcessor,
                IMacroProcessor* macroProc
            );
            virtual ~InputProcessor() = default;

            // IInputProcessor interface
            HoolCodeState processKeyEvent(
                const vKeyEvent& event,
                const vKeyEventState& state,
                const Uint16& data,
                const Uint8& capsStatus = 0,
                const bool& otherControlKey = false
            ) override;

            void startNewSession() override;
            void reset() override;
            bool isActive() const override;

            // Buffer management
            const std::vector<Uint32>& getBuffer() const;
            void setBuffer(const std::vector<Uint32>& newBuffer);

            // Character utilities
            static Uint32 convertToCharData(const Uint16& keyCode, bool isCaps = false);
            static bool isCapsLockOn(const Uint8& capsStatus);
        };
    }
}

#endif /* InputProcessor_h */