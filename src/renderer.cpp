#include "renderer.h"

#include "c_string_view.h"
#include "shader_source.h"


Renderer::Renderer() {
  shader_ = Shader::CreateFromStrings(ver1, frag1);
  if(!shader_.has_value()) {
    std::cerr << "Failed to create shader." << std::endl;
    return;
  }
  if (!shader_->Compile()) {
    std::cerr << "Failed to compile shader." << std::endl;
    return;
  }

  spherical_shader_ = Shader::CreateFromStrings(ver_shperical, frag1);
  if(!spherical_shader_.has_value()) {
    std::cerr << "Failed to create spherical shader." << std::endl;
    return;
  }
  if (!spherical_shader_->Compile()) {
    std::cerr << "Failed to compile spherical shader." << std::endl;
    return;
  }

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  vao_ = vao;
  can_render_ = true;
}

Renderer::~Renderer() {
  if (!can_render_) {
    return;
  }
  glDeleteVertexArrays(1, &vao_);
  for (const auto& polygon : polygons_) {
    glDeleteBuffers(1, &polygon.vbo);
  }
}

void Renderer::AddPolygon(const std::vector<glm::vec2>& polygon) {
  if (!can_render_ || polygon.size() < 2) {
    return;
  }
  GLuint buffer;
  glGenBuffers(1, &buffer);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBufferData(GL_ARRAY_BUFFER, polygon.size() * sizeof(GLfloat) * 2, polygon.data(), GL_STATIC_DRAW);
  
  shader_->Use();
  shader_->SetUniform("u_color", (GLfloat)1.0f, (GLfloat)0.0f, (GLfloat)0.0f, (GLfloat)1.0f);
  glEnableVertexAttribArray(shader_->GetAttributeLocation("a_position"));
  glVertexAttribPointer(shader_->GetAttributeLocation("a_position"), 2, GL_FLOAT, GL_FALSE, 0, 0);
  shader_->Unuse();
  polygons_.push_back({buffer, polygon.size(), GL_TRIANGLE_FAN, shader_.value()});
}

void Renderer::AddOutline(const std::vector<glm::vec2>& outline) {
  if (!can_render_ || outline.size() < 2) {
    return;
  }
  GLuint buffer;
  glGenBuffers(1, &buffer);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBufferData(GL_ARRAY_BUFFER, outline.size() * sizeof(GLfloat) * 2, outline.data(), GL_STATIC_DRAW);
  
  spherical_shader_->Use();
  spherical_shader_->SetUniform("u_color", (GLfloat)0.0f, (GLfloat)0.0f, (GLfloat)1.0f, (GLfloat)1.0f);
  spherical_shader_->SetUniform("u_radius", (GLfloat)1.0f);
  spherical_shader_->SetUniform("u_phi", (GLfloat)0.0f);
  spherical_shader_->SetUniform("u_theta", (GLfloat)0.0f);
  glEnableVertexAttribArray(spherical_shader_->GetAttributeLocation("a_position"));
  glVertexAttribPointer(spherical_shader_->GetAttributeLocation("a_position"), 2, GL_FLOAT, GL_FALSE, 0, 0);
  spherical_shader_->Unuse();
  polygons_.push_back({buffer, outline.size(), GL_LINE_STRIP, spherical_shader_.value()});
}


void Renderer::Render() {
  if (!can_render_) {
    return;
  }
  
  for (const auto& polygon : polygons_) {
    polygon.shader.Use();
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, polygon.vbo);
    glEnableVertexAttribArray(shader_->GetAttributeLocation("a_position"));
    glVertexAttribPointer(shader_->GetAttributeLocation("a_position"), 2, GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(polygon.mode, 0, polygon.vertices_count);
  }
}