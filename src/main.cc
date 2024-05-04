#include <GL/freeglut.h>
// #include <GL/glut.h>
#include <algorithm>
#include <cmath>
#include <glm/glm.hpp>
#include <iostream>
#include <memory>
#include <vector>

#include "environment.hpp"
#include "object.hpp"
#include "physics.cc"
#include "render_utils.hpp"
#include "types.hpp"

////////////// FORCE GENERATOR

class ForceGenerator
{
public:
  virtual void updateForce(Object &object, float timestep) = 0;

  bool operator==(ForceGenerator &fg) { return true; };

  bool operator==(const ForceGenerator &fg) { return true; };
};

class Gravity : public ForceGenerator
{
protected:
  float gravity; // gravity constant

public:
  // TODO: brush up on move semantics
  Gravity(float gravity) { this->gravity = gravity; }

  void updateForce(Object &obj, float timestep)
  {
    obj.force += glm::vec3(0.0f, obj.mass * gravity, 0.0f);
  }

  // bool operator==(ForceGenerator &fg)
  // {
  //     return true; // this part is not important
  // };
};

// TODO: ADD DAMPING FORCE (or just update vel )

// can be reworked into general plane collision, floor for now
class PlaneCollision : public ForceGenerator
{
protected:
  float floorY;

public:
  // TODO: brush up on move semantics
  PlaneCollision(float floorY) { this->floorY = floorY; }

  void updateForce(Object &obj, float timestep)
  {

    // Floor plane collision
    switch (obj.type)
    {
    case ObjectType::Sphere:
    {
      Sphere &sph = static_cast<Sphere &>(obj);
      if (sph.position.y - sph.radius < floorY)
      {
        sph.position.y = floorY + sph.radius;
        sph.velocity.y = -sph.velocity.y * 0.8f; // Simple bounce with damping
      }
    }
    break;
    case ObjectType::Cube:
    {

      Cube &cub = static_cast<Cube &>(obj);
      if (cub.position.y - cub.size / 2.0f < floorY)
      {
        cub.position.y = floorY + cub.size / 2.0f;
        cub.velocity.y = -cub.velocity.y * 0.8f;
      }
    }
    break;
    case ObjectType::Cylinder:
    {

      Cylinder &cyl = static_cast<Cylinder &>(obj);
      if (cyl.position.y - cyl.height / 2.0f < floorY)
      {
        cyl.position.y = floorY + cyl.height / 2.0f;
        cyl.velocity.y = -cyl.velocity.y * 0.8f;
      }
    }
    break;
    case ObjectType::Cone:
    {

      Cone &con = static_cast<Cone &>(obj);
      if (con.position.y - con.height / 2.0f < floorY)
      {
        con.position.y = floorY + con.height / 2.0f;
        con.velocity.y = -con.velocity.y * 0.8f;
      }
    }
    break;
    case ObjectType::Torus:
    {

      Torus &tor = static_cast<Torus &>(obj);
      if (tor.position.y - tor.radius < floorY)
      {
        tor.position.y = floorY + tor.radius;
        tor.velocity.y = -tor.velocity.y * 0.8f;
      }
    }
    break;
    case ObjectType::Cuboid:
    {

      Cuboid &cuboid = static_cast<Cuboid &>(obj);
      if (cuboid.position.y - cuboid.height / 2.0f < floorY)
      {
        cuboid.position.y = floorY + cuboid.height / 2.0f;
        obj.velocity.y = -obj.velocity.y * 0.8f;
      }
    }
    break;
    }
  }

  bool operator==(ForceGenerator &fg)
  {
    return true; // this part is not important
  };
};

class Propulsion : public ForceGenerator
{
protected:
  float gravity;
  float k;

public:
  Propulsion(float k, float gravity)
  {
    this->k = k;
    this->gravity = gravity;
  }
  void updateForce(Object &obj, float timestep)
  {

    //  what would the force be?
    // neglect gravity
    // we dont have shifting mass so we cant actually do it
    // we can try to make it inclined though
    if (obj.velocity.y < 0)
    {
      obj.force += glm::vec3(0.0f, 0.0f, obj.mass * 1.1f);
    }
    else
    {
      obj.force += glm::vec3(obj.mass * 1.1f, 0.0f, 0.0f);
    }
    obj.force += glm::vec3(0.0f, obj.mass * -gravity * k, 0.0f);
  }
};

struct ForceRegistration
{
  Object &object;
  ForceGenerator &fg;

  bool operator==(ForceRegistration &fr)
  {
    return object == fr.object && fg == fr.fg;
  }

  bool operator==(const ForceRegistration &fr)
  {
    return object == fr.object && fg == fr.fg;
  }
};

typedef std::vector<ForceRegistration> Registry;

class ForceRegistry
{
protected:
  Registry registrations;

public:
  ForceRegistry()
  {
    // nothing
  }
  void add(Object &object, ForceGenerator &fg)
  {
    registrations.push_back(ForceRegistration{object, fg});
  }

