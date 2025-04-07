#!/bin/bash

set -e

# === Configuration ===
QT_OPCUA_REPO="https://github.com/qt/qtopcua.git"
SRC_DIR=~/qtopcua
BUILD_DIR=$SRC_DIR/build
PLUGIN_DIR="/usr/lib/x86_64-linux-gnu/qt5/plugins/opcua"

echo "📁 Checking for existing Qt OPC UA source..."
if [ ! -d "$SRC_DIR" ]; then
    echo "📥 Cloning Qt OPC UA..."
    git clone "$QT_OPCUA_REPO" "$SRC_DIR"
else
    echo "✅ Qt OPC UA source already exists at $SRC_DIR"
fi

# === Create build dir ===
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo "🧰 Running qmake with CFLAGS fix..."
qmake "QMAKE_CFLAGS+=-Wno-error=format-security -Wformat" "$SRC_DIR"

echo "🔨 Building all of Qt OPC UA (including plugins)..."
make -j"$(nproc)"

echo "📦 Installing..."
sudo make install

echo "🔍 Checking if open62541 plugin was installed..."
if [ -d "$PLUGIN_DIR" ]; then
    ls "$PLUGIN_DIR"
    echo "✅ Plugin directory found: $PLUGIN_DIR"
else
    echo "⚠️ Plugin directory not found. You may need to set QT_PLUGIN_PATH manually."
fi

echo "🎉 All done! Qt OPC UA is built and installed with open62541 plugin."

