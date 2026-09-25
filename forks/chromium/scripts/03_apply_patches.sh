#!/usr/bin/env bash
set -e

SRC_DIR="$HOME/chromium_build/src"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PATCHES_DIR="$SCRIPT_DIR/../patches"

echo "=== [3/4] Applying Custom C++ Patches to Chromium ==="

if [ ! -d "$SRC_DIR" ]; then
    echo "Error: Chromium source tree not found at $SRC_DIR. Run 02_fetch_chromium.sh first."
    exit 1
fi

cd "$SRC_DIR"

for patch in "$PATCHES_DIR"/*.patch; do
    if [ -f "$patch" ]; then
        patch_name="$(basename "$patch")"
        echo "Applying patch: $patch_name..."
        git apply --check "$patch" && git apply "$patch" || {
            echo "Failed to apply $patch_name cleanly. Please review conflicts."
            exit 1
        }
    fi
done

echo "All custom patches applied successfully!"
