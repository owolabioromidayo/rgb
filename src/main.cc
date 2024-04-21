#include <GL/freeglut.h>
// #include <GL/glut.h>
#include <vector>
#include <cmath>
#include <glm/glm.hpp>


#include "render_utils.hpp"
#include "types.hpp" 
#include "object.hpp" 
#include "physics.cc" 


std::vector<Object> objects;

void render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 5, 10, 0, 0, 0, 0, 1, 0);

    gluLookAt(0.0, 0.0, 3.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0); // Set up the camera position
    
    // CAMERA UPDATE
    glRotatef(angleX, 1.0, 0.0, 0.0);
    glRotatef(angleY, 0.0, 1.0, 0.0);

    // Set camera position
    glTranslatef(-cameraX, -cameraY, -cameraZ);

    renderFloorPlane(100.0f);

    //TODO: work on better obj rendering (when the type gets updated) 
    glColor3f(1.0f, 0.0f, 0.0f);
    for (const auto& obj : objects) {

        glPushMatrix(); 
        glTranslatef(obj.position.x, obj.position.y, obj.position.z);

        switch (obj.type) {
            case ObjectType::Sphere:
                glutSolidSphere(obj.size, 20, 20);
                break;
            case ObjectType::Cube:
                glutSolidCube(obj.size * 2.0f);
                break;
            case ObjectType::Cylinder:
                glPushMatrix();
                glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
                glutSolidCylinder(obj.size, obj.height, 20, 20);
                glPopMatrix();
                break;
            case ObjectType::Cone:
                glPushMatrix();
                glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
                glutSolidCone(obj.size, obj.height, 20, 20);
                glPopMatrix();
                break;
            case ObjectType::Torus:
                glutSolidTorus(obj.size * 0.5f, obj.size, 20, 20);
                break;

            case ObjectType::Cuboid:
                glScalef(obj.size, obj.height, obj.width);
                glutSolidCube(1.0f);
                break;
    }

        glPopMatrix();
    }

    updateFPS();
    drawFPS();

    glutSwapBuffers();
}

void loop(int value) {
    updatePhysics(objects, 0.0f); // floorPlane is at y0 for now
    glutPostRedisplay();
    glutTimerFunc(16, loop, 0);
}

void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    // Create some sample objects (very temporary)
    Object sphere(glm::vec3(0.0f, 5.0f, 0.0f), 1.0f, 1.5f, ObjectType::Sphere);
    Object cube(glm::vec3(10.0f, 0.0f, 0.0f), 1.0f, 2.0f, ObjectType::Cube);
    Object cylinder(glm::vec3(20.0f, 3.0f, 0.0f), 1.5f, 2.0f, 2.0f, ObjectType::Cylinder);
    Object cuboid(glm::vec3(-22.0f, 0.0f, 0.0f), 1.0f, 2.0f, 4.0f, 6.0f, ObjectType::Cube);

    // sphere.setVelocity(glm::vec3(4.0, 0.0, 3.0));
    cube.setVelocity(glm::vec3(10.0, 0.0, 0.0));
    cuboid.setVelocity(glm::vec3(0.0f, 1.0f, 0.0f)); 

    cylinder.setAccel(glm::vec3(1.0f, 0.0f, 0.0f));
    cube.setAccel(glm::vec3(-1.0f, 0.0f, 0.0f));
    cuboid.setAccel(glm::vec3(1.0f, 0.0f, 0.0f));


    objects.push_back(sphere);
    objects.push_back(cube);
    objects.push_back(cuboid);
    objects.push_back(cylinder);

}


int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(1000, 800);
    glutCreateWindow("RGB");

    // glutSetCursor(GLUT_CURSOR_INFO); // glove

    glutDisplayFunc(render);
    glutReshapeFunc(reshape);
    glutMotionFunc(motion); 
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouseWheel);

    glutTimerFunc(0, loop, 0);


    init();
    glutMainLoop();
    return 0;
}