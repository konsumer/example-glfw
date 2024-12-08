#include <GLFW/glfw3.h>
#include <iostream>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

#define SPEED 0.005f

GLFWwindow *window = nullptr;

float getRed() {
  static float red = 0.0f;
  static bool increase = true;

  if (increase) {
    red += SPEED;
    if (red > 1.0f) {
      red = 1.0f;
      increase = false;
    }
  } else {
    red -= SPEED;
    if (red < 0.0f) {
      red = 0.0f;
      increase = true;
    }
  }

  return red;
}

void mainLoop() {
  glClearColor(getRed(), 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  glfwSwapBuffers(window);
  glfwPollEvents();
}

int main() {
  if (!glfwInit()) {
    return EXIT_FAILURE;
  }

  window = glfwCreateWindow(640, 480, "GL is cool!", nullptr, nullptr);
  if (!window) {
    glfwTerminate();
    return EXIT_FAILURE;
  }

  glfwMakeContextCurrent(window);

#ifdef EMSCRIPTEN
  emscripten_set_main_loop(&mainLoop, 0, 1);
#else
  while (!glfwWindowShouldClose(window)) {
    mainLoop();
  }
#endif

  glfwDestroyWindow(window);
  glfwTerminate();

  return EXIT_SUCCESS;
}
