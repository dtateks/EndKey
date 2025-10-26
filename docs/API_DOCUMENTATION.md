# EndKey API Documentation

## Table of Contents
1. [Core API Reference](#core-api-reference)
2. [TypingEngine API](#typingengine-api)
3. [SpellingEngine API](#spellingengine-api)
4. [MacroEngine API](#macroengine-api)
5. [VietnameseProcessor API](#vietnameseprocessor-api)
6. [ConfigurationManager API](#configurationmanager-api)
7. [MemoryManager API](#memorymanager-api)
8. [Utility APIs](#utility-apis)
9. [Examples and Usage](#examples-and-usage)

## Core API Reference

### EngineCore Class

The main coordination hub for the EndKey input method engine.

```cpp
#include "EngineCore.h"
```

#### Public Methods

##### Initialization

```cpp
bool initialize(const EngineConfig& config)
```
**Description**: Initialize the engine with the provided configuration.
**Parameters**:
- `config`: Engine configuration structure containing all settings
**Returns**: `true` if initialization successful, `false` otherwise
**Example**:
```cpp
EngineCore engine;
EngineConfig config = {
    .language = 1,           // Vietnamese
    .inputType = 0,          // Telex
    .checkSpelling = 1,      // Enabled
    .useMacro = 1            // Enabled
};

if (!engine.initialize(config)) {
    // Handle initialization error
}
```

```cpp
void shutdown()
```
**Description**: Cleanly shutdown the engine and release all resources.
**Parameters**: None
**Returns**: void

```cpp
bool isInitialized() const
```
**Description**: Check if the engine is properly initialized.
**Parameters**: None
**Returns**: `true` if initialized, `false` otherwise

##### Event Processing

```cpp
EngineHookState processKeyEvent(const vKeyEvent& event,
                               const vKeyEventState& state,
                               const Uint16& data,
                               const Uint8& capsStatus = 0,
                               const bool& otherControlKey = false)
```
**Description**: Main entry point for processing keyboard events.
**Parameters**:
- `event`: Key event information
- `state`: Current keyboard state (modifier keys, etc.)
- `data`: Raw key code
- `capsStatus`: Caps lock status (optional, default 0)
- `otherControlKey`: Other control key status (optional, default false)
**Returns**: `EngineHookState` structure containing processing results

**Example**:
```cpp
vKeyEvent event = {KEY_DOWN, 0};
vKeyEventState state = {0, 0, 0, 0}; // No modifiers
Uint16 keyCode = 'a';
EngineHookState result = engine.processKeyEvent(event, state, keyCode);

if (result.newCharCount > 0) {
    // Process output characters
    for (int i = 0; i < result.newCharCount; i++) {
        Uint32 charCode = result.charData[i];
        // Handle character output
    }
}
```

##### Session Management

```cpp
void startNewSession()
```
**Description**: Start a new typing session, clearing all state.
**Parameters**: None
**Returns**: void

```cpp
void resetSession()
```
**Description**: Reset current session to initial state.
**Parameters**: None
**Returns**: void

```cpp
bool hasActiveSession() const
```
**Description**: Check if there's an active typing session.
**Parameters**: None
**Returns**: `true` if active session exists, `false` otherwise

##### Configuration Management

```cpp
void updateConfig(const EngineConfig& config)
```
**Description**: Update engine configuration at runtime.
**Parameters**:
- `config`: New configuration settings
**Returns**: void

```cpp
const EngineConfig& getConfig() const
```
**Description**: Get current engine configuration.
**Parameters**: None
**Returns**: const reference to current configuration

##### Mode Management

```cpp
void setVietnameseMode(bool enabled)
```
**Description**: Enable or disable Vietnamese input mode.
**Parameters**:
- `enabled`: true to enable Vietnamese mode, false for English
**Returns**: void

```cpp
bool isVietnameseMode() const
```
**Description**: Check if Vietnamese mode is currently enabled.
**Parameters**: None
**Returns**: `true` if Vietnamese mode is enabled, `false` otherwise

##### Component Access

```cpp
VietnameseProcessor& getVietnameseProcessor()
```
**Description**: Get access to the Vietnamese processor component.
**Parameters**: None
**Returns**: Reference to VietnameseProcessor instance

```cpp
MacroManager& getMacroManager()
```
**Description**: Get access to the macro manager component.
**Parameters**: None
**Returns**: Reference to MacroManager instance

## TypingEngine API

### Class Definition

```cpp
namespace EndKey {
    namespace Engine {
        class TypingEngine {
            // ... implementation
        };
    }
}
```

#### Configuration Structures

```cpp
struct InputConfig {
    int inputType;      // 0: Telex, 1: VNI, 2: Simple Telex
    int language;       // 0: English, 1: Vietnamese
    int codeTable;      // 0: Unicode, 1: TCVN3, 2: VNI-Windows
    bool freeMark;      // Allow tone mark free placement
    bool checkSpelling; // Enable spelling checking
    bool useModernOrthography; // Use modern Vietnamese orthography
};
```

#### Public Methods

##### Core Operations

```cpp
void processKey(const vKeyEvent& event,
               const vKeyEventState& state,
               const Uint16& data,
               const Uint8& capsStatus = 0,
               const bool& otherControlKey = false)
```
**Description**: Process a single key event in the typing context.
**Parameters**:
- `event`: Key event information
- `state`: Keyboard modifier state
- `data`: Raw key code
- `capsStatus`: Caps lock status
- `otherControlKey`: Other control key status
**Returns**: void

**Example**:
```cpp
EndKey::Engine::TypingEngine typingEngine;
EndKey::Engine::TypingEngine::InputConfig config = {
    .inputType = 0,      // Telex
    .language = 1,       // Vietnamese
    .codeTable = 0,      // Unicode
    .freeMark = true,
    .checkSpelling = true,
    .useModernOrthography = true
};
typingEngine.setInputConfig(config);

// Process 'a' key
vKeyEvent event = {KEY_DOWN, 0};
vKeyEventState state = {0, 0, 0, 0};
typingEngine.processKey(event, state, 'a');

// Process tone mark 's' (for â)
typingEngine.processKey(event, state, 's');
```

```cpp
void startNewSession()
```
**Description**: Start a new typing session, clearing all buffers.
**Parameters**: None
**Returns**: void

```cpp
void resetTypingState()
```
**Description**: Reset the typing state to initial values.
**Parameters**: None
**Returns**: void

##### Configuration

```cpp
void setInputConfig(const InputConfig& config)
```
**Description**: Set the typing engine configuration.
**Parameters**:
- `config`: Input configuration structure
**Returns**: void

```cpp
const InputConfig& getInputConfig() const
```
**Description**: Get current typing engine configuration.
**Parameters**: None
**Returns**: const reference to current configuration

##### State Management

```cpp
const TypingState& getCurrentState() const
```
**Description**: Get current typing state information.
**Parameters**: None
**Returns**: const reference to TypingState structure

```cpp
bool isTyping() const
```
**Description**: Check if currently typing (has characters in buffer).
**Parameters**: None
**Returns**: `true` if typing, `false` otherwise

##### Character Processing

```cpp
Uint32 getCharacterCode(const Uint32& data) const
```
**Description**: Convert engine character code to actual character.
**Parameters**:
- `data`: Engine character code
**Returns**: Actual Unicode character code

```cpp
bool shouldBreakWord(const Uint16& keyCode) const
```
**Description**: Check if the key code should trigger a word break.
**Parameters**:
- `keyCode`: Key code to check
**Returns**: `true` if word should break, `false` otherwise

## SpellingEngine API

### Class Definition

```cpp
namespace EndKey {
    namespace Engine {
        class SpellingEngine {
            // ... implementation
        };
    }
}
```

#### Configuration Structures

```cpp
struct SpellingConfig {
    bool checkSpelling = true;
    bool useModernOrthography = true;
    bool allowConsonantZFWJ = false;
    bool quickStartConsonant = false;
    bool quickEndConsonant = false;
    bool restoreIfWrongSpelling = true;
    bool tempOffSpelling = false;
};
```

#### Data Structures

```cpp
struct ToneMarkInfo {
    Uint16 baseChar;     // Base character without tone
    Uint16 toneChar;     // Character with tone applied
    Uint8 toneType;      // 0: none, 1: acute, 2: grave, 3: hook, 4: tilde, 5: dot
    bool isValid;        // Whether the tone application is valid
};
```

#### Public Methods

##### Configuration

```cpp
void setSpellingConfig(const SpellingConfig& config)
```
**Description**: Set spelling engine configuration.
**Parameters**:
- `config`: Spelling configuration structure
**Returns**: void

```cpp
const SpellingConfig& getSpellingConfig() const
```
**Description**: Get current spelling engine configuration.
**Parameters**: None
**Returns**: const reference to current configuration

##### Core Operations

```cpp
bool isValidVietnameseWord(const std::vector<Uint16>& word) const
```
**Description**: Validate if a word follows Vietnamese spelling rules.
**Parameters**:
- `word`: Vector of Unicode characters representing the word
**Returns**: `true` if valid Vietnamese word, `false` otherwise

**Example**:
```cpp
EndKey::Engine::SpellingEngine spellingEngine;
std::vector<Uint16> word = {'v', 'i', 'ệ', 't'}; // "việt"

if (spellingEngine.isValidVietnameseWord(word)) {
    // Word is valid Vietnamese
} else {
    // Word has spelling errors
}
```

```cpp
ToneMarkInfo applyToneMark(Uint16 character, Uint8 toneType) const
```
**Description**: Apply a tone mark to a character.
**Parameters**:
- `character`: Base character to apply tone to
- `toneType`: Type of tone (1: acute, 2: grave, 3: hook, 4: tilde, 5: dot)
**Returns**: ToneMarkInfo structure with result

**Example**:
```cpp
// Apply acute accent to 'a'
ToneMarkInfo result = spellingEngine.applyToneMark('a', 1);
if (result.isValid) {
    Uint16 accentedChar = result.toneChar; // Should be 'á'
}
```

```cpp
Uint16 findVowelForMark(const std::vector<Uint16>& word, Uint8 markType) const
```
**Description**: Find the appropriate vowel in a word to apply a tone mark.
**Parameters**:
- `word`: Vector of characters representing the word
- `markType`: Type of tone mark to apply
**Returns**: Character code of vowel to apply tone to, or 0 if none found

##### Character Classification

```cpp
bool isVowel(Uint16 character) const
```
**Description**: Check if a character is a Vietnamese vowel.
**Parameters**:
- `character`: Unicode character to check
**Returns**: `true` if vowel, `false` otherwise

```cpp
bool isConsonant(Uint16 character) const
```
**Description**: Check if a character is a Vietnamese consonant.
**Parameters**:
- `character`: Unicode character to check
**Returns**: `true` if consonant, `false` otherwise

```cpp
bool isValidConsonantPair(Uint16 first, Uint16 second) const
```
**Description**: Check if two consonants form a valid Vietnamese consonant pair.
**Parameters**:
- `first`: First consonant character
- `second`: Second consonant character
**Returns**: `true` if valid pair, `false` otherwise

##### Performance Optimization

```cpp
void preloadCommonWords()
```
**Description**: Preload frequently used words into cache for faster validation.
**Parameters**: None
**Returns**: void

```cpp
void clearCache()
```
**Description**: Clear all cached spelling data.
**Parameters**: None
**Returns**: void

```cpp
size_t getCacheSize() const
```
**Description**: Get current cache size in bytes.
**Parameters**: None
**Returns**: Cache size in bytes

##### Statistics

```cpp
const SpellingStats& getStatistics() const
```
**Description**: Get spelling engine performance statistics.
**Parameters**: None
**Returns**: const reference to SpellingStats structure

```cpp
void resetStatistics()
```
**Description**: Reset all performance statistics.
**Parameters**: None
**Returns**: void

## MacroEngine API

### Class Definition

```cpp
namespace EndKey {
    namespace Engine {
        class MacroEngine {
            // ... implementation
        };
    }
}
```

#### Configuration Structures

```cpp
struct MacroConfig {
    bool useMacro = true;
    bool useMacroInEnglishMode = false;
    bool autoCapsMacro = true;
    bool skipMacroNextBreak = false;
    bool forceSkipMacroNextBreak = false;
};
```

#### Data Structures

```cpp
struct MacroEntry {
    std::string keyword;          // Macro trigger keyword
    std::string expansion;        // Text to expand to
    bool isActive = true;         // Whether macro is active
    uint32_t usageCount = 0;      // Number of times used
    double averageCaseRatio = 0.0; // Track capitalization patterns
};

struct MacroMatch {
    std::string keyword;          // Matched keyword
    std::string expansion;        // Expanded text
    bool shouldCapitalize = false; // Should capitalize first letter
    bool shouldAllCaps = false;    // Should use all caps
    bool found = false;           // Whether a match was found
};
```

#### Public Methods

##### Configuration

```cpp
void setMacroConfig(const MacroConfig& config)
```
**Description**: Set macro engine configuration.
**Parameters**:
- `config`: Macro configuration structure
**Returns**: void

```cpp
const MacroConfig& getMacroConfig() const
```
**Description**: Get current macro engine configuration.
**Parameters**: None
**Returns**: const reference to current configuration

##### Core Operations

```cpp
MacroMatch findMacro(const std::string& input) const
```
**Description**: Find a macro that matches the input string.
**Parameters**:
- `input`: Input string to match against macros
**Returns**: MacroMatch structure with match information

**Example**:
```cpp
EndKey::Engine::MacroEngine macroEngine;
macroEngine.addMacro("btw", "by the way");

MacroMatch match = macroEngine.findMacro("btw");
if (match.found) {
    std::string expanded = match.expansion; // "by the way"
}
```

```cpp
bool shouldExpandMacro(const std::string& word) const
```
**Description**: Check if a word should trigger macro expansion.
**Parameters**:
- `word`: Word to check for macro expansion
**Returns**: `true` if should expand, `false` otherwise

```cpp
std::string applyAutoCapitalization(const std::string& text,
                                   const std::string& originalCase) const
```
**Description**: Apply automatic capitalization based on original input case.
**Parameters**:
- `text`: Text to capitalize
- `originalCase`: Original input for case pattern analysis
**Returns**: Capitalized text

##### Macro Management

```cpp
void addMacro(const std::string& keyword, const std::string& expansion)
```
**Description**: Add a new macro to the system.
**Parameters**:
- `keyword`: Trigger keyword
- `expansion`: Text to expand to
**Returns**: void

```cpp
void removeMacro(const std::string& keyword)
```
**Description**: Remove a macro from the system.
**Parameters**:
- `keyword`: Keyword of macro to remove
**Returns**: void

```cpp
bool hasMacro(const std::string& keyword) const
```
**Description**: Check if a macro with the given keyword exists.
**Parameters**:
- `keyword`: Keyword to check
**Returns**: `true` if macro exists, `false` otherwise

##### Bulk Operations

```cpp
void loadMacros(const std::unordered_map<std::string, std::string>& macros)
```
**Description**: Load multiple macros at once.
**Parameters**:
- `macros`: Map of keyword to expansion pairs
**Returns**: void

```cpp
std::unordered_map<std::string, std::string> getAllMacros() const
```
**Description**: Get all currently defined macros.
**Parameters**: None
**Returns**: Map of all keyword to expansion pairs

##### Performance Optimization

```cpp
void preloadFrequentMacros()
```
**Description**: Preload frequently used macros into cache.
**Parameters**: None
**Returns**: void

```cpp
void optimizeMacroOrder()
```
**Description**: Reorder macros based on usage frequency for faster lookup.
**Parameters**: None
**Returns**: void

##### Statistics

```cpp
const MacroStats& getStatistics() const
```
**Description**: Get macro engine performance statistics.
**Parameters**: None
**Returns**: const reference to MacroStats structure

```cpp
void resetStatistics()
```
**Description**: Reset all macro statistics.
**Parameters**: None
**Returns**: void

## VietnameseProcessor API

### Class Definition

```cpp
namespace EndKey {
    namespace Engine {
        class VietnameseProcessor {
            // ... implementation
        };
    }
}
```

#### Public Methods

##### Character Conversion

```cpp
Uint16 convertCharacter(Uint16 input, Uint8 toneType) const
```
**Description**: Convert a character by applying tone marks and diacritics.
**Parameters**:
- `input`: Base character to convert
- `toneType`: Type of tone to apply
**Returns**: Converted character code

```cpp
std::vector<Uint16> convertWord(const std::vector<Uint16>& word) const
```
**Description**: Convert an entire word applying Vietnamese processing rules.
**Parameters**:
- `word`: Vector of characters to convert
**Returns**: Vector of converted characters

##### Code Table Conversion

```cpp
std::wstring convertEncoding(const std::wstring& input,
                           int fromCodeTable,
                           int toCodeTable) const
```
**Description**: Convert text between different Vietnamese code tables.
**Parameters**:
- `input`: Input text string
- `fromCodeTable`: Source code table (0: Unicode, 1: TCVN3, 2: VNI-Windows)
- `toCodeTable`: Target code table
**Returns**: Converted text string

##### Tone Mark Processing

```cpp
Uint8 detectToneType(Uint16 character) const
```
**Description**: Detect the tone type of a Vietnamese character.
**Parameters**:
- `character`: Character to analyze
**Returns**: Tone type (0: none, 1: acute, 2: grave, 3: hook, 4: tilde, 5: dot)

## Utility APIs

### ConfigurationManager

```cpp
class ConfigurationManager {
public:
    bool loadConfiguration(const std::string& configFile);
    bool saveConfiguration(const std::string& configFile);
    void updateSetting(const std::string& key, const std::string& value);
    std::string getSetting(const std::string& key) const;
    EngineConfig getEngineConfig() const;
    void setEngineConfig(const EngineConfig& config);
};
```

### MemoryManager

```cpp
class MemoryManager {
public:
    void* allocate(size_t size);
    void deallocate(void* ptr);
    size_t getTotalUsage() const;
    size_t getPeakUsage() const;
    void optimizeMemory();
    MemoryStats getStatistics() const;
};
```

### ConvertTool

```cpp
class ConvertTool {
public:
    std::wstring utf8ToWide(const std::string& utf8Str);
    std::string wideToUtf8(const std::wstring& wideStr);
    Uint16 convertCodeTable(Uint16 charCode, int fromTable, int toTable);
    std::wstring convertString(const std::wstring& input, int fromTable, int toTable);
};
```

## Examples and Usage

### Complete Engine Usage Example

```cpp
#include "EngineCore.h"
#include "TypingEngine.h"
#include "SpellingEngine.h"
#include "MacroEngine.h"

int main() {
    // Initialize engine
    EndKey::Engine::EngineCore engine;

    // Configure engine
    EngineConfig config = {
        .language = 1,                    // Vietnamese
        .inputType = 0,                   // Telex
        .codeTable = 0,                   // Unicode
        .checkSpelling = 1,               // Enabled
        .useModernOrthography = 1,        // Enabled
        .useMacro = 1,                    // Enabled
        .autoCapsMacro = 1,               // Enabled
        .useSmartSwitchKey = 1            // Enabled
    };

    if (!engine.initialize(config)) {
        std::cerr << "Failed to initialize EndKey engine" << std::endl;
        return -1;
    }

    // Add some macros
    auto& macroManager = engine.getMacroManager();
    macroManager.addMacro("btw", "by the way");
    macroManager.addMacro("omg", "oh my god");

    // Process key events
    vKeyEvent event = {KEY_DOWN, 0};
    vKeyEventState state = {0, 0, 0, 0}; // No modifiers

    // Type "việt"
    std::vector<Uint16> testKeys = {'v', 'i', 'e', 'e', 't', 's'};

    for (Uint16 key : testKeys) {
        EngineHookState result = engine.processKeyEvent(event, state, key);

        if (result.newCharCount > 0) {
            std::cout << "Output: ";
            for (int i = 0; i < result.newCharCount; i++) {
                std::cout << static_cast<char>(result.charData[i]);
            }
            std::cout << std::endl;
        }
    }

    // Get performance statistics
    auto& vietnameseProcessor = engine.getVietnameseProcessor();
    auto stats = vietnameseProcessor.getStatistics();
    std::cout << "Characters processed: " << stats.totalProcessed << std::endl;

    // Cleanup
    engine.shutdown();
    return 0;
}
```

### Custom Typing Engine Example

```cpp
#include "TypingEngine.h"

class CustomTypingHandler {
private:
    EndKey::Engine::TypingEngine typingEngine_;

public:
    CustomTypingHandler() {
        // Configure for Vietnamese Telex input
        EndKey::Engine::TypingEngine::InputConfig config = {
            .inputType = 0,      // Telex
            .language = 1,       // Vietnamese
            .codeTable = 0,      // Unicode
            .freeMark = true,
            .checkSpelling = true,
            .useModernOrthography = true
        };

        typingEngine_.setInputConfig(config);
    }

    std::string processTyping(const std::string& input) {
        std::string result;
        typingEngine_.startNewSession();

        vKeyEvent event = {KEY_DOWN, 0};
        vKeyEventState state = {0, 0, 0, 0};

        for (char c : input) {
            typingEngine_.processKey(event, state, static_cast<Uint16>(c));

            // Get current typing state
            const auto& typingState = typingEngine_.getCurrentState();

            // Convert typing buffer to string
            for (size_t i = 0; i < typingState.index; i++) {
                Uint32 charCode = typingEngine_.getCharacterCode(typingState.typingWord[i]);
                result += static_cast<char>(charCode);
            }
        }

        return result;
    }
};
```

### Macro System Integration Example

```cpp
#include "MacroEngine.h"

class MacroSystem {
private:
    EndKey::Engine::MacroEngine macroEngine_;

public:
    MacroSystem() {
        // Configure macro engine
        EndKey::Engine::MacroEngine::MacroConfig config = {
            .useMacro = true,
            .autoCapsMacro = true,
            .useMacroInEnglishMode = false
        };

        macroEngine_.setMacroConfig(config);

        // Load common macros
        loadCommonMacros();
    }

    void loadCommonMacros() {
        macroEngine_.addMacro("btw", "by the way");
        macroEngine_.addMacro("omg", "oh my god");
        macroEngine_.addMacro("lol", "laughing out loud");
        macroEngine_.addMacro("asap", "as soon as possible");
        macroEngine_.addMacro("fyi", "for your information");

        // Preload for performance
        macroEngine_.preloadFrequentMacros();
    }

    std::string expandMacro(const std::string& word, const std::string& originalInput) {
        auto match = macroEngine_.findMacro(word);

        if (match.found) {
            return macroEngine_.applyAutoCapitalization(match.expansion, originalInput);
        }

        return word; // No macro found, return original
    }

    void printMacroStatistics() {
        auto stats = macroEngine_.getStatistics();
        std::cout << "Total macros: " << stats.totalMacros << std::endl;
        std::cout << "Expansions performed: " << stats.expansionsPerformed << std::endl;
        std::cout << "Cache hit ratio: " << (stats.cacheHits * 100.0 / (stats.cacheHits + stats.cacheMisses)) << "%" << std::endl;
    }
};
```

This comprehensive API documentation provides detailed reference material for all major components of the refactored EndKey engine, including practical examples and usage patterns.