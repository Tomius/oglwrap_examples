// Copyright (c), Tamas Csala

#include "oglwrap_example.hpp"

#include <oglwrap/oglwrap.h>
#include <oglwrap/shapes/cube_shape.h>
#include <glm/gtc/matrix_transform.hpp>

class CubeExample : public OglwrapExample {
private:
  // Defines a unit sized cube (see oglwrap/shapes/cube_shape.h)
  gl::CubeShape cube_shape_;

  // A shader program
  gl::Program prog_;

public:
  CubeExample ()
    : cube_shape_({gl::CubeShape::kPosition,
                   gl::CubeShape::kNormal})
  {
    // We need to add a few more lines to the shaders
    gl::ShaderSource vs_source;
    vs_source.set_source(R"""(
      #version 330 core
      in vec4 inPos;
      in vec3 inNormal;

      uniform mat4 mvp;

      out vec3 normal;

      void main() {
        normal = inNormal;
        gl_Position = mvp * inPos;
      })""");
    vs_source.set_source_file("example_shader.vert");
    gl::Shader vs(gl::kVertexShader, vs_source);

    gl::ShaderSource fs_source;
    fs_source.set_source(R"""(
      #version 330 core
      in vec3 normal;

      out vec4 fragColor;

      void main() {
        vec3 lightPos = normalize(vec3(0.3, 1, 0.2));
        float diffuseLighting = max(dot(lightPos, normalize(normal)), 0.0);
        vec3 color = vec3(0.1, 0.3, 0.8);
        fragColor = vec4(diffuseLighting * color, 1.0);
      })""");
    fs_source.set_source_file("example_shader.frag");
    gl::Shader fs(gl::kFragmentShader, fs_source);

    // Create a shader program
    prog_.attachShader(vs);
    prog_.attachShader(fs);
    prog_.link();
    gl::Use(prog_);

    // Bind the attribute locations
    (prog_ | "inPos").bindLocation(gl::CubeShape::kPosition);
    (prog_ | "inNormal").bindLocation(gl::CubeShape::kNormal);

    gl::Enable(gl::kDepthTest);

    // Set the clear color
    gl::ClearColor(0.1f, 0.2f, 0.3f, 1.0f);
  }

protected:
  virtual void Render() override {
    float t = glfwGetTime();
    glm::mat4 camera_mat = glm::lookAt(1.5f*glm::vec3{sin(t), 1.0f, cos(t)}, glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.0f, 1.0f, 0.0f});
    glm::mat4 proj_mat = glm::perspectiveFov<float>(M_PI/3.0, kScreenWidth, kScreenHeight, 0.1, 100);
    gl::Uniform<glm::mat4>(prog_, "mvp") = proj_mat * camera_mat;
    cube_shape_.render();
  }
};

int main() {
  CubeExample().RunMainLoop();
}

