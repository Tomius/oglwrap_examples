// Copyright (c), Tamas Csala

#ifndef OGLWRAP_EXAMPLE_HPP_
#define OGLWRAP_EXAMPLE_HPP_

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

  virtual void Render() = 0;
};


#endif


