// Copyright (c), Tamas Csala

#include "oglwrap_example.hpp"

#include <lodepng.h>
#include <oglwrap/oglwrap.h>
#include <oglwrap/shapes/cube_shape.h>
#include <glm/gtc/matrix_transform.hpp>

class CylinderExample : public OglwrapExample {
private:
  // Defines a unit sized cube (see oglwrap/shapes/cube_shape.h)
  gl::CubeShape cube_shape_;

  // Vertex array for storing the cylinder geometry
  gl::VertexArray vao_;

  // Array buffer for storing the cylinder geometry
  gl::ArrayBuffer buffer_;

  // A shader program
  gl::Program prog_;

  static constexpr float kHalfHeight = 0.5f;
  static constexpr float kRadius = 0.5f;
  static constexpr int kRingsCount = 32;
  static constexpr int kSideVertices = (kRingsCount+1)*2;
  static constexpr int kVerticesPerCap = kRingsCount+2;

public:
  CylinderExample ()
    : cube_shape_({gl::CubeShape::kPosition,
                   gl::CubeShape::kNormal})
  {
    { // Define the cylinder geometry
      std::vector<glm::vec3> data;

      gl::Bind(vao_);
      gl::Bind(buffer_);

      // The side of the cylinder (to be rendered as a triangle strip)
      for (int i = 0; i <= kRingsCount; ++i) {
        float angle = i * 2*M_PI / kRingsCount;

        glm::vec3 top = {kRadius*sin(angle), kHalfHeight, kRadius*cos(angle)};
        data.push_back(top); // position
        data.push_back(top - glm::vec3{0, top.y, 0}); // normal

        glm::vec3 bottom = {kRadius*sin(angle), -kHalfHeight, kRadius*cos(angle)};
        data.push_back(bottom); // position
        data.push_back(bottom - glm::vec3{0, bottom.y, 0}); // normal
      }

      // The caps of the cylinder (to be rendered as a triangle fan)
      for (float y = -kHalfHeight; y < kHalfHeight + 1e-5; y += 2*kHalfHeight) {
        glm::vec3 center = {0, y, 0};
        glm::vec3 normal = normalize(center);
        data.push_back(center); // position
        data.push_back(normal); // normal

        for (int i = 0; i <= kRingsCount; ++i) {
          float angle = i * 2*M_PI / kRingsCount;
          data.push_back({kRadius*sin(angle), y, kRadius*cos(angle)}); // position
          data.push_back(normal); // normal
        }
      }

      gl::VertexAttrib positions(gl::CubeShape::kPosition);
      positions.pointer(3, gl::DataType::kFloat, false, 2*sizeof(glm::vec3), (void*)0);
      positions.enable();

      gl::VertexAttrib normals(gl::CubeShape::kNormal);
      normals.pointer(3, gl::DataType::kFloat, false, 2*sizeof(glm::vec3), (void*)sizeof(glm::vec3));
      normals.enable();

      buffer_.data(data);
      gl::Unbind(buffer_);
      gl::Unbind(vao_);
    }

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

      uniform vec3 color;

      out vec4 fragColor;

      void main() {
        vec3 lightPos = normalize(vec3(0.3, 1, 0.2));
        float diffuseLighting = 0.9*max(dot(lightPos, normalize(normal)), 0.0) + 0.1;
        fragColor = vec4(diffuseLighting * color, 1.0);
      })""");
    fs_source.set_source_file("example_shader.frag");
    gl::Shader fs(gl::kFragmentShader, fs_source);

    // Create a shader program
    prog_.attachShader(vs);
    prog_.attachShader(fs);
    prog_.link();
    gl::Use(prog_);

    (prog_ | "inPos").bindLocation(gl::CubeShape::kPosition);
    (prog_ | "inNormal").bindLocation(gl::CubeShape::kNormal);

    gl::Enable(gl::kDepthTest);

    // Set the clear color
    gl::ClearColor(0.1f, 0.2f, 0.3f, 1.0f);
  }

protected:
  virtual void Render() override {
    float t = glfwGetTime();
    glm::mat4 camera_mat = glm::lookAt(2.5f*glm::vec3{sin(2*t), 1.0f, cos(2*t)},
                                       glm::vec3{0.0f, 0.0f, 0.0f},
                                       glm::vec3{0.0f, 1.0f, 0.0f});
    glm::mat4 proj_mat = glm::perspectiveFov<float>(M_PI/3.0, 600, 600, 0.1, 100);

    gl::Use(prog_);

    { // Cylinder
      glm::mat4 model_mat = glm::translate(glm::mat4{1.0f}, glm::vec3{1, 0, 0});
      gl::Uniform<glm::mat4>(prog_, "mvp") = proj_mat * camera_mat * model_mat;
      gl::Uniform<glm::vec3>(prog_, "color") = glm::vec3{1.0, 0.0, 0.0};

      gl::Bind(vao_);
      gl::DrawArrays(gl::PrimType::kTriangleStrip, 0, kSideVertices);
      gl::DrawArrays(gl::PrimType::kTriangleFan, kSideVertices, kSideVertices + kVerticesPerCap);
      gl::DrawArrays(gl::PrimType::kTriangleFan, kSideVertices + kVerticesPerCap, kSideVertices + 2*kVerticesPerCap);
      gl::Unbind(vao_);
    }

    { // Cube
      glm::mat4 model_mat = glm::translate(glm::mat4{1.0f}, glm::vec3{-1, 0, 0});
      gl::Uniform<glm::mat4>(prog_, "mvp") = proj_mat * camera_mat * model_mat;
      gl::Uniform<glm::vec3>(prog_, "color") = glm::vec3{1.0, 1.0, 0.0};

      cube_shape_.render();
    }

    gl::Unuse(prog_);
  }
};

int main() {
  CylinderExample().RunMainLoop();
}

