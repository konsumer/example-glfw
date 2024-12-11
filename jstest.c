#include <GLFW/glfw3.h>
#include <stdio.h>
#include <math.h>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

// for text rendsering
stbtt_bakedchar fontCdata[96] = {};
GLuint fTex = 0;

GLFWwindow *window = NULL;

// initialize the font
void init_font(char* filename, float fontSize, stbtt_bakedchar* cdata, GLuint* ftex) {
  unsigned char ttf_buffer[1<<20];
  unsigned char temp_bitmap[512*512];
  FILE* f = fopen(filename, "rb");
  if (!f) {
    printf("Failed to open font file\n");
    return;
  }
  fread(ttf_buffer, 1, 1<<20, f);
  fclose(f);
  glGenTextures(1, ftex);
  glBindTexture(GL_TEXTURE_2D, *ftex);
  stbtt_BakeFontBitmap(ttf_buffer, 0, fontSize, temp_bitmap, 512, 512, 32, 96, cdata);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512, 512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, temp_bitmap);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

// Draw some text
void draw_text(const char* text, float x, float y, stbtt_bakedchar* cdata, GLuint* ftex) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, *ftex);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_QUADS);
    float ox = x;
    while (*text) {
      stbtt_aligned_quad q;
      stbtt_GetBakedQuad(cdata, 512, 512, *text-32, &ox, &y, &q, 1);
      glTexCoord2f(q.s0, q.t0); glVertex2f(q.x0, q.y0);
      glTexCoord2f(q.s1, q.t0); glVertex2f(q.x1, q.y0);
      glTexCoord2f(q.s1, q.t1); glVertex2f(q.x1, q.y1);
      glTexCoord2f(q.s0, q.t1); glVertex2f(q.x0, q.y1);
      ++text;
    }
    glEnd();
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}

void joystick_connect_callback(int jid, int event) {
  if (event == GLFW_CONNECTED) {
    printf("connected: %d: %s\n", jid, glfwGetJoystickName(jid));
  } else if (event == GLFW_DISCONNECTED){
    printf("disconnected: %d: %s\n", jid, glfwGetJoystickName(jid));
  }
}

// draw a equilateral (all same-length sides) polygon
void drawEquilateralPolygon(float cx, float cy, float r, int segments) {
  glBegin(GL_TRIANGLE_FAN);
  glVertex2f(cx, cy);    // Center point
  for (int i = 0; i <= segments; i++) {
    float theta = 2.0f * 3.1415926f * (float)i / (float)segments;
    float x = r * cosf(theta);
    float y = r * sinf(theta);
    glVertex2f(x + cx, y + cy);
  }
  glEnd();
}

// set the current drawing color
void setColor(unsigned int hexColor) {
    float r = ((hexColor >> 16) & 0xFF) / 255.0f;  // Red is in the highest bytes
    float g = ((hexColor >> 8) & 0xFF) / 255.0f;   // Green is in the middle
    float b = (hexColor & 0xFF) / 255.0f;          // Blue is in the lowest bytes
    glColor3f(r, g, b);
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
        draw_text(glfwGetJoystickName(jid), 4, (jid * 60) + 20, fontCdata, &fTex);

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
    return 1;
  }

  glfwSetJoystickCallback(joystick_connect_callback);

  window = glfwCreateWindow(320, 240, "gamepad tester", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return 1;
  }

  glfwMakeContextCurrent(window);
  init_font("Karla-Regular.ttf", 16.0f, fontCdata, &fTex);

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

  return 0;
}
