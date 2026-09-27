#!/usr/bin/env bash
set -e

GECKO_DIR="$HOME/firefox_fork_build/gecko-dev"

echo "=========================================================="
echo " [6/6] Launching AtlasBrowser (Gecko Engine)"
echo "=========================================================="

cd "$GECKO_DIR"
./mach run "$@"
