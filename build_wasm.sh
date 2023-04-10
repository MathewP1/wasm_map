#!/bin/bash

# build_and_reload.sh

# Set your project's root directory
ROOT_DIR="$(dirname "$(realpath "$0")")"

# Move to the project root directory
cd "$ROOT_DIR" || exit 1

# Build the WebAssembly project
cd build/wasm || exit 1
emcmake cmake ../.. -DCMAKE_BUILD_TYPE=Release
emmake make

# Copy the generated files to the web directory
cp wasm_map.{js,wasm} ../../web

# Run the server and reload the page (in the background)
pkill -f "python3 -m http.server" # Kill any existing server instances
cd ../../web || exit 1
python3 -m http.server 8080