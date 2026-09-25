#!/usr/bin/env bash
set -e

FIREFOX_TAG=${1:-"FIREFOX_128_0_ESR_RELEASE"}
TARGET_DIR="$HOME/firefox_build"

echo "=== [2/4] Fetching Firefox Source Tree ==="
mkdir -p "$TARGET_DIR"
cd "$TARGET_DIR"

if [ ! -d "mozilla-unified" ]; then
    echo "Cloning mozilla-unified (shallow clone to save space)..."
    git clone --depth 1 --branch "$FIREFOX_TAG" https://github.com/mozilla/gecko-dev.git mozilla-unified
fi

cd mozilla-unified

# Run Mozilla bootstrap
./mach bootstrap --no-interactive --application-choice=browser

echo "Firefox source checkout complete!"
