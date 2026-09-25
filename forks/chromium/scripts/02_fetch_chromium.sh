#!/usr/bin/env bash
set -e

# Target Chromium version / git tag (example: stable channel)
CHROMIUM_TAG=${1:-"128.0.6613.119"}
WORKSPACE_DIR="$HOME/chromium_build"

echo "=== [2/4] Fetching Chromium Source (Tag: $CHROMIUM_TAG) ==="
echo "WARNING: Chromium source code requires ~50-80 GB of disk space and fast internet."

mkdir -p "$WORKSPACE_DIR"
cd "$WORKSPACE_DIR"

export PATH="$HOME/depot_tools:$PATH"

if [ ! -d "src" ]; then
    echo "Running 'fetch --nohooks chromium'..."
    fetch --nohooks chromium
fi

cd src
echo "Checking out tag: $CHROMIUM_TAG..."
git fetch --tags
git checkout "tags/$CHROMIUM_TAG" -b "custom_fork_$CHROMIUM_TAG"

echo "Synchronizing submodules via gclient sync..."
gclient sync --with_branch_heads --with_tags

# Install build dependencies for Linux
if [ "$(uname)" = "Linux" ]; then
    echo "Running install-build-deps.sh..."
    ./build/install-build-deps.sh --no-prompt
fi

# Run hooks (generate ninja files, toolchains)
gclient runhooks

echo "Chromium checkout complete and ready for patches!"