  // TODO : fix static error asser problem on .erase(it)
  //  void remove(Object &object, ForceGenerator &fg)
  //  {
  //      ForceRegistration f = ForceRegistration{object, fg};
  //      auto it = std::find(registrations.begin(), registrations.end(), f);
  //      if (it != registrations.end())
  //          registrations.erase(it);
  //      else
  //          std::cerr << "Could not remove object \n";
  //  }

  void clear(Object &object, ForceGenerator &fg) { registrations.clear(); }

  void updateForces(float duration)
  {

    // reset forces
    for (auto &k : registrations)
      k.object.force = glm::vec3(0.0f);

    for (auto &k : registrations)
      k.fg.updateForce(k.object, duration);
  }
};

////////////

const float timeStep = 1.0f / 60.0f;
Environment env;
ForceRegistry fr;

auto grav = Gravity(-9.81f);
auto fplane_force = PlaneCollision(0.0f);
auto prop = Propulsion(1.03f, -9.81f);

void render()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  // gluLookAt(0, 5, 10, 0, 0, 0, 0, 1, 0);

  gluLookAt(0.0, 0.0, 6.0, 0.0, 0.0, 0.0, 0.0, 3.0,
            0.0); // Set up the camera position

  // CAMERA UPDATE
  glRotatef(angleX, 1.0, 0.0, 0.0);
  glRotatef(angleY, 0.0, 1.0, 0.0);

  // Set camera position
  glTranslatef(-cameraX, -cameraY, -cameraZ);

  renderFloorPlane(100.0f);

  // TODO: work on better obj rendering (when the type gets updated)
  glColor3f(1.0f, 0.0f, 0.0f);
  for (auto &obj : env.objects)
  {

    glPushMatrix();
    glTranslatef(obj->position.x, obj->position.y, obj->position.z);

    switch (obj->type)
    {
    case ObjectType::Sphere:
    {
      Sphere *sph = objectPtrTo<Sphere>(obj);
      glutSolidSphere(sph->radius, 20, 20);
    }
    break;
    case ObjectType::Cube:
    {
      Cube *cub = objectPtrTo<Cube>(obj);
      glutSolidCube(cub->size * 2.0f);
    }
    break;
    case ObjectType::Cylinder:
    {
      Cylinder *cyl = objectPtrTo<Cylinder>(obj);
      glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
      glutSolidCylinder(cyl->radius, cyl->height, 20, 20);
    }
    break;
    case ObjectType::Cone:
    {
      Cone *con = objectPtrTo<Cone>(obj);
      glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
      glutSolidCone(con->radius, con->height, 20, 20);
    }
    break;
    case ObjectType::Torus:
    {
      Torus *tor = objectPtrTo<Torus>(obj);
      glutSolidTorus(tor->radius * 0.5f, tor->thickness, 20, 20);
    }
    break;

    case ObjectType::Cuboid:
    {

      Cuboid *cuboid = objectPtrTo<Cuboid>(obj);
      glScalef(cuboid->length, cuboid->height, cuboid->width);
      glutSolidCube(1.0f);
    }
    break;
    }

    glPopMatrix();
  }

  updateFPS();
  drawFPS();

  glutSwapBuffers();
}

void loop(int value)
{

  // updatePhysics(objects, 0.0f); // floorPlane is at y0 for now
  fr.updateForces(timeStep);

  for (auto &obj : env.objects)
  {
    std::cout << obj->force.x << " " << obj->force.y << " " << obj->force.z
              << std::endl;
    obj->updatePhysics(timeStep);
  }
  glutPostRedisplay();
  glutTimerFunc(16, loop, 0);
}

void init()
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  // Create some sample objects (very temporary)
  Sphere sphere({.position = glm::vec3(0.0f, 5.0f, 0.0f)}, 1.5f);
  Cube cube({.position = glm::vec3(10.0f, 0.0f, 0.0f)}, 2.0f);
  Cylinder cylinder({.position = glm::vec3(20.0f, 3.0f, 0.0f)}, 1.5f, 2.0f);
  Cuboid cuboid({.position = glm::vec3(-22.0f, 0.0f, 0.0f)}, 2.0, 4.0f, 6.0f);

  // sphere.setVelocity(glm::vec3(4.0, 0.0, 3.0));
  // cube.setVelocity(glm::vec3(10.0, 0.0, 0.0));
  // cuboid.setVelocity(glm::vec3(0.0f, 1.0f, 0.0f));

  // cylinder.setAccel(glm::vec3(0.0f, 0.0f, 0.0f));
  // cube.setAccel(glm::vec3(0.0f, 0.0f, 0.0f));
  // cuboid.setAccel(glm::vec3(0.0f, 0.0f, 0.0f));
  env.addObject<Sphere>(sphere);
  env.addObject<Cube>(cube);
  env.addObject<Cylinder>(cylinder);
  env.addObject<Cuboid>(cuboid);
  // env.objects.push_back(std::make_unique<Sphere>(sphere));
  // env.objects.push_back(std::make_unique<Cube>(cube));
  // objects.push_back(cube);
  // objects.push_back(cuboid);
  // objects.push_back(cylinder);

  for (auto &obj : env.objects)
  {
    fr.add(*obj, grav);
    fr.add(*obj, fplane_force);
  }
  fr.add(*(env.objects.at(0)), prop);
}

int main(int argc, char **argv)
{
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
