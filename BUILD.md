# macOS App Build Guide

Build macOS apps using xcodebuild command line tools.

## Build Steps

1. **Kill existing processes**
   ```bash
   pkill -f "AppName"
   ```

2. **Clean build**
   ```bash
   xcodebuild -project path/to/Project.xcodeproj \
     -scheme SchemeName \
     -configuration Release \
     clean build \
     CODE_SIGNING_ALLOWED=NO
   ```

3. **Move app to root**
   ```bash
   find ~/Library/Developer/Xcode/DerivedData -name "AppName.app" -type d
   cp -r ~/Library/Developer/Xcode/DerivedData/*/Build/Products/Release/AppName.app ./AppName.app
   ```

4. **Sign app**
   ```bash
   codesign --force --deep --sign - ./AppName.app
   codesign --verify --verbose ./AppName.app
   ```

## Notes
- Replace `AppName` and `SchemeName` with actual values
- Ad-hoc signature enables system permissions
- `CODE_SIGNING_ALLOWED=NO` speeds up development builds