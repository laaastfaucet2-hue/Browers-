#!/usr/bin/env bash
set -e

SRC_DIR="$HOME/chromium_build/src"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ARGS_FILE="$SCRIPT_DIR/../args.gn"
OUT_DIR="$SRC_DIR/out/Default"

echo "=== [4/4] Building Custom Chromium Fork ==="

if [ ! -d "$SRC_DIR" ]; then
    echo "Error: Chromium source tree not found at $SRC_DIR."
    exit 1
fi

export PATH="$HOME/depot_tools:$PATH"
cd "$SRC_DIR"

mkdir -p "$OUT_DIR"
cp "$ARGS_FILE" "$OUT_DIR/args.gn"

echo "Generating Ninja files via GN..."
gn gen "$OUT_DIR"

echo "Starting compilation with autoninja..."
echo "This will take several hours depending on CPU core count and RAM."
autoninja -C "$OUT_DIR" chrome

echo "=========================================================="
echo "Build finished! Binary output located at:"
echo "$OUT_DIR/chrome"
echo "=========================================================="
