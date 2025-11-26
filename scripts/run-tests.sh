#!/bin/bash
# Simple test runner - compile all files together into test binary

set -e

echo "🔧 Running EndKey tests..."

BUILD_DIR="build/tests"
mkdir -p "$BUILD_DIR"

# Compile all files into a test binary
echo "📦 Compiling test files..."
swiftc \
    -F /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/Library/Frameworks \
    -framework XCTest \
    -framework Carbon \
    -framework Cocoa \
    -F /Applications/Xcode.app/Contents/Developer/Library/Frameworks \
    -framework SwiftUI \
    EndKey/Core/*.swift \
    EndKey/Models/*.swift \
    EndKey/Utils/*.swift \
    EndKeyTests/*.swift \
    -o "$BUILD_DIR/EndKeyTests" \
    2>&1 | tee "$BUILD_DIR/compile.log"

if [ $? -eq 0 ]; then
    echo "✅ Compilation successful!"
    echo "🧪 Running tests..."

    # Run tests
    "$BUILD_DIR/EndKeyTests"
else
    echo "❌ Compilation failed!"
    cat "$BUILD_DIR/compile.log"
    exit 1
fi
