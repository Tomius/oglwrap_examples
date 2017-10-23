// Copyright (c), Tamas Csala

#include "oglwrap_example.hpp"

#include <lodepng.h>
#include <oglwrap/oglwrap.h>
#include <oglwrap/shapes/cube_shape.h>
#include <oglwrap/shapes/sphere_shape.h>
#include <glm/gtc/matrix_transform.hpp>

class Skybox {
private:
  gl::CubeShape cube_;

  gl::Program prog_;
  gl::TextureCube texture_;
  gl::LazyUniform<glm::mat4> uProjectionMatrix_;
  gl::LazyUniform<glm::mat3> uCameraMatrix_;

public:
  Skybox(const std::string& project_dir)
      : cube_({gl::CubeShape::kPosition})
      , prog_{gl::Shader(gl::kVertexShader, project_dir + "/src/glsl/06_skybox.vert"),
              gl::Shader(gl::kFragmentShader, project_dir + "/src/glsl/06_skybox.frag")}
      , uProjectionMatrix_(prog_, "uProjectionMatrix")
      , uCameraMatrix_(prog_, "uCameraMatrix")
  {
    unsigned width, height;
    std::vector<unsigned char> data;
    std::string path = project_dir + "/src/resource/skybox.png";
    unsigned error = lodepng::decode(data, width, height, path, LCT_RGBA, 8);

    if (error) {
      std::cerr << "Image decoder error " << error << ", for image " << path << ": " << lodepng_error_text(error) << std::endl;
      throw std::runtime_error("Image decoder error");
    }

    assert(width % 4 == 0);
    assert(width / 4 == height / 3);
    unsigned size = width / 4;

    gl::Bind(texture_);
    for (int i = 0; i < 6; ++i) {
      std::vector<unsigned> subdata;
      unsigned startx, starty;
      switch (i) {
        case 0: startx = 2*size; starty = 1*size; break;
        case 1: startx = 0*size; starty = 1*size; break;
        case 2: startx = 1*size; starty = 0*size; break;
        case 3: startx = 1*size; starty = 2*size; break;
        case 4: startx = 1*size; starty = 1*size; break;
        case 5: startx = 3*size; starty = 1*size; break;
      }
      for (unsigned y = starty; y < starty + size; ++y) {
        for (unsigned x = startx; x < startx + size; ++x) {
          subdata.push_back(reinterpret_cast<unsigned*>(data.data())[y*width + x]);
        }
      }
      assert(subdata.size() == size*size);
      texture_.upload(texture_.cubeFace(i), gl::kSrgb8Alpha8, size, size,
                      gl::kRgba, gl::kUnsignedByte, subdata.data());
    }
    texture_.minFilter(gl::kLinear);
    texture_.magFilter(gl::kLinear);
    gl::Unbind(texture_);

    gl::Use(prog_);
    prog_.validate();
    gl::UniformSampler(prog_, "uTex") = 0;
    (prog_ | "aPosition").bindLocation(cube_.kPosition);
    gl::Unuse(prog_);
  }

  void Render(const glm::mat4& camera_mat, const glm::mat4& proj_mat) {
    gl::Use(prog_);

    uCameraMatrix_ = glm::mat3{camera_mat};
    uProjectionMatrix_ = proj_mat;

    gl::TemporaryDisable depth_test{gl::kDepthTest};
    gl::TemporaryEnable cubemapSeamless{gl::kTextureCubeMapSeamless};

    gl::BindToTexUnit(texture_, 0);
    gl::DepthMask(false);

    cube_.render();

    gl::DepthMask(true);
    gl::Unbind(texture_);
    gl::Unuse(prog_);
  }
};

class SkyboxExample : public OglwrapExample {
private:
  Skybox skybox;
  gl::SphereShape sphere_shape_;

  gl::Program prog_;

public:
  SkyboxExample ()
    : skybox(GetProjectDir())
    , sphere_shape_({gl::SphereShape::kPosition,
                     gl::SphereShape::kNormal})
    , prog_{gl::Shader(gl::kVertexShader, GetProjectDir() + "/src/glsl/06_cube.vert"),
            gl::Shader(gl::kFragmentShader, GetProjectDir() + "/src/glsl/06_cube.frag")}
  {
    (prog_ | "inPos").bindLocation(gl::SphereShape::kPosition);
    (prog_ | "inNormal").bindLocation(gl::SphereShape::kNormal);
  }

protected:
  virtual void Render() override {
    float t = glfwGetTime();
    glm::mat4 camera_mat = glm::lookAt(2.5f*glm::vec3{sin(0.5*t), 0.0f, cos(0.5*t)},
                                       glm::vec3{0.0f, 0.0f, 0.0f},
                                       glm::vec3{0.0f, 1.0f, 0.0f});
    glm::mat4 proj_mat = glm::perspectiveFov<float>(M_PI/3.0, kScreenWidth, kScreenHeight, 0.1, 100);

    skybox.Render(camera_mat, proj_mat);

    gl::Use(prog_);
    gl::Uniform<glm::mat4>(prog_, "mvp") = proj_mat * camera_mat;
    gl::TemporaryEnable depth_test{gl::kDepthTest};
    sphere_shape_.render();
    gl::Unuse(prog_);
  }
};

int main() {
  SkyboxExample().RunMainLoop();
}

