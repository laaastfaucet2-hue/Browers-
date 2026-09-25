#!/usr/bin/env bash
set -e

echo "=== [1/4] Installing Chromium Build Prerequisites ==="

# Check OS
if [ -f /etc/debian_version ]; then
    echo "Detected Debian/Ubuntu system..."
    sudo apt-get update
    sudo apt-get install -y \
        git curl python3 python3-pip ninja-build lsb-release \
        build-essential pkg-config libglib2.0-dev libnss3-dev \
        libasound2-dev libpulse-dev libxss-dev libxtst-dev
else
    echo "Please adapt prerequisite packages for your Linux distribution / OS."
fi

# Install Chromium depot_tools
DEPOT_TOOLS_DIR="$HOME/depot_tools"
if [ ! -d "$DEPOT_TOOLS_DIR" ]; then
    echo "Cloning depot_tools into $DEPOT_TOOLS_DIR..."
    git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git "$DEPOT_TOOLS_DIR"
else
    echo "depot_tools already installed."
fi

# Add to PATH
export PATH="$DEPOT_TOOLS_DIR:$PATH"
echo "export PATH=\"$DEPOT_TOOLS_DIR:\$PATH\"" >> ~/.bashrc

echo "Depot tools ready. Verify with: gclient --version"
