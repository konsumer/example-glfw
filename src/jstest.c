#include <GLFW/glfw3.h>
#include "./2d.h"

#include <math.h>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

TextureFont* font;

GLFWwindow *window = NULL;

void joystick_connect_callback(int jid, int event) {
  if (event == GLFW_CONNECTED) {
    printf("connected: %d: %s\n", jid, glfwGetJoystickName(jid));
  } else if (event == GLFW_DISCONNECTED){
    printf("disconnected: %d: %s\n", jid, glfwGetJoystickName(jid));
  }
}

void mainLoop() {
  glfwPollEvents();
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  for (int jid = GLFW_JOYSTICK_1; jid<GLFW_JOYSTICK_4;jid++) {
    if (glfwJoystickPresent(jid)) {
      int count = 0;
      const unsigned char* buttons = glfwGetJoystickButtons(jid, &count);
      for (int gamepadButton=0;gamepadButton<count;gamepadButton++) {
        setColor(0xffffff);
        draw_text(glfwGetJoystickName(jid), 4, (jid * 60) + 20, font);

        if (buttons[gamepadButton]) {
          setColor(0xff0000);
        } else {
          setColor(0xcccccc);
        }
        drawEquilateralPolygon((gamepadButton*15) + 10, (jid * 60) + 40, 6, 8);
      }
    }
  }

  glfwSwapBuffers(window);
}

int main() {
  if (!glfwInit()) {
    fprintf(stderr, "Could not init window.\n");
    return 1;
  }

  glfwSetJoystickCallback(joystick_connect_callback);

  window = glfwCreateWindow(320, 240, "gamepad tester", NULL, NULL);
  if (!window) {
    fprintf(stderr, "Could not create window.\n");
    glfwTerminate();
    return 1;
  }
  glfwMakeContextCurrent(window);

  font = init_font("assets/Karla-Regular.ttf", 16.0f);
  if (!font) {
    fprintf(stderr, "Could not load font.\n");
    return 1;
  }

  // Enable blending
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // 2D mode
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, 320, 240, 0, -1, 1);
  glMatrixMode(GL_MODELVIEW);

#ifdef EMSCRIPTEN
  emscripten_set_main_loop(&mainLoop, 0, 1);
#else
  while (!glfwWindowShouldClose(window)) {
    mainLoop();
  }
#endif

  glfwDestroyWindow(window);
  glfwTerminate();

  free(font);

  return 0;
}
