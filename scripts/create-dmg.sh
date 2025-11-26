#!/bin/bash
set -e

PROJECT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_DIR="$PROJECT_DIR/build"
APP_NAME="EndKey"
VERSION="1.0.0"

APP_PATH="$BUILD_DIR/$APP_NAME.app"
DMG_PATH="$BUILD_DIR/$APP_NAME-$VERSION.dmg"
TEMP_DMG="$BUILD_DIR/temp.dmg"

# Cleanup function for error handling
cleanup() {
    [ -n "$MOUNT_POINT" ] && [ -d "$MOUNT_POINT" ] && hdiutil detach "$MOUNT_POINT" 2>/dev/null || true
    rm -f "$TEMP_DMG"
}
trap cleanup EXIT

# Check if app exists
if [ ! -d "$APP_PATH" ]; then
  echo "Error: $APP_PATH not found. Run build-release.sh first."
  exit 1
fi

echo "Creating DMG..."

# Remove existing DMG
rm -f "$DMG_PATH" "$TEMP_DMG"

# Create temp DMG with sufficient size
hdiutil create -size 50m -fs HFS+ -volname "$APP_NAME" "$TEMP_DMG"

# Mount DMG and extract mount point using plist for reliable parsing
MOUNT_POINT=$(hdiutil attach "$TEMP_DMG" -plist | plutil -extract "system-entities.0.mount-point" raw - 2>/dev/null || \
              hdiutil attach "$TEMP_DMG" | grep "/Volumes/" | sed 's/.*\(\/Volumes\/.*\)/\1/')

if [ -z "$MOUNT_POINT" ] || [ ! -d "$MOUNT_POINT" ]; then
  echo "Error: Failed to mount DMG"
  exit 1
fi

# Copy app to DMG
cp -R "$APP_PATH" "$MOUNT_POINT/"

# Create Applications symlink for drag-to-install
ln -s /Applications "$MOUNT_POINT/Applications"

# Unmount DMG
hdiutil detach "$MOUNT_POINT"
MOUNT_POINT=""

# Convert to compressed read-only DMG
hdiutil convert "$TEMP_DMG" -format UDZO -o "$DMG_PATH"

echo "DMG created: $DMG_PATH"
ls -lh "$DMG_PATH"
