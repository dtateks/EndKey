#!/bin/bash
set -e

PROJECT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_DIR="$PROJECT_DIR/build"
APP_NAME="EndKey"

echo "Building $APP_NAME..."

# Clean build folder
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"

# Build release using xcodebuild
xcodebuild -project "$PROJECT_DIR/$APP_NAME.xcodeproj" \
  -scheme "$APP_NAME" \
  -configuration Release \
  -derivedDataPath "$BUILD_DIR/DerivedData" \
  build

# Copy built app to build folder
APP_PATH="$BUILD_DIR/DerivedData/Build/Products/Release/$APP_NAME.app"

if [ -d "$APP_PATH" ]; then
  cp -R "$APP_PATH" "$BUILD_DIR/"
  echo "Build complete: $BUILD_DIR/$APP_NAME.app"
else
  echo "Error: Built app not found at $APP_PATH"
  exit 1
fi
