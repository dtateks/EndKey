//
//  IMacroProcessor.h
//  EndKey
//
//  Created by Hive Coder Agent on 10/26/25.
//  Copyright © 2025 EndKey. All rights reserved.
//

#ifndef IMacroProcessor_h
#define IMacroProcessor_h

#include "DataType.h"
#include <vector>
#include <string>

namespace EndKey {
    namespace Core {
        /**
         * Interface for macro processing
         * Handles text expansion and macro replacement
         */
        class IMacroProcessor {
        public:
            virtual ~IMacroProcessor() = default;

            /**
             * Check if current buffer matches any macro
             * @param buffer Current typing buffer
             * @return Macro key if found, empty string otherwise
             */
            virtual std::string findMacro(const std::vector<Uint32>& buffer) = 0;

            /**
             * Get macro expansion for given key
             * @param macroKey The macro key
             * @param isCaps Whether to capitalize
             * @return Expanded text
             */
            virtual std::string expandMacro(const std::string& macroKey, bool isCaps = false) = 0;

            /**
             * Add or update a macro
             * @param key Macro key
             * @param value Macro value
             * @return true if successful
             */
            virtual bool addMacro(const std::string& key, const std::string& value) = 0;

            /**
             * Remove a macro
             * @param key Macro key to remove
             * @return true if removed successfully
             */
            virtual bool removeMacro(const std::string& key) = 0;

            /**
             * Load macros from storage
             * @return true if loaded successfully
             */
            virtual bool loadMacros() = 0;

            /**
             * Save macros to storage
             * @return true if saved successfully
             */
            virtual bool saveMacros() = 0;

            /**
             * Enable/disable macro processing
             */
            virtual void setMacroEnabled(bool enabled) = 0;

            /**
             * Enable/disable macro in English mode
             */
            virtual void setMacroInEnglishModeEnabled(bool enabled) = 0;

            /**
             * Enable/disable auto-capitalization
             */
            virtual void setAutoCapsEnabled(bool enabled) = 0;

            /**
             * Check if macro processing is enabled
             */
            virtual bool isMacroEnabled() const = 0;

            /**
             * Skip macro on next word break
             */
            virtual void skipNextMacro() = 0;

            /**
             * Force skip macro on next word break
             */
            virtual void forceSkipNextMacro() = 0;
        };
    }
}

#endif /* IMacroProcessor_h */