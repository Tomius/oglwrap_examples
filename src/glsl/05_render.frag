#version 330 core
in vec3 position;
in vec3 normal;

uniform vec3 color;
uniform vec3 lightPos;
uniform mat4 shadowTransform;
uniform sampler2DShadow shadowMap;

out vec4 fragColor;

void main() {
  vec4 shadow_coord = shadowTransform * vec4(position, 1.0);
  shadow_coord.xyz /= shadow_coord.w;
  shadow_coord.z -= 0.005;
  shadow_coord.xyz = (shadow_coord.xyz + 1) * 0.5;

  float visibility = texture(shadowMap, shadow_coord.xyz);
  visibility = 0.2 + 0.8*visibility;
  float diffuseLighting = 0.9*visibility*max(dot(lightPos, normalize(normal)), 0.0) + 0.1;

  fragColor = vec4(diffuseLighting * color, 1.0);
}
