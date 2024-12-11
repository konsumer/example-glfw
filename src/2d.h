// little 2D utils for drawing in OpenGL

#include <GLFW/glfw3.h>
#include <stdio.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

typedef struct {
  stbtt_bakedchar cdata[96];
  GLuint ftex;
} TextureFont;

// initialize a font
TextureFont* init_font(char* filename, float fontSize) {
  TextureFont* ret = malloc(sizeof(TextureFont));
  unsigned char ttf_buffer[1<<20];
  unsigned char temp_bitmap[512*512];
  FILE* f = fopen(filename, "rb");
  if (!f) {
    printf("Failed to open font file\n");
    return NULL;
  }
  fread(ttf_buffer, 1, 1<<20, f);
  fclose(f);
  glGenTextures(1, &ret->ftex);
  glBindTexture(GL_TEXTURE_2D, ret->ftex);
  stbtt_BakeFontBitmap(ttf_buffer, 0, fontSize, temp_bitmap, 512, 512, 32, 96, ret->cdata);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512, 512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, temp_bitmap);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  return ret;
}

// Draw some text
void draw_text(const char* text, float x, float y, TextureFont* font) {
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, font->ftex);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBegin(GL_QUADS);
  float ox = x;
  while (*text) {
    stbtt_aligned_quad q;
    stbtt_GetBakedQuad(font->cdata, 512, 512, *text-32, &ox, &y, &q, 1);
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
  float r = ((hexColor >> 16) & 0xFF) / 255.0f;
  float g = ((hexColor >> 8) & 0xFF) / 255.0f;
  float b = (hexColor & 0xFF) / 255.0f;
  glColor3f(r, g, b);
}
