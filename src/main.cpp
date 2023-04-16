// src/main.cpp

#include <array>
#include <iostream>

#include <GLES3/gl3.h>
#include <emscripten.h>
#include <emscripten/html5.h>

#include "examples.h"

namespace {
  int canvas_width = 0;
  int canvas_height = 0;
}

EMSCRIPTEN_KEEPALIVE bool initWebGL(EmscriptenWebGLContextAttributes attrs) {
  EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx =
      emscripten_webgl_create_context("#webglCanvas", &attrs);
  if (ctx <= 0) {
    std::cerr << "Failed to create WebGL context." << std::endl;
    return false;
  }

  emscripten_webgl_make_context_current(ctx);
  emscripten_get_canvas_element_size("#webglCanvas", &canvas_width,
                                     &canvas_height);
  glViewport(0, 0, canvas_width, canvas_height);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);  // Set background color
  glClear(GL_COLOR_BUFFER_BIT);  // Clear the canvas with the background color

  
  // emscripten_set_main_loop(render_frame, 0, 1);
  return true;
}

int main() {
  std::cout << "WebAssembly module loaded." << std::endl;

  EmscriptenWebGLContextAttributes attrs;
  emscripten_webgl_init_context_attributes(&attrs);
  attrs.majorVersion = 2;
  attrs.minorVersion = 0;

  if (!initWebGL(attrs)) {
    std::cerr << "Failed to initialize WebGL." << std::endl;
    return 1;
  }

  if (!DrawTriangle(canvas_width, canvas_height)) {
    std::cerr << "Failed to draw!" << std::endl;
    return 1;
  }

  emscripten_exit_with_live_runtime();
  return 0;
}