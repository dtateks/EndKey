---
trigger: always_on
---

always build after task done

```bash
# Kill existing processes
killall -9 EndKey EndKeyHelper 2>/dev/null

# Clean build
xcodebuild -project macOS/EndKey.xcodeproj \
  -target EndKey \
  -configuration Debug \
  CODE_SIGNING_ALLOWED=NO \
  clean build

# Install to Applications
cp -af macOS/build/Debug/EndKey.app /Applications/

# Sign app (required for persistent permissions)
codesign --force --deep --sign - /Applications/EndKey.app
```