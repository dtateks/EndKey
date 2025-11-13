# EndKey - Vietnamese Input Method for macOS

[![macOS](https://img.shields.io/badge/macOS-10.14%2B-blue)](https://www.apple.com/macos/)
[![Language](https://img.shields.io/badge/language-C%2B%2B%20%7C%20Objective--C-lightgrey)](https://en.wikipedia.org/wiki/List_of_programming_languages)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)

EndKey is a professional Vietnamese input method for macOS that provides intelligent, real-time Vietnamese text conversion with advanced features. It supports multiple input methods (Telex, VNI, Simple Telex), smart application-aware language switching, text expansion macros, and comprehensive Vietnamese orthography support.

## Features

### Core Input Methods
- **Telex**: Traditional Vietnamese input with diacritic markers (w, f, r, s, x, j)
- **VNI**: Numeric-based input using tone numbers (1-6)
- **Simple Telex**: Streamlined variants for faster typing

### Smart Capabilities
- **Real-time conversion**: Instant Vietnamese character rendering
- **Smart application switching**: Remembers language preferences per app
- **Text expansion macros**: Define shortcuts that expand to full phrases
- **Auto-capitalization**: Automatic sentence start capitalization
- **Quick Telex**: Fast consonant combinations (ch, ph, th, etc.)
- **Double-space period**: Automatic period insertion

### Vietnamese Language Support
- **Unicode-only**: Full UTF-8 compliance
- **Modern orthography**: Contemporary Vietnamese spelling rules
- **Spell checking**: Optional validation with restoration
- **Tone mark processing**: Complete set of Vietnamese diacritics
- **Diphthong handling**: Complex vowel combinations (òa vs oà)

### Additional Features
- **Convert Tool**: Batch text encoding conversion
- **Status menu**: Quick access to settings and controls
- **Auto-startup**: Launch with system
- **Temporary disable**: Hotkey to pause EndKey when needed
- **Case-preserving macros**: Expansions respect original casing

## Quick Start

### Requirements
- macOS 10.14 (Mojave) or later
- Administrator privileges for initial setup

### Installation

1. **Download** the latest EndKey app from [Releases](https://github.com/tuyenvm/EndKey/releases)

2. **Install** by moving EndKey.app to Applications folder

3. **First Launch**:
   ```
   Open EndKey from Applications
   Click "Open Security & Privacy" when prompted
   Grant Accessibility permission in System Preferences
   Return to EndKey and click "Restart" if needed
   ```

4. **Enable** EndKey:
   - Check "Enable EndKey" in the preferences
   - Or use Cmd+Shift hotkey to toggle
   - Status menu icon shows active/inactive state

### Basic Usage

**Type Vietnamese** (Telex example):
```
Type: w a s     → Output: wás
Type: q u o a n → Output: qu oan
Type: g i u w s → Output: gi uws
```

**Switch Languages**:
- Press Cmd+Shift to toggle English/Vietnamese
- Or check "Use Smart Switch" to auto-switch per application

**Add Macros**:
1. Open Preferences (Cmd+,)
2. Go to "Macro" tab
3. Click "+" to add new macro
4. Enter shortcut (e.g., "btw") and expansion (e.g., "by the way")
5. Close window - macros are auto-saved

**Quick Convert Text**:
1. Copy Vietnamese text to clipboard
2. Press Ctrl+Option+V (or use Status Menu → Convert)
3. Text is converted and saved back to clipboard

## Configuration

Access preferences via:
- **Menu Bar**: Click EndKey icon → Preferences
- **Keyboard**: Cmd+,
- **Status Menu**: Quick settings and toggles

### Preferences Tabs

**Primary**:
- Input Method selection (Telex/VNI/Simple Telex)
- Feature toggles (Free Mark, Quick Telex, etc.)
- Spell checking options

**Macro**:
- View all macros
- Add/edit/delete entries
- Import/export functionality

**System**:
- Smart application switching
- Advanced features
- Performance settings

## Documentation

Comprehensive documentation is available in the `docs/` directory:

- **[Project Overview & PDR](docs/project-overview-pdr.md)**: Detailed product requirements, user stories, and feature specifications
- **[Codebase Summary](docs/codebase-summary.md)**: High-level architecture, components, and technology stack
- **[Code Standards](docs/code-standards.md)**: Development practices, coding conventions, and style guides
- **[System Architecture](docs/system-architecture.md)**: Deep dive into system design, data flow, and implementation details

## Technology Stack

- **Engine**: Pure C++11 for Vietnamese text processing
- **UI Layer**: Objective-C 2.0 with ARC (Cocoa/AppKit)
- **Event System**: Core Graphics (CGEventTap)
- **Build System**: Xcode project with C++/Objective-C compilation
- **No External Dependencies**: Self-contained with only system frameworks

## Architecture Highlights

```
┌─────────────────────┐
│   macOS App Layer   │  (Objective-C UI)
├─────────────────────┤
│  Event Tap Layer    │  (CGEventTap)
├─────────────────────┤
│   Engine Bridge     │  (C++ interface)
├─────────────────────┤
│  Core Processing    │  (Vietnamese engine)
│  - Vietnamese       │
│  - Macro System     │
│  - Smart Switch     │
└─────────────────────┘
```

## Build from Source

See [BUILD.md](BUILD.md) for detailed build instructions.

### Quick Build

```bash
# Clean and build
xcodebuild -project macOS/EndKey.xcodeproj \
           -scheme ModernKey \
           -configuration Release \
           clean build \
           CODE_SIGNING_ALLOWED=NO

# App will be in DerivedData - copy to root
find ~/Library/Developer/XCode/DerivedData -name "EndKey.app" -type d
```

## Project Structure

```
EndKey/
├── engine/                    # C++ core engine
│   ├── Engine.cpp/h          # Main processing logic
│   ├── Vietnamese.cpp/h      # Vietnamese language processing
│   ├── Macro.cpp/h           # Text expansion system
│   └── *.cpp/h               # Other engine modules
├── macOS/ModernKey/           # macOS application
│   ├── *.m, *.mm             # Objective-C implementation
│   ├── Base.lproj/           # UI resources
│   └── Resources/            # Assets and localizations
├── docs/                     # Documentation (this directory)
├── BUILD.md                  # Build instructions
└── README.md                 # This file
```

## Performance

- **Latency**: <10ms from key press to character display
- **Memory**: ~50MB total footprint
- **CPU**: <5% during active typing
- **Startup**: <3 seconds to full functionality

## Keyboard Shortcuts

| Shortcut | Action |
|----------|--------|
| Cmd+Shift | Toggle EndKey on/off |
| Cmd+, | Open Preferences |
| Ctrl+Option+V | Quick Convert |
| Cmd+Space | System language switch (if enabled) |

## Troubleshooting

### Common Issues

**Permission Not Granted**:
```
EndKey requires Accessibility permission
→ System Preferences → Security & Privacy → Accessibility
→ Grant permission to EndKey
→ Restart EndKey
```

**Events Not Processing**:
```
Check status menu icon (grayed out = inactive)
→ Click icon → Enable EndKey
→ Or verify hotkey isn't conflicting
```

**Vietnamese Not Converting**:
```
Verify input method in Preferences
→ Primary tab → Input Type
→ Choose correct method (Telex/VNI)
→ Check "Vietnamese" language mode active
```

**Macros Not Expanding**:
```
Enable macros in Preferences → Primary
→ Check "Use Macro" and "Use Macro in English Mode"
→ Verify macro exists in Macro tab
→ Ensure not in middle of another word
```

### Log Location
```
~/Library/Logs/EndKey/
```

## Contributing

We welcome contributions! Please:
1. Fork the repository
2. Create a feature branch
3. Follow code standards (see docs/code-standards.md)
4. Test thoroughly
5. Submit a pull request

## Support

- **Issues**: Report bugs via [GitHub Issues](https://github.com/tuyenvm/EndKey/issues)
- **Discussions**: Join [GitHub Discussions](https://github.com/tuyenvm/EndKey/discussions)
- **Email**: Contact information in About dialog

## License

MIT License - see LICENSE file for details.

## Acknowledgments

- Vietnamese language processing engine
- macOS Accessibility API integration
- Cocoa framework for native UI
- Contributors and testers

---

**Developed with ❤️ for the Vietnamese-speaking macOS community**
