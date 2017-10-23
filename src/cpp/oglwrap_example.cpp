// Copyright (c), Tamas Csala

#include "oglwrap_example.hpp"

OglwrapExample::OglwrapExample() {
  if (!glfwInit()) {
    std::terminate();
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_RESIZABLE, false);

  window_ = glfwCreateWindow(kScreenWidth, kScreenHeight, "Example application", nullptr, nullptr);

  if (!window_) {
    std::cerr << "FATAL: Couldn't create a glfw window. Aborting now." << std::endl;
    glfwTerminate();
    std::terminate();
  }

  glfwMakeContextCurrent(window_);

  bool success = gladLoadGL();
  if (!success) {
    std::cerr << "gladLoadGL failed" << std::endl;
    std::terminate();
  }
}

OglwrapExample::~OglwrapExample() {
  glfwTerminate();
}

void OglwrapExample::RunMainLoop() {
  while (!glfwWindowShouldClose(window_)) {
    gl::Clear().Color().Depth();

    Render ();

    glfwSwapBuffers(window_);
    glfwPollEvents();
  }
}

std::string OglwrapExample::GetProjectDir() {
  std::string current_file = __FILE__;
  size_t found = current_file.find_last_of("/\\");
  assert (found != std::string::npos);
  std::string dir = current_file.substr(0, found);
  return dir + "/../..";
}
