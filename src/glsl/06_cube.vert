#version 330 core
in vec4 inPos;
in vec3 inNormal;

uniform mat4 mvp;

out vec3 normal;

void main() {
  normal = inNormal;
  gl_Position = mvp * inPos;
}
