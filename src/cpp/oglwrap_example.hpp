// Copyright (c), Tamas Csala

#ifndef OGLWRAP_EXAMPLE_HPP_
#define OGLWRAP_EXAMPLE_HPP_

#include <string>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <oglwrap/oglwrap.h>

class OglwrapExample {
public:
  OglwrapExample();
  ~OglwrapExample();

  void RunMainLoop();

protected:
  GLFWwindow* window_;
  static constexpr int kScreenWidth = 600;
  static constexpr int kScreenHeight = 600;

  virtual void Render() = 0;

  std::string GetProjectDir();
};


#endif


