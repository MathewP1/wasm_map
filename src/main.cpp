// src/main.cpp

#include <array>
#include <iostream>

#include <GLES3/gl3.h>
#include <emscripten.h>
#include <emscripten/html5.h>

#include "app_state.h"

void render_frame(void* data) {
  AppState* app = static_cast<AppState*>(data);
  app->Update();
  app->Render();
}

EMSCRIPTEN_KEEPALIVE bool initWebGL(EmscriptenWebGLContextAttributes attrs) {
  EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx =
      emscripten_webgl_create_context("#webglCanvas", &attrs);
  if (ctx <= 0) {
    std::cerr << "Failed to create WebGL context." << std::endl;
    return false;
  }
  emscripten_webgl_make_context_current(ctx);
  return true;
}

EM_BOOL KeyCallback(int event_type, const EmscriptenKeyboardEvent* e,
                    void* app_state) {
  AppState* app = static_cast<AppState*>(app_state);
  app->KeyEvent(event_type, e);
  return 0;
}

EM_BOOL MouseCallback(int event_type, const EmscriptenMouseEvent* e,
                       void* app_state) {
  AppState* app = static_cast<AppState*>(app_state);
  app->MouseEvent(event_type, e);
  return 0;
}

EM_BOOL UICallback(int event_type, const EmscriptenUiEvent* e,
                    void* app_state) {
  AppState* app = static_cast<AppState*>(app_state);
  app->UIEvent(event_type, e);
  return 0;
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

  int canvas_width, canvas_height;
  emscripten_get_canvas_element_size("#webglCanvas", &canvas_width,
                                     &canvas_height);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
  glViewport(0, 0, canvas_width, canvas_height);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // Set background color
  glClear(GL_COLOR_BUFFER_BIT);  // Clear the canvas with the background color

  AppState app_state(canvas_width, canvas_height);

  emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, &app_state, 1, KeyCallback);
  emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, &app_state, 1, KeyCallback);
  emscripten_set_mousedown_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, &app_state, 1, MouseCallback);
  emscripten_set_mouseup_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, &app_state, 1, MouseCallback);
  emscripten_set_mousemove_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, &app_state, 1, MouseCallback);
  emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, &app_state, 1, UICallback);

  emscripten_set_main_loop_arg(render_frame, &app_state, 0, 1);

  // emscripten_exit_with_live_runtime();
  return 0;
}