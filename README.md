# EndKey

Lightweight Vietnamese input method for macOS. Built with EventTap architecture for direct distribution without IME registration.

## Features

- **Input Methods**: Telex and VNI Vietnamese typing
- **System Integration**: Menu bar toggle (E/V) with global hotkey (Cmd+Shift)
- **Background Operation**: Runs as menu bar app (no dock icon)
- **Settings**: Launch at login, input method selection
- **Direct Distribution**: No App Store or notarization required

## Quick Start

1. Download `EndKey.dmg` from releases
2. Mount DMG and drag EndKey to Applications
3. Launch EndKey (right-click → Open to bypass Gatekeeper)
4. Grant Accessibility permission when prompted
5. Start typing Vietnamese immediately

## Installation

### Standard Installation
1. Download `EndKey.dmg`
2. Open DMG and drag EndKey to Applications
3. First launch: Right-click → Open (to bypass Gatekeeper)
4. Grant Accessibility permission when prompted

### If blocked by Gatekeeper
Run in Terminal:
```bash
xattr -cr /Applications/EndKey.app
```

## Usage

### Mode Switching
- **E** = English mode (passthrough)
- **V** = Vietnamese mode (Telex/VNI active)
- **Cmd+Shift** = Toggle between modes
- **Menu bar** = Click E/V indicator to switch

### Telex Input
| Key | Result | Key | Result |
|-----|--------|-----|--------|
| `aa` | â | `s` | sắc (á) |
| `ee` | ê | `f` | huyền (à) |
| `oo` | ô | `r` | hỏi (ả) |
| `aw` | ă | `x` | ngã (ã) |
| `ow` | ơ | `j` | nặng (ạ) |
| `uw` | ư | `dd` | đ |

### VNI Input
| Key | Result | Key | Result |
|-----|--------|-----|--------|
| `a1-5` | tones (áàảãạ) | `a6` | â |
| `d9` | đ | `a7` | ă |
| `o6` | ô | `o7` | ơ |
| `e6` | ê | `u7` | ư |

### Examples
- Type `xin chao` → `xin chào` (Telex)
- Type `viet65 nam` → `việt nam` (VNI)
- Press Backspace to undo transformations

## System Requirements

- macOS 11.0 (Big Sur) or later
- Accessibility permission (required for keyboard capture)
- 5MB disk space, <20MB memory usage

## Technical Details

### Architecture
- **EventTap-based**: Direct keyboard event interception
- **Background agent**: No dock icon (LSUIElement)
- **Swift/SwiftUI**: Native macOS performance
- **Zero dependencies**: Pure system frameworks

### Security
- Local-only processing (no network)
- Minimal permissions (Accessibility only)
- No data collection or analytics
- Open source for transparency

## Development

### Building from Source
```bash
# Clone and build
git clone https://github.com/your-repo/EndKey.git
cd EndKey

# Build release version
./scripts/build-release.sh

# Create DMG installer
./scripts/create-dmg.sh
```

### Project Structure
```
EndKey/
├── EndKey/Core/          # Vietnamese input engines
├── EndKey/UI/            # Menu bar and settings
├── EndKey/Models/        # Application state
├── scripts/              # Build automation
└── docs/                 # Documentation
```

## Documentation

- [Project Overview](docs/project-overview-pdr.md) - Product requirements and roadmap
- [System Architecture](docs/system-architecture.md) - Technical design and event flow
- [Code Standards](docs/code-standards.md) - Development guidelines and patterns
- [Codebase Summary](docs/codebase-summary.md) - Component documentation

## Support

### Common Issues
1. **Permission denied**: Grant Accessibility in System Preferences → Security & Privacy
2. **Not working**: Check menu bar icon (E/V), ensure Vietnamese mode is active
3. **Wrong characters**: Verify input method (Telex vs VNI) in preferences

### Getting Help
- Check documentation in `/docs` folder
- Review installation requirements
- Verify macOS version compatibility
- Test with different applications

## Contributing

1. Fork the repository
2. Create feature branch (`git checkout -b feature/amazing-feature`)
3. Commit changes (`git commit -m 'Add amazing feature'`)
4. Push to branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

See [Code Standards](docs/code-standards.md) for development guidelines.

## License

MIT License - see LICENSE file for details.

## Acknowledgments

- Vietnamese input method research community
- Apple EventTap documentation and examples
- Open source Vietnamese typing projects for reference
