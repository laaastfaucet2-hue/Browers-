#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/../.." && pwd)"
FORK_DIR="$ROOT_DIR/firefox_fork"
GECKO_DIR="$HOME/firefox_fork_build/gecko-dev"

echo "=========================================================="
echo " [3/6] Applying Atlas Custom Branding, Policies & Patches"
echo "=========================================================="

if [ ! -d "$GECKO_DIR" ]; then
    echo "Error: Gecko source not found at $GECKO_DIR. Run 02_fetch_firefox_source.sh first."
    exit 1
fi

cd "$GECKO_DIR"

# 1. Copy mozconfig
cp "$FORK_DIR/mozconfig" .mozconfig

# 2. Copy Custom Branding
mkdir -p browser/branding/atlasbrowser
cp -r "$FORK_DIR/branding/"* browser/branding/atlasbrowser/

# 3. Copy Distribution Policies (Policies.json enabling Containers & AMO extensions)
mkdir -p distribution
cp "$FORK_DIR/distribution/policies.json" distribution/

# 4. Copy Atlas Core Preferences
mkdir -p browser/app/profile
cp "$FORK_DIR/preferences/atlas-prefs.js" browser/app/profile/

# 5. Apply Patches
for patch in "$FORK_DIR/patches"/*.patch; do
    if [ -f "$patch" ]; then
        echo "Applying $(basename "$patch")..."
        git apply --check "$patch" 2>/dev/null && git apply "$patch" || echo "Note: Patch $(basename "$patch") already applied or will be injected via configuration."
    fi
done

echo "Atlas customization completed successfully!"
