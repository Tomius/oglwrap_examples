// Copyright (c), Tamas Csala

#include "oglwrap_example.hpp"

#include <oglwrap/oglwrap.h>
#include <oglwrap/shapes/cube_shape.h>
#include <oglwrap/shapes/sphere_shape.h>
#include <glm/gtc/matrix_transform.hpp>

class ShadowExample : public OglwrapExample {
private:
  // Defines a unit sized cube (see oglwrap/shapes/cube_shape.h)
  gl::CubeShape cube_shape_;

  // Defines a unit sized sphere (see oglwrap/shapes/sphere_shape.h)
  gl::SphereShape sphere_shape_;

  // A shader program for rendering the final objects
  gl::Program prog_;

  // A shader program for rendering the depth texture
  gl::Program shadow_prog_;

  // Texture to store depth info
  gl::Texture2D depth_tex_;

  // Framebuffer for rendering depth into depth_tex_
  gl::Framebuffer fbo_;

  // The camera and projection matrix for the shadow transformation
  glm::mat4 shadow_transform_;

  // The "position" of the directional light, directed towards the origin
  glm::vec3 light_source_pos_ = normalize(glm::vec3{0.3f, 1.0f, 0.2f});

  static constexpr int kDepthTextureResolution = 4096;

public:
  ShadowExample ()
    : cube_shape_({gl::CubeShape::kPosition,
                   gl::CubeShape::kNormal})
    , sphere_shape_({gl::SphereShape::kPosition,
                     gl::SphereShape::kNormal})
  {
    SetupDepthTexture();
    SetupFrameBuffer();
    SetupRenderProgram();
    SetupShadowProgram();
    SetupAttributePositions();
    SetupShadowTransform();
    SetupStaticUniforms();
    SetupContextParams();
  }

protected:
  virtual void Render() override {
    ShadowRender();
    FinalRender();
  }

private:
  void ShadowRender() {
    gl::Bind(fbo_);
    gl::Clear().Color().Depth();
    gl::Viewport(0, 0, kDepthTextureResolution, kDepthTextureResolution);

    gl::Use(shadow_prog_);

    { // Sphere
      glm::mat4 model_mat = glm::translate(glm::mat4{1.0f}, glm::vec3{1, 0, 0});
      gl::Uniform<glm::mat4>(shadow_prog_, "mvp") = shadow_transform_ * model_mat;

      sphere_shape_.render();
    }

    { // Cube
      glm::mat4 model_mat = glm::translate(glm::mat4{1.0f}, glm::vec3{-1, 0, 0});
      gl::Uniform<glm::mat4>(shadow_prog_, "mvp") = shadow_transform_ * model_mat;

      cube_shape_.render();
    }

     { // Floor
      glm::mat4 offset_mat = glm::translate(glm::mat4{1.0f}, glm::vec3{0, -0.505, 0});
      glm::mat4 model_mat = glm::scale(offset_mat, glm::vec3{10, 0.1, 10});
      gl::Uniform<glm::mat4>(shadow_prog_, "mvp") = shadow_transform_ * model_mat;

      cube_shape_.render();
    }

    gl::Unuse(shadow_prog_);

    gl::Unbind(fbo_);
    gl::Viewport(kScreenWidth, kScreenHeight);
  }

  void FinalRender() {
    float t = glfwGetTime();
    glm::mat4 camera_mat = glm::lookAt(2.5f*glm::vec3{sin(t), 1.0f, cos(t)},
                                       glm::vec3{0.0f, 0.0f, 0.0f},
                                       glm::vec3{0.0f, 1.0f, 0.0f});
    glm::mat4 proj_mat = glm::perspectiveFov<float>(M_PI/3.0, kScreenWidth, kScreenHeight, 0.1, 100);

    gl::Use(prog_);

    auto texture_bind_guard = gl::MakeTemporaryBind(depth_tex_);

    { // Sphere
      glm::mat4 model_mat = glm::translate(glm::mat4{1.0f}, glm::vec3{1, 0, 0});
      gl::Uniform<glm::mat4>(prog_, "model_mat") = model_mat;
      gl::Uniform<glm::mat4>(prog_, "mvp") = proj_mat * camera_mat * model_mat;
      gl::Uniform<glm::vec3>(prog_, "color") = glm::vec3{1.0, 0.5, 1.0};

      sphere_shape_.render();
    }

    { // Cube
      glm::mat4 model_mat = glm::translate(glm::mat4{1.0f}, glm::vec3{-1, 0, 0});
      gl::Uniform<glm::mat4>(prog_, "model_mat") = model_mat;
      gl::Uniform<glm::mat4>(prog_, "mvp") = proj_mat * camera_mat * model_mat;
      gl::Uniform<glm::vec3>(prog_, "color") = glm::vec3{0.1, 0.8, 0.4};

      cube_shape_.render();
    }

    { // Floor
      glm::mat4 offset_mat = glm::translate(glm::mat4{1.0f}, glm::vec3{0, -0.505, 0});
      glm::mat4 model_mat = glm::scale(offset_mat, glm::vec3{10, 0.1, 10});
      gl::Uniform<glm::mat4>(prog_, "model_mat") = model_mat;
      gl::Uniform<glm::mat4>(prog_, "mvp") = proj_mat * camera_mat * model_mat;
      gl::Uniform<glm::vec3>(prog_, "color") = glm::vec3{0.5, 0.5, 0.5};

      cube_shape_.render();
    }

    gl::Unuse(prog_);
  }

