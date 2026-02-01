#!/bin/bash
set -e

# Configuration
PROJECT_NAME="EA V-CORE"
VERSION="1.0.0"
IDENTIFIER="com.emuaudio.eavcore"
BUILD_DIR="build_mac"
ARTIFACTS_DIR="$BUILD_DIR/EA_V-CORE_artefacts/Release"
OUTPUT_DIR="Installers"

VST3_SOURCE="$ARTIFACTS_DIR/VST3/$PROJECT_NAME.vst3"
AU_SOURCE="$ARTIFACTS_DIR/AU/$PROJECT_NAME.component"

echo "Started Packaging for $PROJECT_NAME v$VERSION..."

# Ensure build artifacts exist
if [ ! -d "$VST3_SOURCE" ] || [ ! -d "$AU_SOURCE" ]; then
    echo "Error: Build artifacts not found. Please run ./build_mac.sh first."
    exit 1
fi

mkdir -p "$OUTPUT_DIR"
mkdir -p "$OUTPUT_DIR/payload/vst3"
mkdir -p "$OUTPUT_DIR/payload/au"

# 1. Prepare Component Packages
echo "Building VST3 Package..."
pkgbuild --root "$VST3_SOURCE" \
         --install-location "/Library/Audio/Plug-Ins/VST3/$PROJECT_NAME.vst3" \
         --identifier "$IDENTIFIER.vst3" \
         --version "$VERSION" \
         "$OUTPUT_DIR/vst3.pkg"

echo "Building AU Package..."
pkgbuild --root "$AU_SOURCE" \
         --install-location "/Library/Audio/Plug-Ins/Components/$PROJECT_NAME.component" \
         --identifier "$IDENTIFIER.au" \
         --version "$VERSION" \
         "$OUTPUT_DIR/au.pkg"

# 2. Build Product Package (Combined)
echo "Building Final Bundle..."
productbuild --distribution "distribution.xml" \
             --package-path "$OUTPUT_DIR" \
             --resources "Resources" \
             "$OUTPUT_DIR/$PROJECT_NAME Installer mac.pkg"

# Cleanup intermediate files
rm "$OUTPUT_DIR/vst3.pkg"
rm "$OUTPUT_DIR/au.pkg"

echo "Packaging Complete!"
echo "Installer: $OUTPUT_DIR/$PROJECT_NAME Installer mac.pkg"
