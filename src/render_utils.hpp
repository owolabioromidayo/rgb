#ifndef __RENDER_UTILS__HPP_
#define __RENDER_UTILS__HPP_
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <iostream>
#include <stdarg.h>
#include <stdio.h>
#include <sys/time.h>

#include <ft2build.h>
#include FT_FREETYPE_H

const float tileSize = 1.0f;
float zoomFactor = 1.0f;
float fov = 45.0f;

// CORE STUFF
// void reshape(int width, int height) {
//   glViewport(0, 0, width, height);
//   glMatrixMode(GL_PROJECTION);
//   glLoadIdentity();
//   gluPerspective(fov / zoomFactor, (float)width / (float)height, 0.1, 100.0);
// }

void reshape(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(fov / zoomFactor,
                 static_cast<float>(width) / static_cast<float>(height), 0.1,
                 100.0);
}

////////////////// FPS //////////////
struct timeval lastTime;
int frameCount = 0;
float fps = 0.0;

void updateFPS() {
  struct timeval currentTime;
  gettimeofday(&currentTime, NULL);

  double elapsed = (currentTime.tv_sec - lastTime.tv_sec) +
                   (currentTime.tv_usec - lastTime.tv_usec) / 1.0e6;

  frameCount++;

  if (elapsed > 1.0) {
    fps = frameCount / elapsed;
    // printf("FPS: %.2f\n", fps);
    // Reset frame count and last update time
    frameCount = 0;
    lastTime = currentTime;
  }
}

// void drawText(float x, float y, void *font, const char *format, ...) {
//   va_list args;
//   va_start(args, format);

//   char buffer[256];
//   vsprintf(buffer, format, args);

//   va_end(args);

//   glPushMatrix();
//   glLoadIdentity();
//   glRasterPos2f(x, y);

//   for (const char *c = buffer; *c != '\0'; ++c) {
//     glutBitmapCharacter(font, *c);
//   }

//   glPopMatrix();
// }

// TOOD: freetype not importing rn
void drawText(float x, float y, const char *text, int fontSize) {
  FT_Library ft;
  if (FT_Init_FreeType(&ft)) {
    std::cout << "ERROR::FREETYPE: Could not init FreeType Library"
              << std::endl;
    return;
  }

  FT_Face face;
  if (FT_New_Face(
          ft, "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
          0, &face)) {
    std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
    return;
  }

  FT_Set_Pixel_Sizes(face, 0, fontSize);

  glPushMatrix();
  glLoadIdentity();

  glRasterPos2f(x, y);

  for (const char *c = text; *c; ++c) {
    if (FT_Load_Char(face, *c, FT_LOAD_RENDER))
      continue;

    glBitmap(face->glyph->bitmap.width, face->glyph->bitmap.rows,
             face->glyph->bitmap_left, face->glyph->bitmap_top,
             face->glyph->advance.x / 64.0f, 0, face->glyph->bitmap.buffer);
  }

  glPopMatrix();

  FT_Done_Face(face);
  FT_Done_FreeType(ft);
}

void drawFPS(GLFWwindow *window) {
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);

  glColor3f(1.0, 1.0, 1.0); // Set text color to white
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0, width, 0, height);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  drawText(10, height - 20, "FPS: %.2f", fps);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
}

///////CAMERA STUFF , TO BE MOVED

///// INIT CAMERA VARS
float cameraX = 0.0;
float cameraY = 0.0;
float cameraZ = 0.0;

float angleX = 0.0;
float angleY = 0.0;

int startX = 0;
int startY = 0;

// Function to handle keyboard input for adjusting camera position
// void keyboard(unsigned char key, int x, int y) {
//   switch (key) {
//   case 's':
//     cameraY -= 0.1;
//     break;
//   case 'w':
//     cameraY += 0.1;
//     break;
//   case 'a':
//     cameraX -= 0.1;
//     break;
//   case 'd':
//     cameraX += 0.1;
//     break;
//   case 'q':
//     cameraZ -= 0.1;
//     break;
//   case 'e':
//     cameraZ += 0.1;
//     break;
//   }
//   // glutPostRedisplay();
// }
void keyboard(GLFWwindow *window, int key, int scancode, int action, int mods) {
  if (action == GLFW_PRESS || action == GLFW_REPEAT) {
    switch (key) {
    case GLFW_KEY_S:
      cameraY -= 0.1;
      break;
    case GLFW_KEY_W:
      cameraY += 0.1;
      break;
    case GLFW_KEY_A:
      cameraX -= 0.1;
      break;
    case GLFW_KEY_D:
      cameraX += 0.1;
      break;
    case GLFW_KEY_Q:
      cameraZ -= 0.1;
      break;
    case GLFW_KEY_E:
      cameraZ += 0.1;
      break;
    }
  }
}

void motion(GLFWwindow *window, double x, double y) {
  // Calculate mouse movement
  double deltaX = x - startX;
  double deltaY = y - startY;

  int MAX = 2000;

  if (deltaX > MAX)
    deltaX = MAX;

  else if (deltaX < -MAX)
    deltaX = -MAX;

  if (deltaY > MAX)
    deltaY = MAX;

  else if (deltaY < -MAX)
    deltaY = -MAX;

  // printf("Changes : %d %d \n", deltaX, deltaY);
  // Update rotation angles based on mouse movement
  // inversion actually works here
  angleX += deltaY * 0.1;
  angleY += deltaX * 0.1;

  // Update start position for next motion event
  startX = x;
  startY = y;

  // glutPostRedisplay();
}

// zooming
// void mouseWheel(int button, int dir, int x, int y) {
//   if (dir > 0) {
//     zoomFactor *= 1.2f;
//   } else {
//     zoomFactor *= 0.8f;
//   }

//   zoomFactor =
//       std::max(0.1f, std::min(zoomFactor, 10.0f)); // clamp to prevent
//       weirdness
//   reshape(glutGet(GLUT_WINDOW_WIDTH),
//           glutGet(GLUT_WINDOW_HEIGHT)); // Update the projection matrix
// }

void mouseWheel(GLFWwindow *window, double xoffset, double yoffset) {
  if (yoffset > 0) {
    zoomFactor *= 1.2f;
  } else {
    zoomFactor *= 0.8f;
  }

  zoomFactor =
      std::max(0.1f, std::min(zoomFactor, 10.0f)); // clamp to prevent weirdness
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  reshape(window, width, height); // Update the projection matrix
}

void renderFloorPlane(float size) {
  glPushMatrix();
  glBegin(GL_QUADS);

  int numTiles = static_cast<int>(size / tileSize);

  for (int i = -numTiles; i <= numTiles; i++) {
    for (int j = -numTiles; j <= numTiles; j++) {
      float x = i * tileSize;
      float z = j * tileSize;

      // Alternate the colors of the tiles
      if ((i + j) % 2 == 0)
        glColor3f(0.6f, 0.6f, 0.6f);
      else
        glColor3f(0.8f, 0.8f, 0.8f);

      glVertex3f(x, 0, z);
      glVertex3f(x + tileSize, 0, z);
      glVertex3f(x + tileSize, 0, z + tileSize);
      glVertex3f(x, 0, z + tileSize);
    }
  }

  glEnd();
  glPopMatrix();
}

#endif