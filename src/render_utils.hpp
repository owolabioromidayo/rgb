#ifndef __RENDER_UTILS__HPP_
#define __RENDER_UTILS__HPP_
#include <GL/glut.h>
#include <stdarg.h>
#include <sys/time.h>
#include <stdio.h>


const float tileSize = 1.0f;
float zoomFactor = 1.0f;
float fov = 45.0f;


//CORE STUFF
void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov/zoomFactor, (float)width / (float)height, 0.1, 100.0);
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

void drawText(float x, float y, void* font, const char* format, ...) {
    va_list args;
    va_start(args, format);

    char buffer[256];
    vsprintf(buffer, format, args);

    va_end(args);

    glPushMatrix();
    glLoadIdentity();
    glRasterPos2f(x, y);

    for (const char* c = buffer; *c != '\0'; ++c) {
        glutBitmapCharacter(font, *c);
    }

    glPopMatrix();
}


void drawFPS() {
    glColor3f(1.0, 1.0, 1.0); // Set text color to white
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT));
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    drawText(10, glutGet(GLUT_WINDOW_HEIGHT) - 20, GLUT_BITMAP_HELVETICA_12, "FPS: %.2f", fps);
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
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 's':
            cameraY -= 0.1;
            break;
        case 'w':
            cameraY += 0.1;
            break;
        case 'a':
            cameraX -= 0.1;
            break;
        case 'd':
            cameraX += 0.1;
            break;
        case 'q':
            cameraZ -= 0.1;
            break;
        case 'e':
            cameraZ += 0.1;
            break;
    }
    glutPostRedisplay();
}


void motion(int x, int y) {
    // Calculate mouse movement
    int deltaX = x - startX;
    int deltaY = y - startY;

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

   
    glutPostRedisplay();
}

//zooming
void mouseWheel(int button, int dir, int x, int y) {
    if (dir > 0) {
        zoomFactor *= 1.2f; 
    } else {
        zoomFactor *= 0.8f; 
    }

    zoomFactor = std::max(0.1f, std::min(zoomFactor, 10.0f)); // clamp to prevent weirdness
    reshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT)); // Update the projection matrix
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