#!/usr/bin/env bash
set -e

echo "=== [1/4] Installing Firefox Build Prerequisites ==="

# 1. System packages
sudo apt-get update
sudo apt-get install -y \
    curl git python3 python3-pip python3-venv \
    build-essential clang lld libgtk-3-dev libasound2-dev \
    libpulse-dev libdbus-glib-1-dev libxt-dev

# 2. Rust and Cargo (Firefox core engine uses Rust & C++)
if ! command -v rustc &> /dev/null; then
    echo "Installing Rust toolchain via rustup..."
    curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y
    source "$HOME/.cargo/env"
fi

# 3. cbindgen
cargo install --force cbindgen

# 4. Mozilla bootstrap tool
python3 -m pip install --user mercurial

echo "Firefox build prerequisites installed!"
