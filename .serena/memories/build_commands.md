# EndKey Build Commands

## Complete Rebuild Workflow (Primary Development Command)
```bash
# 1. Kill all running instances
killall -9 EndKey EndKeyHelper 2>/dev/null

# 2. Clean build directory
rm -rf Sources/EndKey/macOS/build/

# 3. Build with clean
xcodebuild -project Sources/EndKey/macOS/EndKey.xcodeproj \
  -target EndKey \
  -configuration Debug \
  CODE_SIGNING_ALLOWED=NO \
  clean build

# 4. Install to Applications
rm -rf /Applications/EndKey.app
cp -a Sources/EndKey/macOS/build/Debug/EndKey.app /Applications/

# 5. Sign for permissions persistence
codesign --force --deep --sign - /Applications/EndKey.app
```

## Build Targets Available
- **EndKey**: Main application target
- **EndKeyHelper**: Helper application for permissions
- **EndKeyUpdate**: Auto-updater component

## Xcode Project Configuration
- **Project File**: Sources/EndKey/macOS/EndKey.xcodeproj
- **Main Target**: EndKey (ModernKey app)
- **Helper Target**: EndKeyHelper
- **Update Target**: EndKeyUpdate
- **Configuration**: Debug (development), Release (production)

## Build Configurations
- **Debug**: Development với debug symbols
- **Release**: Production optimized build
- **Code Signing**: Required cho distribution
- **Entitlements**: Accessibility và system permissions

## Development Workflow Commands

### Clean Build Only
```bash
xcodebuild -project Sources/EndKey/macOS/EndKey.xcodeproj \
  -target EndKey \
  -configuration Debug \
  CODE_SIGNING_ALLOWED=NO \
  clean build
```

### Build Specific Target
```bash
xcodebuild -project Sources/EndKey/macOS/EndKey.xcodeproj \
  -target EndKeyHelper \
  -configuration Debug \
  CODE_SIGNING_ALLOWED=NO \
  build
```

### Install Applications
```bash
# Install main app
cp -a Sources/EndKey/macOS/build/Debug/EndKey.app /Applications/

# Install helper app
cp -a Sources/EndKey/macOS/build/Debug/EndKeyHelper.app /Applications/
```

### Code Signing
```bash
# Sign main app
codesign --force --deep --sign - /Applications/EndKey.app

# Sign helper app
codesign --force --deep --sign - /Applications/EndKeyHelper.app
```

## Testing Commands
```bash
# Monitor console logs
log stream --predicate 'process == "EndKey"'

# Test accessibility permissions
# System Preferences → Security & Privacy → Accessibility
```

## Claude-Flow SPARC Development
```bash
# SPARC methodology workflow
npx claude-flow@alpha sparc spec-pseudocode "Analyze Vietnamese input processing requirements"
npx claude-flow@alpha sparc architect "Design cross-platform engine architecture"
npx claude-flow@alpha sparc tdd "Implement Vietnamese tone mark conversion"
npx claude-flow@alpha sparc integration "Integrate engine with macOS UI"
```