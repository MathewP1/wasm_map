## How to build locally
1. Install Cmake 
2. Install python3
3. Install Emscripten SDK.
- `git clone https://github.com/emscripten-core/emsdk.git`
- `cd emsdk`
- `./emsdk install latest`
- `./emsdk activate latest`
- `source ./emsdk_env.sh` or optionally add this line to `~/.bash_profie` 
3. `git submodule update --init --recursive` to download third_party code.
4. Run build script: `./build_wasm.sh`
