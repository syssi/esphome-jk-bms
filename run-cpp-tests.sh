#!/usr/bin/env bash
set -euo pipefail

REPO_DIR=$(cd "$(dirname "$0")" && pwd)
WORKSPACE=/tmp/$(basename "$REPO_DIR")
ESPHOME_DIR=$WORKSPACE/esphome
VENV_DIR=$WORKSPACE/venv

# Clone or reuse
if [[ ! -d "$ESPHOME_DIR/.git" ]]; then
    echo "Cloning esphome (dev)..."
    mkdir -p "$WORKSPACE"
    git clone --depth 1 --branch dev git@github.com:esphome/esphome.git "$ESPHOME_DIR"
else
    echo "esphome already cloned - skipping clone"
fi

# Sync each component dir individually so --delete only affects our files
for dir in "$REPO_DIR"/components/*/; do
    component=$(basename "$dir")
    rsync -a --delete "$dir" "$ESPHOME_DIR/esphome/components/$component/"
done

for dir in "$REPO_DIR"/tests/components/*/; do
    component=$(basename "$dir")
    rsync -a --delete "$dir" "$ESPHOME_DIR/tests/components/$component/"
done

# Set up venv once; delete $VENV_DIR to force reinstall
if [[ ! -d "$VENV_DIR" ]]; then
    echo "Creating Python venv..."
    python3 -m venv "$VENV_DIR"
    . "$VENV_DIR/bin/activate"
    pip install -q -r "$ESPHOME_DIR/requirements.txt" -r "$ESPHOME_DIR/requirements_test.txt"
    pip install -q -e "$ESPHOME_DIR"
fi

# CI symlinks venv inside the esphome dir
ln -sfn "$VENV_DIR" "$ESPHOME_DIR/venv"

COMPONENTS=${*:-$(ls "$REPO_DIR/tests/components")}

echo "Running C++ unit tests: $COMPONENTS"
. "$VENV_DIR/bin/activate"
cd "$ESPHOME_DIR"
PLATFORMIO_LIBDEPS_DIR=~/.platformio/libdeps \
ASAN_OPTIONS=detect_leaks=0 \
script/cpp_unit_test.py $COMPONENTS
