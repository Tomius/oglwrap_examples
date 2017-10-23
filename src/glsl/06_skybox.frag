// Copyright (c) Tamas Csala

#version 330 core

in vec3 vDirection;

uniform samplerCube uTex;

out vec4 fragColor;

void main() {
  fragColor = texture(uTex, normalize(vec3(vDirection.x, vDirection.y, vDirection.z)));
}
