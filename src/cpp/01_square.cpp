// Copyright (c), Tamas Csala

#include "oglwrap_example.hpp"

#include <oglwrap/shapes/rectangle_shape.h>

class SquareExample : public OglwrapExample {
private:
  // Defines a full screen rectangle (see oglwrap/shapes/rectangle_shape.h)
  gl::RectangleShape rectangle_shape_;

  // A shader program
  gl::Program prog_;

public:
  SquareExample () {
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

    // Create the shader program
    prog_.attachShader(vs);
    prog_.attachShader(fs);
    prog_.link();
    gl::Use(prog_);

    // Bind the attribute position to the location that the RectangleShape uses
    // (Both use attribute 0 by default for position, so this call isn't necessary)
    (prog_ | "pos").bindLocation(gl::RectangleShape::kPosition);

    // Set the clear color to grey
    gl::ClearColor(0.2f, 0.2f, 0.2f, 1.0f);
  }

protected:
  virtual void Render() override {
    rectangle_shape_.render();
  }
};

int main() {
  SquareExample().RunMainLoop();
}


