#version 330 core
in vec4 inPos;

uniform mat4 mvp;

void main() {
  gl_Position = mvp * inPos;
}
