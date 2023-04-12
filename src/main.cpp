// src/main.cpp

#include <array>
#include <iostream>

#include <GLES3/gl3.h>
#include <emscripten.h>
#include <emscripten/html5.h>

#include "shader.h"
#include "utility.h"

// constexpr std::string_view ver = NullTerminate(R"(#version 300 es
 
// // an attribute is an input (in) to a vertex shader.
// // It will receive data from a buffer
// in vec4 a_position;
 
// // all shaders have a main function
// void main() {
 
//   // gl_Position is a special variable a vertex shader
//   // is responsible for setting
//   gl_Position = a_position;
// }
// )");
// constexpr std::string_view frag = NullTerminate(R"(#version 300 es
 
// // fragment shaders don't have a default precision so we need
// // to pick one. highp is a good default. It means "high precision"
// precision highp float;
 
// // we need to declare an output for the fragment shader
// out vec4 outColor;
 
// void main() {
//   // Just set the output to a constant reddish-purple
//   outColor = vec4(1, 0, 0.5, 1);
// }
// )");

constexpr CStringView ver_cstr = R"(#version 300 es
// an attribute is an input (in) to a vertex shader.
// It will receive data from a buffer
// in vec4 a_position;
in vec2 a_position;
uniform vec2 u_resolution;

// all shaders have a main function
void main() {
  // convert the position from pixels to 0.0 to 1.0
  vec2 zeroToOne = a_position / u_resolution;
  // convert from 0->1 to 0->2
  vec2 zeroToTwo = zeroToOne * 2.0;
  // convert from 0->2 to -1->+1 (clip space)
  vec2 clipSpace = zeroToTwo - 1.0;

  // gl_Position is a special variable a vertex shader
  // is responsible for setting
  gl_Position = vec4(clipSpace, 0, 1);
}
)";

constexpr CStringView frag_cstr = R"(#version 300 es
 
// fragment shaders don't have a default precision so we need
// to pick one. highp is a good default. It means "high precision"
precision highp float;
 
// we need to declare an output for the fragment shader
out vec4 outColor;
 
void main() {
  // Just set the output to a constant reddish-purple
  outColor = vec4(1, 0, 0.5, 1);
}
)";

void render_frame() {
  static float r = 0.0f;
  static float g = 0.2f;
  static float b = 0.4f;
  if (r += 0.01f; r > 1.0f)
    r = 0.0f;
  if (g += 0.03f; g > 1.0f)
    g = 0.0f;
  if (b += 0.02f; b > 1.0f)
    b = 0.0f;
  glClearColor(r, g, b, 1.0f);   // Set background color
  glClear(GL_COLOR_BUFFER_BIT);  // Clear the canvas with the background color
  emscripten_webgl_commit_frame();  // Commit the frame
}

EMSCRIPTEN_KEEPALIVE bool initWebGL(EmscriptenWebGLContextAttributes attrs) {
  EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx = emscripten_webgl_create_context("#webglCanvas", &attrs);
  if (ctx <= 0) {
    std::cerr << "Failed to create WebGL context." << std::endl;
    return false;
  }

  emscripten_webgl_make_context_current(ctx);
  int canvas_width, canvas_height;
  emscripten_get_canvas_element_size("#webglCanvas", &canvas_width, &canvas_height);
  glViewport(0, 0, canvas_width, canvas_height);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);   // Set background color
  glClear(GL_COLOR_BUFFER_BIT);  // Clear the canvas with the background color

  auto shader = Shader::CreateFromStrings(ver_cstr, frag_cstr);
  if (!shader.has_value()) {
    std::cerr << "Failed to create shader." << std::endl;
    return false;
  }
  if (!shader->Compile()) {
    std::cerr << "Failed to compile shader." << std::endl;
    return false;
  }
  shader->Use();

  GLuint buffer;
  glGenBuffers(1, &buffer);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);

  std::array<GLfloat, 12> vertices = {
    10.0f, 20.0f,
    80.0f, 20.0f,
    10.0f, 30.0f,
    10.0f, 30.0f,
    80.0f, 20.0f,
    80.0f, 30.0f,
  };
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);

  shader->SetUniform("u_resolution", (GLfloat)canvas_width, (GLfloat)canvas_height);

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glEnableVertexAttribArray(shader->GetAttributeLocation("a_position"));
  glVertexAttribPointer(shader->GetAttributeLocation("a_position"), 2, GL_FLOAT, GL_FALSE, 0, 0);


  glDrawArrays(GL_TRIANGLES, 0, 6);

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
  // emscripten_exit_with_live_runtime();
  return 0;
}