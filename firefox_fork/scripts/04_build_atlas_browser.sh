#!/usr/bin/env bash
set -e

GECKO_DIR="$HOME/firefox_fork_build/gecko-dev"

echo "=========================================================="
echo " [4/6] Building Atlas Firefox Browser with Mach"
echo "=========================================================="

if [ ! -d "$GECKO_DIR" ]; then
    echo "Error: Gecko source not found at $GECKO_DIR."
    exit 1
fi

cd "$GECKO_DIR"

echo "Starting Mozilla mach build..."
./mach build

echo "AtlasBrowser compilation completed successfully!"
