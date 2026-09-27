#!/usr/bin/env bash
set -e

echo "=========================================================="
echo " [1/6] Setting Up Gecko / Firefox Build Environment"
echo "=========================================================="

# 1. Update system packages
if [ -f /etc/debian_version ]; then
    sudo apt-get update
    sudo apt-get install -y \
        curl git python3 python3-pip python3-venv \
        build-essential clang lld libgtk-3-dev libasound2-dev \
        libpulse-dev libdbus-glib-1-dev libxt-dev libx11-xcb-dev \
        m4 libssl-dev libffi-dev mercurial
fi

# 2. Setup Rust toolchain (Gecko requires modern Rust for Servo & layout)
if ! command -v rustc &> /dev/null; then
    echo "Installing Rust toolchain..."
    curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y
    source "$HOME/.cargo/env"
fi

# Ensure cbindgen is installed
cargo install --force cbindgen

echo "Gecko build environment initialized successfully!"
