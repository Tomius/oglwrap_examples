#version 330 core
in vec3 normal;

out vec4 fragColor;

void main() {
  vec3 lightPos = normalize(vec3(-0.3, 1, -0.2));
  float diffuseLighting = 0.8*max(dot(lightPos, normalize(normal)), 0.0) + 0.2;
  vec3 color = vec3(0.7, 0.5, 0.3);
  fragColor = vec4(diffuseLighting * color, 1.0);
}
