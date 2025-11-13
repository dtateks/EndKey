# EndKey Project Overview & Product Development Requirements (PDR)

## Project Overview

EndKey is a professional Vietnamese input method for macOS that provides an advanced typing experience with intelligent features for Vietnamese language processing. The application intercepts keyboard and mouse events to provide real-time Vietnamese text conversion with multiple input methods, smart features, and application-aware language switching.

### What is EndKey?

EndKey is a desktop input method editor (IME) designed specifically for macOS users who need efficient Vietnamese typing capabilities. Unlike traditional input methods, EndKey offers:

- **Real-time Vietnamese text conversion** from Romanized input to properly accented Vietnamese characters
- **Multiple input method support** including Telex, VNI, and Simple Telex
- **Smart application-switching** that remembers language preferences per application
- **Text expansion macros** for productivity enhancement
- **Comprehensive Vietnamese orthography support** including modern and traditional spelling rules

### Key Features

#### Core Input Methods
- **Telex**: Traditional Vietnamese input using diacritic markers (w, f, r, s, x, j, a, e, o)
- **VNI**: Numeric-based input system using numbers 1-6 for tone marks
- **Simple Telex 1 & 2**: Streamlined versions of Telex for faster typing

#### Vietnamese Language Processing
- **Unicode-only support**: Full Unicode compliance with proper UTF-8 handling
- **Modern orthography**: Support for contemporary Vietnamese spelling conventions
- **Spell checking**: Optional spelling validation with wrong spelling restoration
- **Tone mark handling**: Complete set of Vietnamese diacritical marks (sắc, huyền, hỏi, ngã, nặng)
- **Diphthong processing**: Support for complex Vietnamese vowel combinations (òa, úy vs oà, uý)

#### Smart Features
- **Smart application switching**: Automatically switches between English/Vietnamese based on application preferences
- **Quick Telex**: Fast consonant combination shortcuts (ch, ph, th, etc.)
- **Quick start/end consonants**: Intelligent consonant recognition (f→ph, j→gi, w→qu)
- **Temporary disabling**: Hotkey to temporarily disable EndKey when needed
- **Auto capitalize**: Automatic capitalization of first letter in sentences
- **Double space period**: Convert double space to period and space automatically
- **Uppercase macro**: Case-preserving macro expansion (e.g., "btw" → "By The Way")

#### Macro System
- **Text expansion**: Define abbreviations that expand to full text
- **Case preservation**: Automatic case adjustment in macro expansion
- **Application-wide**: Macros work across all applications
- **Persistent storage**: Macro definitions saved and loaded between sessions

#### Additional Features
- **Convert tool**: Batch convert Vietnamese text between different encodings
- **Accessibility permissions**: Built-in permission handling for macOS accessibility features
- **Performance optimization**: Efficient event processing for minimal system impact
- **Status menu**: Quick access to settings and status information

## Product Development Requirements (PDR)

### User Stories

#### Primary Users
- **Vietnamese professionals** who need efficient Vietnamese typing on macOS
- **Content creators** writing in Vietnamese who require accurate diacritics
- **Multilingual users** working across English and Vietnamese
- **Students** learning Vietnamese who need proper orthography
- **Businesses** requiring Vietnamese localization

#### Core User Stories

**As a Vietnamese professional**, I want to type Vietnamese text quickly and accurately, so that I can be productive when writing documents, emails, and reports.

**Acceptance Criteria:**
- Can type Vietnamese using Telex method with real-time conversion
- All Vietnamese characters render correctly with proper diacritics
- Conversion happens automatically without manual intervention
- Works across all macOS applications

**As a content creator**, I want to expand text shortcuts to full phrases, so that I can write faster and more consistently.

**Acceptance Criteria:**
- Can define custom macro shortcuts (e.g., "btw" → "by the way")
- Macros expand automatically when space or punctuation is typed
- Case is preserved appropriately (btw → By the way, BTW → BY THE WAY)
- Macro list can be viewed and edited through the UI

**As a multilingual user**, I want the system to remember my language preferences per application, so that I don't need to manually switch input methods.

**Acceptance Criteria:**
- Can set default input method for each application (English or Vietnamese)
- System automatically applies the correct input method when switching apps
- Preferences persist across application restarts
- Can override preferences temporarily when needed

**As a user**, I want to temporarily disable the input method when typing passwords or codes, so that my input isn't accidentally converted.

**Acceptance Criteria:**
- Hotkey (Cmd+Shift) temporarily disables EndKey
- Visual indicator shows when EndKey is disabled
- Re-enables automatically after next word break
- Can manually re-enable through status menu

**As a Vietnamese learner**, I want spell checking to help me learn proper Vietnamese, so that I can improve my orthography.

**Acceptance Criteria:**
- System highlights potentially misspelled Vietnamese words
- User can choose to restore incorrect words to original form
- Toggle available to enable/disable spell checking
- Works with modern Vietnamese orthography