  void SetupDepthTexture() {
    gl::Bind(depth_tex_);
    depth_tex_.upload(static_cast<gl::enums::PixelDataInternalFormat>(GL_DEPTH_COMPONENT16),
                      kDepthTextureResolution, kDepthTextureResolution, gl::kDepthComponent, gl::kFloat, nullptr);
    depth_tex_.minFilter(gl::kNearest);
    depth_tex_.magFilter(gl::kLinear);
    depth_tex_.wrapS(gl::kClampToBorder);
    depth_tex_.wrapT(gl::kClampToBorder);
    depth_tex_.borderColor(glm::vec4(1.0f));
    depth_tex_.compareFunc(gl::kLequal);
    depth_tex_.compareMode(gl::kCompareRefToTexture);
    gl::Unbind(depth_tex_);
  }

  void SetupFrameBuffer() {
    gl::Bind(fbo_);
    fbo_.attachTexture(gl::kDepthAttachment, depth_tex_);
    gl::DrawBuffer(gl::kNone);
    gl::ReadBuffer(gl::kNone);
    fbo_.validate();
    gl::Unbind(fbo_);
  }

  void SetupRenderProgram() {
    gl::Shader vs(gl::kVertexShader, GetProjectDir() + "/src/glsl/05_render.vert");
    gl::Shader fs(gl::kFragmentShader, GetProjectDir() + "/src/glsl/05_render.frag");

    // Create a shader program
    prog_.attachShader(vs);
    prog_.attachShader(fs);
    prog_.link();
  }

  void SetupShadowProgram() {
    gl::Shader vs(gl::kVertexShader, GetProjectDir() + "/src/glsl/05_shadow.vert");
    gl::Shader fs(gl::kFragmentShader, GetProjectDir() + "/src/glsl/05_shadow.frag");

    // Create a shader program
    shadow_prog_.attachShader(vs);
    shadow_prog_.attachShader(fs);
    shadow_prog_.link();
  }

  void SetupAttributePositions() {
    (prog_ | "inPos").bindLocation(gl::CubeShape::kPosition);
    (prog_ | "inNormal").bindLocation(gl::CubeShape::kNormal);
    (shadow_prog_ | "inPos").bindLocation(gl::CubeShape::kPosition);
  }

  void SetupShadowTransform() {
    float shadow_volume_diameter = 10;
    glm::mat4 shadow_proj = glm::ortho<float>(-shadow_volume_diameter, shadow_volume_diameter,
                                              -shadow_volume_diameter, shadow_volume_diameter,
                                              0.0f, 2*shadow_volume_diameter);
    glm::mat4 shadow_camera = glm::lookAt(glm::vec3(0.0f) + shadow_volume_diameter*light_source_pos_,
                                          glm::vec3(0.0f),
                                          glm::vec3(0, 1, 0));
    shadow_transform_ = shadow_proj * shadow_camera;
  }

  void SetupStaticUniforms() {
    gl::Use(prog_);
    gl::Uniform<glm::vec3>(prog_, "lightPos") = light_source_pos_;
    gl::Uniform<glm::mat4>(prog_, "shadowTransform") = shadow_transform_;
    gl::Unuse(prog_);
  }

  void SetupContextParams() {
    gl::Enable(gl::kDepthTest);
    gl::ClearColor(0.1f, 0.2f, 0.3f, 1.0f);
  }
};

int main() {
  ShadowExample().RunMainLoop();
}

