# EndKey

Lightweight Vietnamese input method for macOS.

## Features

- Simple Telex and VNI input methods
- Menubar toggle (E/V)
- Cmd+Shift hotkey to switch modes
- Launch at login option

## Installation

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

- **E** = English mode (passthrough)
- **V** = Vietnamese mode (Telex/VNI active)
- **Cmd+Shift** = Toggle between modes

### Telex

- `aa` → â, `ee` → ê, `oo` → ô
- `aw` → ă, `ow` → ơ, `uw` → ư
- `dd` → đ
- `s` → sắc, `f` → huyền, `r` → hỏi, `x` → ngã, `j` → nặng

### VNI

- `a1-5` → tones (á à ả ã ạ)
- `a6` → â, `a7` → ă
- `d9` → đ

## Requirements

- macOS 11.0 (Big Sur) or later

## Building from Source

```bash
# Build release
./scripts/build-release.sh

# Create DMG
./scripts/create-dmg.sh
```

## License

MIT
