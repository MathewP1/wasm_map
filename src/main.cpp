// src/main.cpp

#include <iostream>
#include <emscripten.h>
#include <emscripten/html5.h>
#include <GLES2/gl2.h>

// can use emscripten_set_main_loop_arg to avoid global variable
EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx;

void render_frame() {
    static float r = 0.0f;
    static float g = 0.2f;
    static float b = 0.4f;
    if (r += 0.01f; r > 1.0f) r = 0.0f;
    if (g += 0.03f; g > 1.0f) g = 0.0f;
    if (b += 0.02f; b > 1.0f) b = 0.0f;
    glClearColor(r, g, b, 1.0f); // Set background color
    glClear(GL_COLOR_BUFFER_BIT);         // Clear the canvas with the background color
    emscripten_webgl_commit_frame();      // Commit the frame
}

EMSCRIPTEN_KEEPALIVE void initWebGL(EmscriptenWebGLContextAttributes attrs) {
    ctx = emscripten_webgl_create_context("#webglCanvas", &attrs);
    if (ctx <= 0) {
        std::cerr << "Failed to create WebGL context." << std::endl;
        return;
    }
    
    emscripten_webgl_make_context_current(ctx);
    emscripten_set_main_loop(render_frame, 0, 1);
}

int main() {
    std::cout << "WebAssembly module loaded." << std::endl;
    
    EmscriptenWebGLContextAttributes attrs;
    emscripten_webgl_init_context_attributes(&attrs);
    attrs.majorVersion = 2;
    attrs.minorVersion = 0;
    
    initWebGL(attrs);
    emscripten_exit_with_live_runtime();
    return 0;
}