#!/usr/bin/env bash
# Usage: ./build.sh [debug|release]
set -euo pipefail
cd "$(dirname "$0")"

MODE="${1:-release}"
xmake f -y -p mingw -a x86_64 -m "$MODE" --mingw=/opt/homebrew
xmake -y

EXE="out/$MODE/windows-x86_64/CS2Loader.exe"
echo "Built: $EXE"