### Functional Requirements

1. **Event Interception**
   - Must intercept keyboard events at system level
   - Must handle mouse events for cursor position tracking
   - Must capture modifier key states (Ctrl, Option, Command, Shift)
   - Must capture Caps Lock state
   - Performance impact < 5% CPU usage

2. **Text Processing**
   - Must convert Romanized input to Vietnamese characters in real-time
   - Must support Telex, VNI, and Simple Telex input methods
   - Must handle multi-character combinations (diphthongs, triphthongs)
   - Must validate input sequences for correctness
   - Must restore original text for invalid sequences

3. **Configuration Management**
   - Must persist all settings to disk
   - Must load settings on application startup
   - Must provide UI for all configuration options
   - Must validate configuration changes

4. **Application Integration**
   - Must work across all macOS applications
   - Must not interfere with application shortcuts
   - Must respect application-specific input methods
   - Must handle permission requests properly

5. **Data Persistence**
   - Must save macro definitions to disk
   - Must save smart switch preferences per application
   - Must maintain backward compatibility with older versions
   - Must handle corrupted data gracefully

### Non-Functional Requirements

#### Performance
- **Latency**: Keyboard input to display < 10ms
- **Memory usage**: < 50MB RAM footprint
- **CPU usage**: < 5% during active typing
- **Startup time**: < 3 seconds to full functionality

#### Reliability
- **Uptime**: Must run continuously without crashes
- **Error handling**: Graceful degradation for invalid input
- **Data integrity**: No data loss for user settings and macros
- **Recovery**: Automatic recovery from transient errors

#### Security
- **Privacy**: No data sent to external servers (except version check)
- **Permissions**: Minimal required permissions (accessibility only)
- **Sandboxing**: Compatible with macOS sandbox requirements

#### Compatibility
- **macOS versions**: Compatible with macOS 10.14+ (Mojave and later)
- **Architectures**: Support for both Intel and Apple Silicon (universal binary)
- **Applications**: Works with all native and web-based applications

#### Usability
- **Learning curve**: Telex method discoverable by Vietnamese speakers
- **Visual feedback**: Clear indicators for enabled/disabled states
- **Accessibility**: Support for VoiceOver and other accessibility features
- **Localization**: Vietnamese and English UI languages

### Technical Constraints

1. **Platform limitation**: Must use macOS accessibility APIs (CGEventTap)
2. **Permission requirement**: Requires Accessibility permission from user
3. **System integration**: Must use macOS status menu for quick access
4. **Code signing**: Must be signed for distribution and permissions
5. **Sandboxing**: Must work within macOS App Store requirements

### Dependencies

#### External
- **macOS SDK**: Core Graphics framework for event tapping
- **Foundation Framework**: For Cocoa UI components
- **Application Services**: For clipboard and pasteboard operations

#### Internal
- **Engine core**: C++ engine for Vietnamese text processing
- **Vietnamese module**: Character mapping and combination logic
- **Macro module**: Text expansion functionality
- **SmartSwitch module**: Application-aware language switching

### Current Project Status

#### Completed Features
- Core Vietnamese input method (Telex, VNI, Simple Telex)
- Real-time text conversion with Unicode support
- Macro system with text expansion
- Smart application switching
- Basic UI with preferences panel
- Accessibility integration
- Convert tool for text encoding
- Version update checking

#### In Progress / Needs Enhancement
- Performance optimization for event processing
- Enhanced spell checking with more comprehensive dictionary
- Additional input method variants
- Improved accessibility features
- Better error reporting and diagnostics

#### Known Limitations
- Requires manual permission granting (one-time setup)
- May not work with some full-screen games or special input contexts
- Complex web applications may have compatibility issues
- Performance impact on very old macOS hardware

### Success Metrics

1. **User Adoption**: Number of active users monthly
2. **Performance**: Average input latency and CPU usage
3. **Reliability**: Crash-free session duration
4. **User Satisfaction**: Rating and review scores
5. **Feature Usage**: Percentage of users enabling each feature
6. **Support Requests**: Reduction in common support issues

### Roadmap Considerations

#### Short Term (Next 3-6 months)
- Performance improvements
- Bug fixes and stability enhancements
- Enhanced UI/UX
- Better documentation

#### Medium Term (6-12 months)
- Additional input methods
- Advanced macro features (variables, date/time insertion)
- Sync capabilities across devices
- Mobile version consideration

#### Long Term (12+ months)
- AI-powered suggestions
- Cloud-based configuration sync
- Collaborative macro sharing
- Integration with productivity tools

## Conclusion

EndKey represents a mature, feature-rich Vietnamese input method for macOS with a solid foundation in Vietnamese language processing. The project combines a performant C++ engine with a native macOS interface to provide users with an efficient, reliable Vietnamese typing experience. With ongoing development focused on performance, usability, and expanded features, EndKey continues to serve the Vietnamese-speaking macOS community effectively.
