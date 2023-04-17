#ifndef EXAMPLES_H
#define EXAMPLES_H

#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include <GLES3/gl3.h>
#include <emscripten.h>
#include <emscripten/val.h>

#include "c_string_view.h"
#include "shader.h"

constexpr CStringView ver1 = R"(#version 300 es
// an attribute is an input (in) to a vertex shader.
// It will receive data from a buffer
// in vec4 a_position;
in vec2 a_position;
uniform vec2 u_resolution;
uniform vec4 u_color;
out vec4 v_color;

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
  gl_Position = vec4(clipSpace * vec2(1, 1), 0, 1);
  v_color = u_color;
}
)";

constexpr CStringView frag1 = R"(#version 300 es
 
// fragment shaders don't have a default precision so we need
// to pick one. highp is a good default. It means "high precision"
precision highp float;
// we need to declare an output for the fragment shader
in vec4 v_color;
out vec4 outColor;
 
void main() {
  // Just set the output to a constant reddish-purple
  outColor = v_color;
}
)";

inline bool DrawTriangle(int canvas_width, int canvas_height) {
  auto shader = Shader::CreateFromStrings(ver1, frag1);
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
      10.0f, 20.0f, 80.0f, 20.0f, 10.0f, 30.0f,
      10.0f, 30.0f, 80.0f, 20.0f, 80.0f, 30.0f,
  };
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(),
               GL_STATIC_DRAW);

  shader->SetUniform("u_resolution", (GLfloat)canvas_width,
                     (GLfloat)canvas_height);

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glEnableVertexAttribArray(shader->GetAttributeLocation("a_position"));
  glVertexAttribPointer(shader->GetAttributeLocation("a_position"), 2, GL_FLOAT,
                        GL_FALSE, 0, 0);

  glDrawArrays(GL_TRIANGLES, 0, 6);
  return true;
}

inline std::vector<std::vector<float>> ReadMapData(
    const std::string& filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "Failed to open file: " << filename << std::endl;
    return {};
  }

  std::vector<std::vector<float>> polygons;
  std::string line;
  while (std::getline(file, line)) {
    std::vector<float> polygon;
    std::istringstream iss(line);
    float x;
    while (iss >> x) {
      polygon.push_back(x);
    }
    polygons.push_back(polygon);
  }
  return polygons;
}

// returns a vector of GLfloats with normalized coordinates and a center point
inline std::vector<GLfloat> GetNormalizedTriangleFanPolygon(
    const std::vector<float>& polygon,
    int canvas_width,
    int canvas_height) {
  std::vector<GLfloat> vertices;
  float sum_x = 0.0f;
  float sum_y = 0.0f;
  for (size_t i = 0; i < polygon.size(); i += 2) {
    // normalize to [0, canvas_width] and [0, canvas_height]
    float x = (polygon[i] + 90) / 180.0f * (float)canvas_width;
    float y = (polygon[i + 1] + 180) / 360.0f * (float)canvas_height;
    sum_x += x;
    sum_y += y;
    vertices.push_back(x);
    vertices.push_back(y);
  }
  // add center point
  vertices.insert(vertices.begin(), sum_y / (polygon.size() / 2));
  vertices.insert(vertices.begin(), sum_x / (polygon.size() / 2));

  return vertices;
}

inline bool DrawMap([[maybe_unused]] int canvas_width,
                    [[maybe_unused]] int canvas_height) {
  // open file with map data
  auto polygons = ReadMapData("/files/land_polygons.txt");
  if (polygons.empty()) {
    std::cerr << "Failed to read map data." << std::endl;
    return false;
  }

  std::cout << "Number of polygons: " << polygons.size() << std::endl;

  // Create shader
  auto shader = Shader::CreateFromStrings(ver1, frag1);
  if (!shader.has_value()) {
    std::cerr << "Failed to create shader." << std::endl;
    return false;
  }
  if (!shader->Compile()) {
    std::cerr << "Failed to compile shader." << std::endl;
    return false;
  }
  shader->Use();

  // Create buffer
  for (auto polygon : polygons) {
    auto vertices =
        GetNormalizedTriangleFanPolygon(polygon, canvas_width, canvas_height);
    std::cout << "Drawing vertices: " << std::endl;
    for (size_t i = 0; i < vertices.size(); i += 2) {
      std::cout << vertices[i] << ", " << vertices[i + 1] << std::endl;
    }
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    // glBufferData(GL_ARRAY_BUFFER, polygons[0].size() * sizeof(float),
    //              polygons[0].data(), GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat),
                 vertices.data(), GL_STATIC_DRAW);
    shader->SetUniform("u_color", (GLfloat)1.0f, (GLfloat)0.5f, (GLfloat)0.3f,
                       (GLfloat)1.0f);
    shader->SetUniform("u_resolution", (GLfloat)canvas_width,
                       (GLfloat)canvas_height);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(shader->GetAttributeLocation("a_position"));
    glVertexAttribPointer(shader->GetAttributeLocation("a_position"), 2,
                          GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays(GL_TRIANGLE_FAN, 0, vertices.size() / 2);
  }

  // GLuint* buffer = new GLuint[polygons.size()];
  // glGenBuffers(polygons.size(), buffer);
  // for (int i = 0; i < polygons.size(); ++i) {
  //   glBindBuffer(GL_ARRAY_BUFFER, buffer[i]);
  //   glBufferData(GL_ARRAY_BUFFER, polygons[i].size() * sizeof(float),
  //                polygons[i].data(), GL_STATIC_DRAW);
  //   shader->SetUniform("u_color", (GLfloat)1.0f/(i+1), (GLfloat)1.0f/(i+1),
  //                      (GLfloat)1.0f/(i+1), (GLfloat)1.0f);

  // }

  return true;
}

#endif