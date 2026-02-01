#!/bin/bash
set -e

# Find CMake
echo "Searching for CMake..."
if command -v cmake &> /dev/null; then
    CMAKE_CMD="cmake"
elif [ -f "/Applications/CMake.app/Contents/bin/cmake" ]; then
    CMAKE_CMD="/Applications/CMake.app/Contents/bin/cmake"
elif [ -f "/usr/local/bin/cmake" ]; then
    CMAKE_CMD="/usr/local/bin/cmake"
elif [ -f "/opt/homebrew/bin/cmake" ]; then
    CMAKE_CMD="/opt/homebrew/bin/cmake"
else
    echo "Error: CMake not found. Please install CMake."
    exit 1
fi

echo "Using CMake at: $CMAKE_CMD"

BUILD_DIR="build_mac"

echo "Configuring..."
"$CMAKE_CMD" -B "$BUILD_DIR" -S . -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64"

echo "Building..."
"$CMAKE_CMD" --build "$BUILD_DIR" --config Release --parallel 4

echo "Installation..."
# Note: JUCE 8 might output artifacts in a slightly different structure or name depending on the project name.
# We will use 'find' to locate the artifacts to be safe.

VST3_PATH=$(find "$BUILD_DIR" -name "EA V-CORE.vst3" -type d | head -n 1)
AU_PATH=$(find "$BUILD_DIR" -name "EA V-CORE.component" -type d | head -n 1)

VST3_DEST="$HOME/Library/Audio/Plug-Ins/VST3/"
AU_DEST="$HOME/Library/Audio/Plug-Ins/Components/"

mkdir -p "$VST3_DEST"
mkdir -p "$AU_DEST"

if [ -n "$VST3_PATH" ] && [ -d "$VST3_PATH" ]; then
    echo "Copying VST3 from $VST3_PATH to $VST3_DEST"
    rm -rf "$VST3_DEST/EA V-CORE.vst3"
    cp -R "$VST3_PATH" "$VST3_DEST"
else
    echo "Warning: VST3 artifact not found."
fi

# VST Path
VST_PATH=$(find "$BUILD_DIR" -maxdepth 4 -name "EA V-CORE.vst" -type d | head -n 1)
# Note: JUCE might output .vst inside VST folder or similar.

if [ -n "$AU_PATH" ] && [ -d "$AU_PATH" ]; then
    echo "Copying AU from $AU_PATH to $AU_DEST"
    rm -rf "$AU_DEST/EA V-CORE.component"
    cp -R "$AU_PATH" "$AU_DEST"
else
    echo "Warning: AU artifact not found."
fi

# Force AudioComponentRegistrar check
killall -9 AudioComponentRegistrar 2>/dev/null || true

echo "Build and Install Complete!"
