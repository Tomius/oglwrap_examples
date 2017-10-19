// Copyright (c), Tamas Csala

#ifndef EXAMPLE_INIT_HPP_
#define EXAMPLE_INIT_HPP_

#include <glad/glad.h>
#include <GLFW/glfw3.h>

GLFWwindow* InitializeExample() {
  if (!glfwInit()) {
    std::terminate();
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

  GLFWwindow* window = glfwCreateWindow(600, 600, "Example application", nullptr, nullptr);

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

  return window;
}

#endif


