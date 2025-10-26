//
//  IVietnameseProcessor.h
//  EndKey
//
//  Created by Hive Coder Agent on 10/26/25.
//  Copyright © 2025 EndKey. All rights reserved.
//

#ifndef IVietnameseProcessor_h
#define IVietnameseProcessor_h

#include "DataType.h"
#include <vector>

namespace EndKey {
    namespace Core {
        /**
         * Interface for Vietnamese language processing
         * Handles tone marks, character conversion, and spelling
         */
        class IVietnameseProcessor {
        public:
            virtual ~IVietnameseProcessor() = default;

            /**
             * Process Vietnamese character input
             * @param keyCode The input key code
             * @param currentBuffer Current typing buffer
             * @param inputType Input method (Telex, VNI, etc.)
             * @return Processing result
             */
            virtual HoolCodeState processCharacter(
                const Uint16& keyCode,
                std::vector<Uint32>& currentBuffer,
                const vKeyInputType& inputType
            ) = 0;

            /**
             * Convert character code to actual character
             * @param data Character data with masks
             * @return Unicode character
             */
            virtual Uint32 convertToCharacter(const Uint32& data) = 0;

            /**
             * Check spelling of current word
             * @param buffer Current word buffer
             * @return true if spelling is correct
             */
            virtual bool checkSpelling(const std::vector<Uint32>& buffer) = 0;

            /**
             * Apply modern orthography rules
             * @param buffer Character buffer to modify
             */
            virtual void applyModernOrthography(std::vector<Uint32>& buffer) = 0;

            /**
             * Set input type (Telex, VNI, Simple Telex)
             */
            virtual void setInputType(const vKeyInputType& type) = 0;

            /**
             * Enable/disable spelling check
             */
            virtual void setSpellCheckingEnabled(bool enabled) = 0;

            /**
             * Enable/disable modern orthography
             */
            virtual void setModernOrthographyEnabled(bool enabled) = 0;
        };
    }
}

#endif /* IVietnameseProcessor_h */