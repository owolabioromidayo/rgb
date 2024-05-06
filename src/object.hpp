#ifndef __OBJECT_HPP_
#define __OBJECT_HPP_

#include <glm/glm.hpp>
#include <memory>
#include <type_traits>

enum class ObjectType
{
  Sphere,
  Cube,
  Cylinder,
  Cone,
  Torus,
  Cuboid
};

// very temp, change at will!
// TODO: we need to store diff attributes for diff object types, I'll leave that
// to you

struct ObjectArgs
{
  glm::vec3 position = glm::vec3(0.0f);
  glm::vec3 velocity = glm::vec3(0.0f);
  glm::vec3 acceleration = glm::vec3(0.0f);
  glm::vec3 force = glm::vec3(0.0f);
  float mass = 1.0f;
};

class Object
{
public:
  glm::vec3 position;
  glm::vec3 velocity;
  glm::vec3 acceleration;
  glm::vec3 force;
  float mass;
  ObjectType type;

  Object(ObjectArgs args, ObjectType type)
  {
    this->position = args.position;
    this->velocity = args.velocity;
    this->acceleration = args.acceleration;
    this->force = args.force;
    this->mass = args.mass;
    this->type = type;
  }

  void setVelocity(const glm::vec3 &vel) { this->velocity = vel; }

  void setAccel(const glm::vec3 &accel) { this->acceleration = accel; }

  void setForce(const glm::vec3 &f) { this->force = f; }

  void applyForce(const glm::vec3 &f) { this->force += f; }

  void updatePhysics(float timeStep)
  {
    acceleration = force / mass;
    velocity += acceleration * 0.9999f * timeStep;
    position += velocity * timeStep;
  }
  bool partialEq(Object &obj)
  {
    return mass == obj.mass && position == obj.position &&
           velocity == obj.velocity && acceleration == obj.acceleration;
  }

  virtual bool operator==(Object &object) { return this->partialEq(object); };

  template <typename T>
  void ptr() { static_cast<T *>(this); }
};

class Sphere : public Object
{
public:
  float radius;
  Sphere(ObjectArgs object, float radius)
      : Object(object, ObjectType::Sphere), radius(radius){};
  bool operator==(Object &obj) override
  {
    Sphere &sph = static_cast<Sphere &>(obj);
    if (sph.partialEq(sph) && sph.radius == radius)
    {
      return true;
    }
    return false;
  }
};

class Cube : public Object
{
public:
  float size;
  Cube(ObjectArgs object, float size)
      : Object(object, ObjectType::Cube), size(size){};
  bool operator==(Object &obj) override
  {
    Cube &cub = static_cast<Cube &>(obj);
    if (cub.partialEq(cub) && cub.size == size)
    {
      return true;
    }
    return false;
  }
};

class Cylinder : public Object
{
public:
  float radius;
  float height;
  Cylinder(ObjectArgs object, float radius, float height)
      : Object(object, ObjectType::Cylinder), radius(radius), height(height){};
  bool operator==(Object &obj) override
  {
    Cylinder &cyl = static_cast<Cylinder &>(obj);
    if (cyl.partialEq(cyl) && cyl.radius == radius && cyl.height == height)
    {
      return true;
    }
    return false;
  }
};

class Cone : public Object
{
public:
  float radius;
  float height;
  Cone(ObjectArgs object, float radius, float height)
      : Object(object, ObjectType::Cone), radius(radius), height(height){};

  bool operator==(Object &obj) override
  {
    Cone &con = static_cast<Cone &>(obj);
    if (con.partialEq(con) && con.radius == radius && con.height == height)
    {
      return true;
    }
    return false;
  }
};

class Torus : public Object
{
public:
  float radius;
  float thickness;
  Torus(ObjectArgs object, float radius, float thickness)
      : Object(object, ObjectType::Torus), radius(radius),
        thickness(thickness){};

  bool operator==(Object &obj) override
  {
    Torus &tor = static_cast<Torus &>(obj);
    if (tor.partialEq(tor) && tor.radius == radius &&
        tor.thickness == thickness)
    {
      return true;
    }
    return false;
  }
};

class Cuboid : public Object
{
public:
  float width;
  float height;
  float length;
  Cuboid(ObjectArgs object, float width, float height, float length)
      : Object(object, ObjectType::Cuboid), width(width), height(height),
        length(length){};
  bool operator==(Object &obj) override
  {
    Cuboid &cuboid = static_cast<Cuboid &>(obj);
    if (cuboid.partialEq(cuboid) && cuboid.width == width &&
        cuboid.height == height && cuboid.length == length)
    {
      return true;
    }
    return false;
  }
};

template <typename T, typename = std::enable_if_t<std::is_base_of_v<Object, T>>>
T *objectPtrTo(std::unique_ptr<Object> &ptr)
{
  return static_cast<T *>(ptr.get());
}

// void load_obj(const char* filename, vector<glm::vec4> &vertices,
// vector<glm::vec3> &normals, vector<GLushort> &elements)
// {
//     ifstream in(filename, ios::in);
//     if (!in)
//     {
//         std::cerr << "Cannot open " << filename << endl; exit(1);
//     }

//     string line;
//     while (getline(in, line))
//     {
//         if (line.substr(0,2) == "v ")
//         {
//             istringstream s(line.substr(2));
//             glm::vec4 v; s >> v.x; s >> v.y; s >> v.z; v.w = 1.0f;
//             vertices.push_back(v);
//         }
//         else if (line.substr(0,2) == "f ")
//         {
//             istringstream s(line.substr(2));
//             GLushort a,b,c;
//             s >> a; s >> b; s >> c;
//             a--; b--; c--;
//            elements.push_back(a); elements.push_back(b);
//            elements.push_back(c);
//         }
//         /* anything else is ignored */
//     }

//     normals.resize(vertices.size(), glm::vec3(0.0, 0.0, 0.0));
//     for (int i = 0; i < elements.size(); i+=3)
//     {
//         GLushort ia = elements[i];
//         GLushort ib = elements[i+1];
//         GLushort ic = elements[i+2];
//         glm::vec3 normal = glm::normalize(glm::cross(
//         glm::vec3(vertices[ib]) - glm::vec3(vertices[ia]),
//         glm::vec3(vertices[ic]) - glm::vec3(vertices[ia])));
//         normals[ia] = normals[ib] = normals[ic] = normal;
//     }
// }

#endif
