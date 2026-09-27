#!/usr/bin/env bash
set -e

GECKO_DIR="$HOME/firefox_fork_build/gecko-dev"

echo "=========================================================="
echo " [5/6] Packaging AtlasBrowser Distribution Packages"
echo "=========================================================="

cd "$GECKO_DIR"
./mach package

echo "Packaging complete! Installers generated in obj-dir/dist/"
