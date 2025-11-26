#!/bin/bash
set -e

PROJECT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_DIR="$PROJECT_DIR/build"
APP_NAME="EndKey"
VERSION="1.0.0"

APP_PATH="$BUILD_DIR/$APP_NAME.app"
DMG_PATH="$BUILD_DIR/$APP_NAME-$VERSION.dmg"
TEMP_DMG="$BUILD_DIR/temp.dmg"

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

# Mount DMG and extract mount point (handles spaces in volume names)
MOUNT_POINT=$(hdiutil attach "$TEMP_DMG" | grep "/Volumes/" | sed 's/.*\(\/Volumes\/.*\)/\1/')

# Copy app to DMG
cp -R "$APP_PATH" "$MOUNT_POINT/"

# Create Applications symlink for drag-to-install
ln -s /Applications "$MOUNT_POINT/Applications"

# Unmount DMG
hdiutil detach "$MOUNT_POINT"

# Convert to compressed read-only DMG
hdiutil convert "$TEMP_DMG" -format UDZO -o "$DMG_PATH"

# Cleanup temp file
rm -f "$TEMP_DMG"

echo "DMG created: $DMG_PATH"
ls -lh "$DMG_PATH"
