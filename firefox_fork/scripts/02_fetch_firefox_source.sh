#!/usr/bin/env bash
set -e

# Target Firefox ESR / Release tag (example: FIREFOX_128_0_ESR_RELEASE)
FIREFOX_VERSION=${1:-"FIREFOX_128_0_ESR_RELEASE"}
BUILD_ROOT="$HOME/firefox_fork_build"

echo "=========================================================="
echo " [2/6] Fetching Firefox Source Tree (Tag: $FIREFOX_VERSION)"
echo "=========================================================="

mkdir -p "$BUILD_ROOT"
cd "$BUILD_ROOT"

if [ ! -d "gecko-dev" ]; then
    echo "Cloning mozilla/gecko-dev (shallow clone to save disk)..."
    git clone --depth 1 --branch "$FIREFOX_VERSION" https://github.com/mozilla/gecko-dev.git gecko-dev
fi

cd gecko-dev

echo "Running Mozilla bootstrap..."
./mach bootstrap --no-interactive --application-choice=browser

echo "Firefox source checkout ready for Atlas custom patches!"
