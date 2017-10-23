// Copyright (c), Tamas Csala

#include "oglwrap_example.hpp"

#include <lodepng.h>
#include <oglwrap/oglwrap.h>
#include <oglwrap/shapes/rectangle_shape.h>

class TexturedSquareExample : public OglwrapExample {
private:
  // Defines a full screen rectangle (see oglwrap/shapes/rectangle_shape.h)
  gl::RectangleShape rectangle_shape_;

  // A shader program
  gl::Program prog_;

  // A 2D texture
  gl::Texture2D tex_;

public:
  TexturedSquareExample ()
    // Now we need texture coordinates too, not just position
    : rectangle_shape_({gl::RectangleShape::kPosition,
                        gl::RectangleShape::kTexCoord})
  {
    // We need to add a few more lines to the shaders
    gl::ShaderSource vs_source;
    vs_source.set_source(R"""(
      #version 330 core
      in vec2 pos;
      in vec2 inTexCoord;

      out vec2 texCoord;

      void main() {
        texCoord = inTexCoord;

        // Shrink the full screen rectangle to a smaller size
        gl_Position = vec4(0.3 * pos.x, 0.15 * pos.y, 0, 1);
      })""");
    vs_source.set_source_file("example_shader.vert");
    gl::Shader vs(gl::kVertexShader, vs_source);

    gl::ShaderSource fs_source;
    fs_source.set_source(R"""(
      #version 330 core
      in vec2 texCoord;
      uniform sampler2D tex;

      out vec4 fragColor;

      void main() {
        fragColor = texture(tex, vec2(texCoord.x, 1-texCoord.y));
      })""");
    fs_source.set_source_file("example_shader.frag");
    gl::Shader fs(gl::kFragmentShader, fs_source);

    // Create a shader program
    prog_.attachShader(vs);
    prog_.attachShader(fs);
    prog_.link();
    gl::Use(prog_);

    // Bind the attribute positions to the locations that the RectangleShape uses
    (prog_ | "pos").bindLocation(gl::RectangleShape::kPosition);
    (prog_ | "inTexCoord").bindLocation(gl::RectangleShape::kTexCoord);

    // Set the texture uniform
    gl::UniformSampler(prog_, "tex") = 0;

    // Load and setup a texture
    {
      gl::Bind(tex_);
      unsigned width, height;
      std::vector<unsigned char> data;
      std::string path = GetProjectDir() + "/deps/oglwrap/logo.png";
      unsigned error = lodepng::decode(data, width, height, path, LCT_RGBA, 8);
      if (error) {
        std::cerr << "Image decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
        std::terminate();
      }
      tex_.upload(gl::kSrgb8Alpha8, width, height,
          gl::kRgba, gl::kUnsignedByte, data.data());
      tex_.minFilter(gl::kLinear);
      tex_.magFilter(gl::kLinear);
    }

    // Enable alpha blending
    gl::Enable(gl::kBlend);
    gl::BlendFunc(gl::kSrcAlpha, gl::kOneMinusSrcAlpha);

    // Set the clear color to white
    gl::ClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  }

protected:
  virtual void Render() override {
    rectangle_shape_.render();
  }
};

int main() {
  TexturedSquareExample().RunMainLoop();
}

