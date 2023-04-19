#ifndef SHADER_SOURCE_H
#define SHADER_SOURCE_H

#include "c_string_view.h"

constexpr CStringView ver1 = R"(#version 300 es
in vec2 a_position;
uniform vec4 u_color;
out vec4 v_color;

void main() {
  // positions will be lattitude and longitude
  // convert to x,y without any fancy projections (yet)
  float x = a_position.x / 180.0;
  float y = a_position.y / 90.0;
  gl_Position = vec4(x, y, 0, 1);
  v_color = u_color;
}
)";

constexpr CStringView ver_shperical = R"(#version 300 es
in vec2 a_position;
uniform vec4 u_color;
uniform float u_radius;
uniform float u_theta;
uniform float u_phi;
out vec4 v_color;

vec3 cylindricalToCartesian(float lat, float log, float radius) {
  float latitude = radians(lat);
  float longitude = radians(log);
  float x = radius * cos(longitude);
  float y = latitude;
  float z = radius * sin(longitude);
  return vec3(x, y, z);
}

vec2 cartesianToClipSpace(vec3 cartesian) {
  float x = cartesian.x / (cartesian.z + 1.0);
  float y = cartesian.y / (cartesian.z + 1.0);
  return vec2(x, y);
}

mat4 rotationMatrix(vec3 axis, float angle) {
  float s = sin(angle);
  float c = cos(angle);
  float oc = 1.0 - c;

  return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s, oc * axis.z * axis.x + axis.y * s, 0.0,
              oc * axis.x * axis.y + axis.z * s, oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s, 0.0,
              oc * axis.z * axis.x - axis.y * s, oc * axis.y * axis.z + axis.x * s, oc * axis.z * axis.z + c,           0.0,
              0.0,                                0.0,                                0.0,                                1.0);
}

void main() {
  v_color = u_color;
  float lat = a_position.y;
  float lon = a_position.x;
  
  float x = u_radius * (lon - u_theta) * cos(radians(36.0));
  float y = u_radius * (lat - u_phi);


  gl_Position = vec4(x, y, 0, 1);
}

)";

constexpr CStringView frag1 = R"(#version 300 es
precision highp float;
in vec4 v_color;
out vec4 outColor;
 
void main() {
  outColor = v_color;
}
)";

#endif  // SHADER_SOURCE_H