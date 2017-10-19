// Copyright (c), Tamas Csala

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <oglwrap/oglwrap.h>
#include <oglwrap/shapes/rectangle_shape.h>

int main(int argc, char* argv[]) {
  if (!glfwInit()) {
    std::terminate();
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

  GLFWwindow *window = glfwCreateWindow(400, 400, "Example application", nullptr, nullptr);

  if (!window) {
    std::cerr << "FATAL: Couldn't create a glfw window. Aborting now." << std::endl;
    glfwTerminate();
    std::terminate();
  }

  glfwMakeContextCurrent(window);

  bool success = gladLoadGL();
  if (!success) {
    std::cerr << "gladLoadGL failed" << std::endl;
    std::terminate();
  }

  gl::Disable(gl::kDepthTest);
  gl::ClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  // Defines a full screen rectangle (see oglwrap/shapes/rectangle.h)
  gl::RectangleShape rectangle;

  // create a shader program
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

  gl::Program prog(vs, fs);
  gl::Use(prog);

  while (!glfwWindowShouldClose(window)) {
    gl::Clear().Color().Depth();

    rectangle.render();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
}


