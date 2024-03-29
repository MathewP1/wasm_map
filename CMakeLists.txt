# CMakeLists.txt

cmake_minimum_required(VERSION 3.12)
project(wasm_map)

set(CMAKE_CXX_STANDARD 17)

add_executable(wasm_map src/main.cpp
               src/shader.h src/shader.cpp
               src/map_data.h src/map_data.cpp
               src/app_state.h src/app_state.cpp
               src/renderer.h src/renderer.cpp)

include_directories(src/third_party/glm)               

# Set fail on warning
target_compile_options(wasm_map PRIVATE -Wall -Wextra -pedantic -Werror -pthread)

# Set up Emscripten options for WebAssembly build
if(EMSCRIPTEN)
    set_target_properties(wasm_map PROPERTIES
        SUFFIX ".js"
        LINK_FLAGS "--bind -s WASM=1 -s NO_EXIT_RUNTIME=1 \
        -s EXPORTED_FUNCTIONS=['_malloc','_free','_main'] \
        -s EXPORTED_RUNTIME_METHODS=['ccall','cwrap','getValue','setValue'] \
        -s USE_WEBGL2=1 -s FULL_ES2=1 \
        -s USE_PTHREADS=1 \
        --preload-file ${CMAKE_SOURCE_DIR}/files@/files"
    )
endif()