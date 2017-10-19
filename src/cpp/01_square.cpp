// Copyright (c), Tamas Csala

#include "example_init.hpp"

#include <oglwrap/oglwrap.h>
#include <oglwrap/shapes/rectangle_shape.h>

int main(int argc, char* argv[]) {
  GLFWwindow *window = InitializeExample();

  gl::ClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  // Defines a full screen rectangle (see oglwrap/shapes/rectangle.h)
  gl::RectangleShape rectangle;

  // Create a vertex shader
  gl::ShaderSource vs_source;
  vs_source.set_source(R"""(
    #version 330 core
    in vec2 pos;
    void main() {
      // Shrink the full screen rectangle to only half size
      gl_Position = vec4(0.5 * pos.xy, 0, 1);
    })""");
  // Give a name for the shader (will be displayed in diagnostic messages)
  vs_source.set_source_file("example_shader.vert");
  gl::Shader vs(gl::kVertexShader, vs_source);

  // Create a fragment shader
  gl::ShaderSource fs_source;
  fs_source.set_source(R"""(
    #version 330 core
    out vec4 fragColor;
    void main() {
      fragColor = vec4(0.0, 1.0, 1.0, 1.0);
    })""");
  // Give a name for the shader (will be displayed in diagnostic messages)
  fs_source.set_source_file("example_shader.frag");
  gl::Shader fs(gl::kFragmentShader, fs_source);

  // Create a shader program
  gl::Program prog(vs, fs);
  gl::Use(prog);

  while (!glfwWindowShouldClose(window)) {
    gl::Clear().Color();

    rectangle.render();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
}


