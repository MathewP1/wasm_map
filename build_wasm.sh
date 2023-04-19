#!/bin/bash

# Add command line argument
if [ "$1" = "-h" ] || [ "$1" = "--help" ]; then
    echo "Usage: ./build_wasm.sh"
    echo "Builds the WebAssembly project and copies the generated files to the web directory"
    exit 0
fi
# Add command line argument to skip map data generation
if [ "$1" = "-s" ] || [ "$1" = "--skip-map-data" ]; then
    SKIP_MAP_DATA=true
fi

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

# Skip this part is command line arg is set
if [ -z "$SKIP_MAP_DATA" ]; then
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
    # Copy the generated files to files directory
    mkdir -p "$ROOT_DIR/files" || error_exit "Failed to create files directory"
    cp land_polygons.txt "$ROOT_DIR/files" || error_exit "Failed to copy land_polygons.txt to files directory"
    # Deactivate venv
    deactivate || error_exit "Failed to deactivate python venv"
    cd "$ROOT_DIR" || error_exit "Failed to change directory to project root"
fi


# Build the WebAssembly project
mkdir -p "$BUILD_DIR" || error_exit "Failed to create build directory"
cd "$BUILD_DIR" || error_exit "Failed to change directory to $BUILD_DIR"
emcmake cmake ../.. -DCMAKE_BUILD_TYPE=Release || error_exit "emcmake cmake failed"
emmake make || error_exit "emmake make failed"

# Copy the generated files to the web directory
cp wasm_map.{js,wasm,data,worker.js} "$ROOT_DIR/web" || error_exit "Failed to copy wasm_map.{js,wasm} files"

# Run the server and reload the page (in the background)
pkill -f "python3 server.py" # Kill any existing server instances
cd ../../web || error_exit "Failed to change directory to web"
python3  ../server.py || error_exit "Failed to start HTTP server"