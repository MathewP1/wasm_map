#!/bin/bash

# build_and_reload.sh

error_exit() {
    echo "Error: ${1:-"Unknown Error"}" 1>&2
    exit 1
}

# Check if emscripten is installed
if ! command -v emcmake &> /dev/null; then
    error_exit "emcmake not found. Please install emscripten"
fi

# Set your project's root directory
ROOT_DIR="$(dirname "$(realpath "$0")")"
BUILD_DIR="build/wasm"

# Move to the project root directory
cd "$ROOT_DIR" || error_exit "Failed to change directory to project root"

# Run python script to fetch map data
MAP_DATA_DIR="map_data"
cd "$MAP_DATA_DIR" || error_exit "Failed to change directory to $MAP_DATA_DIR"
# Create python venv if it doesn't exist
if [ ! -d "venv" ]; then
    python3 -m venv venv || error_exit "Failed to create python venv"
fi
# Activate venv
source venv/bin/activate || error_exit "Failed to activate python venv"
# Install dependencies
pip install -r requirements.txt || error_exit "Failed to install python dependencies"
# Run script
python3 prepare_map_data.py || error_exit "Failed to run prepare_map_data.py"
# Deactivate venv
deactivate || error_exit "Failed to deactivate python venv"
cd "$ROOT_DIR" || error_exit "Failed to change directory to project root"


# Build the WebAssembly project
mkdir -p "$BUILD_DIR" || error_exit "Failed to create build directory"
cd "$BUILD_DIR" || error_exit "Failed to change directory to $BUILD_DIR"
emcmake cmake ../.. -DCMAKE_BUILD_TYPE=Release || error_exit "emcmake cmake failed"
emmake make || error_exit "emmake make failed"

# Copy the generated files to the web directory
cp wasm_map.{js,wasm} ../../web || error_exit "Failed to copy wasm_map.{js,wasm} files"

# Run the server and reload the page (in the background)
pkill -f "python3 -m http.server" # Kill any existing server instances
cd ../../web || error_exit "Failed to change directory to web"
python3 -m http.server 8080 || error_exit "Failed to start HTTP server"