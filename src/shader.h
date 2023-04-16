#ifndef SHADER_H
#define SHADER_H

#include <iostream>
#include <optional>
#include <string>

#include <GLES3/gl3.h>

#include "c_string_view.h"

template <typename T>
class Key {
 private:
  Key() = default;
  // prevents this from being an aggregate so avoids aggregate initialization
  // which doesn't have public/private distinction
  Key(int) {}
  friend T;
};

class Shader {
 public:
  // public constructor with private key
  Shader(const Key<Shader>& key,
         GLuint compiled_vertex_shader,
         GLuint compiled_fragment_shader);
  //   void AddAttribute(std::string_view name);
  // Get Attribute Location?
  bool Compile();

  template <typename... T>
  bool SetUniform(CStringView name, T&&... value) {
    static_assert(sizeof...(value) <= 4 && sizeof...(value) > 0,
                  "Shader::SetUniform: uniform has more than 4 values");
    GLint location = glGetUniformLocation(program_, name.data());
    if (location == -1) {
      std::cerr << "ERROR: Shader::SetUniform: uniform " << name << " not found"
                << std::endl;
      return false;
    }
    SetUniformImpl(location, std::forward<T>(value)...);
    return true;
  }

  GLint GetAttributeLocation(CStringView c_sv);
  void Use() const;
  static std::optional<Shader> CreateFromStrings(CStringView vertex_source,
                                                 CStringView fragment_source);

 private:
  GLuint program_{0}, vertex_shader_{0}, fragment_shader_{0};

  // Specializations for GLfloat
  void SetUniformImpl(GLint location, GLfloat v0);
  void SetUniformImpl(GLint location, GLfloat v0, GLfloat v1);
  void SetUniformImpl(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
  void SetUniformImpl(GLint location,
                      GLfloat v0,
                      GLfloat v1,
                      GLfloat v2,
                      GLfloat v3);

  // Specializations for GLint
  void SetUniformImpl(GLint location, GLint v0);
  void SetUniformImpl(GLint location, GLint v0, GLint v1);
  void SetUniformImpl(GLint location, GLint v0, GLint v1, GLint v2);
  void SetUniformImpl(GLint location, GLint v0, GLint v1, GLint v2, GLint v3);

  template <typename... T>
  void SetUniformImpl([[maybe_unused]]GLint location, [[maybe_unused]]T&&... value) {
    static_assert(true,
                  "Shader::SetUniform: uniform has unsupported type (only "
                  "GLfloat and GLint are supported)");
  }
};

#endif
