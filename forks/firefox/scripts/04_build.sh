#!/usr/bin/env bash
set -e

SRC_DIR="$HOME/firefox_build/mozilla-unified"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
MOZCONFIG_FILE="$SCRIPT_DIR/../mozconfig"

echo "=== [4/4] Building Custom Firefox Fork with Mach ==="

if [ ! -d "$SRC_DIR" ]; then
    echo "Error: Firefox source not found at $SRC_DIR."
    exit 1
fi

cd "$SRC_DIR"
cp "$MOZCONFIG_FILE" .mozconfig

echo "Running ./mach build..."
./mach build

echo "Packaging browser binaries..."
./mach package

echo "=========================================================="
echo "Firefox build complete! Run with: ./mach run"
echo "=========================================================="
