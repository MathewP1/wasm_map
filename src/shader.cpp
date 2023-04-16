#include "shader.h"

#include <iostream>

namespace {
std::optional<GLuint> CompileShader(GLenum shader_type, CStringView source) {
  GLuint shader = glCreateShader(shader_type);
  auto source_cstr = source.data();
  auto source_length = static_cast<GLint>(source.size());
  glShaderSource(shader, 1, &source_cstr, &source_length);
  glCompileShader(shader);
  GLint compiled;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
  if (compiled != GL_TRUE) {
    GLsizei log_length = 0;
    GLchar message[1024];
    glGetShaderInfoLog(shader, 1024, &log_length, message);
    std::cerr << "ERROR: Shader compilation failed with message:\n"
              << message << std::endl;
    return std::nullopt;
  }
  return std::make_optional(shader);
}
}  // namespace

Shader::Shader([[maybe_unused]]const Key<Shader>& key,
               GLuint compiled_vertex_shader,
               GLuint compiled_fragment_shader)
    : vertex_shader_(compiled_vertex_shader),
      fragment_shader_(compiled_fragment_shader) {
  program_ = glCreateProgram();
}

// void Shader::AddAttribute(CStringView name) {
//   glBindAttribLocation(program_, attribute_count_++, name.data());
// }

bool Shader::Compile() {
  glAttachShader(program_, vertex_shader_);
  glAttachShader(program_, fragment_shader_);
  glLinkProgram(program_);

  GLint linked;
  glGetProgramiv(program_, GL_LINK_STATUS, &linked);
  if (linked != GL_TRUE) {
    GLsizei log_length = 0;
    GLchar message[1024];
    glGetProgramInfoLog(program_, 1024, &log_length, message);
    std::cerr << "ERROR: Shader linking failed with message:\n"
              << message << std::endl;
    return false;
  }
  return true;
}

GLint Shader::GetAttributeLocation(CStringView name) {
  // if (!IsNullTerminated(name)) {
  //     std::cerr << "ERROR: Shader::GetAttributeLocation: name is not null
  //     terminated" << std::endl; return -1;
  // }
  return glGetAttribLocation(program_, name.data());
}

void Shader::Use() const {
  glUseProgram(program_);
}

std::optional<Shader> Shader::CreateFromStrings(CStringView vertex_source,
                                                CStringView fragment_source) {
  // if (!IsNullTerminated(vertex_source) || !IsNullTerminated(fragment_source))
  // {
  //   std::cerr << "ERROR: Shader::CreateFromStrings: string is not null
  //   terminated" << std::endl; return std::nullopt;
  // }
  auto vertex_shader = CompileShader(GL_VERTEX_SHADER, vertex_source);
  if (!vertex_shader.has_value()) {
    return std::nullopt;
  }
  auto fragment_shader = CompileShader(GL_FRAGMENT_SHADER, fragment_source);
  if (!fragment_shader.has_value()) {
    return std::nullopt;
  }

  Key<Shader> key;
  return std::make_optional<Shader>(key, vertex_shader.value(),
                                    fragment_shader.value());
}

void Shader::SetUniformImpl(GLint location, GLfloat v0) {
  glUniform1f(location, v0);
}

void Shader::SetUniformImpl(GLint location, GLfloat v0, GLfloat v1) {
  glUniform2f(location, v0, v1);
}

void Shader::SetUniformImpl(GLint location,
                            GLfloat v0,
                            GLfloat v1,
                            GLfloat v2) {
  glUniform3f(location, v0, v1, v2);
}

void Shader::SetUniformImpl(GLint location,
                            GLfloat v0,
                            GLfloat v1,
                            GLfloat v2,
                            GLfloat v3) {
  glUniform4f(location, v0, v1, v2, v3);
}

void Shader::SetUniformImpl(GLint location, GLint v0) {
  glUniform1i(location, v0);
}

void Shader::SetUniformImpl(GLint location, GLint v0, GLint v1) {
  glUniform2i(location, v0, v1);
}

void Shader::SetUniformImpl(GLint location, GLint v0, GLint v1, GLint v2) {
  glUniform3i(location, v0, v1, v2);
}

void Shader::SetUniformImpl(GLint location,
                            GLint v0,
                            GLint v1,
                            GLint v2,
                            GLint v3) {
  glUniform4i(location, v0, v1, v2, v3);
}