#version 330 core
in vec4 inPos;
in vec3 inNormal;

uniform mat4 mvp;
uniform mat4 model_mat;

out vec3 position;
out vec3 normal;

void main() {
  normal = inNormal;
  position = vec3(model_mat * inPos);
  gl_Position = mvp * inPos;
}
