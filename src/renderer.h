#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#include <optional>

#include <glm/glm.hpp>

#include "shader.h"

// Renderer expects that vertices are in lattitude and longitude format.
class Renderer {
 public:
  Renderer();
  ~Renderer();	
  // TODO: fix, doesn't work properly :), has to convert given vertices to triangle fan or triangulate
  void AddPolygon(const std::vector<glm::vec2>& polygon);
  void AddOutline(const std::vector<glm::vec2>& outline);

  void Render();

 private:
  struct RenderObject {
    GLuint vbo{0};
    std::size_t vertices_count{0};
    GLenum mode{GL_TRIANGLES};
    Shader& shader;
  };
  std::vector<RenderObject> polygons_;

  bool can_render_{false};
  std::optional<Shader> shader_;
  std::optional<Shader> spherical_shader_;
  GLuint vao_{0};
};

#endif  // RENDERER_H