#!/usr/bin/env bash
set -e

SRC_DIR="$HOME/firefox_build/mozilla-unified"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PATCHES_DIR="$SCRIPT_DIR/../patches"

echo "=== [3/4] Applying Custom C++ / JS Patches to Firefox ==="

if [ ! -d "$SRC_DIR" ]; then
    echo "Error: Firefox source not found at $SRC_DIR."
    exit 1
fi

cd "$SRC_DIR"

for patch in "$PATCHES_DIR"/*.patch; do
    if [ -f "$patch" ]; then
        echo "Applying patch: $(basename "$patch")..."
        git apply --check "$patch" && git apply "$patch" || {
            echo "Failed to apply $(basename "$patch")."
            exit 1
        }
    fi
done

echo "Firefox custom patches applied successfully!"
