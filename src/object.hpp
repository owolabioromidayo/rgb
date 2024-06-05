#ifndef __OBJECT_HPP_
#define __OBJECT_HPP_

#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>
#include <type_traits>


enum class ObjectType { Sphere, Cube, Cylinder, Cone, Torus, Cuboid };

// very temp, change at will!
// TODO: we need to store diff attributes for diff object types, I'll leave that
// to you

struct ObjectArgs {
  glm::vec3 position = glm::vec3(0.0f);
  glm::vec3 velocity = glm::vec3(0.0f);
  glm::vec3 acceleration = glm::vec3(0.0f);
  glm::vec3 force = glm::vec3(0.0f);
  float mass = 1.0f;
};

class Object {
public:
  glm::vec3 position;
  glm::vec3 velocity;
  glm::vec3 acceleration;
  glm::vec3 forceAccum;
  glm::vec3 torqueAccum;
  float mass;
  float inverseMass;
  glm::quat orientation;
  glm::vec3 rotation;
  glm::mat4 transformationMatrix;
  glm::mat3 inverseInertiaTensor;
  glm::mat3 inverseInertiaTensorWorld;
  float angularDamping;
  float linearDamping;
  bool isAwake;

  ObjectType type;

  Object(ObjectArgs args, ObjectType type) {
    this->position = args.position;
    this->velocity = args.velocity;
    this->acceleration = args.acceleration;
    this->forceAccum = args.force;
    this->mass = args.mass;
    this->type = type;

    // guards for this
    this->inverseMass = 1/ mass;
  }

  void setVelocity(const glm::vec3 &vel) { this->velocity = vel; }

  void setAccel(const glm::vec3 &accel) { this->acceleration = accel; }

  void setForce(const glm::vec3 &f) { this->forceAccum = f; }

  void applyForce(const glm::vec3 &f) { this->forceAccum += f; }

  void applyForceAtPoint(const glm::vec3 &force, const glm::vec3 &point) {
    glm::vec3 pt = point;
    pt -= position;

    forceAccum += force;
    torqueAccum += glm::cross(pt, force);

    isAwake = true;
  }

  glm::vec3 getPointInWorldSpace(const glm::vec3 &point) {
    glm::vec4 pt = glm::vec4(point.x, point.y, point.z, 1);
    pt = transformationMatrix * pt;
    glm::vec3 res = glm::vec3(pt.x, pt.y, pt.z);
  }

  void applyForceAtBodyPoint(const glm::vec3 &force, const glm::vec3 &point) {
    glm::vec3 pt = getPointInWorldSpace(point);
    applyForceAtPoint(force, pt);
    isAwake = true;
  }

  void clearAccumulators() {
    forceAccum = glm::vec3(0.0f);
    torqueAccum = glm::vec3(0.0f);
  }

  void calculateDerivedData() {
    glm::normalize(orientation);

    _calculateTransformationMatrix(transformationMatrix, position, orientation);
    _transformInertiaTensor(inverseInertiaTensorWorld, orientation,
                            inverseInertiaTensor, transformationMatrix);
  }

  void setInertiaTensor(const glm::mat3 &inertiaTensor) {
    // will this fking fail?
    this->inverseInertiaTensor = glm::inverse(inertiaTensor);
  }

  static inline void
  _calculateTransformationMatrix(glm::mat4 &transformationMatrix,
                                 const glm::vec3 &position,
                                 const glm::quat &orientation) {
    float i = orientation.x;
    float j = orientation.y;
    float k = orientation.z;
    float r = orientation.w;

    // quaternion to rotation matrix conversion + position vector in last col
    transformationMatrix[0][0] = 1 - 2 * j * j - 2 * k * k;
    transformationMatrix[0][1] = 2 * i * j - 2 * r * k;
    transformationMatrix[0][2] = 2 * i * k + 2 * r * j;
    transformationMatrix[0][3] = position.x;

    transformationMatrix[1][0] = 2 * i * j + 2 * r * k;
    transformationMatrix[1][1] = 1 - 2 * i * i - 2 * k * k;
    transformationMatrix[1][2] = 2 * j * k - 2 * r * i;
    transformationMatrix[1][3] = position.y;

    transformationMatrix[2][0] = 2 * i * k - 2 * r * j;
    transformationMatrix[2][1] = 2 * j * k + 2 * r * i;
    transformationMatrix[2][2] = 1 - 2 * i * i - 2 * j * j;
    transformationMatrix[2][3] = position.z;

    // homogeneous padding from 3x4 to 4x4
    transformationMatrix[3][0] = 0.0f;
    transformationMatrix[3][1] = 0.0f;
    transformationMatrix[3][2] = 0.0f;
    transformationMatrix[3][3] = 1.0f;
  }

  static inline void _transformInertiaTensor(glm::mat3 &iitWorld,
                                             const glm::quat &q,
                                             const glm::mat3 &iitBody,
                                             const glm::mat4 &rotmat) {

    float t4 = rotmat[0][0] * iitBody[0][0] + rotmat[0][1] * iitBody[1][1] +
               rotmat[0][2] * iitBody[2][0];

    float t9 = rotmat[0][0] * iitBody[0][1] + rotmat[0][1] * iitBody[1][1] +
               rotmat[0][2] * iitBody[2][1];
    float t14 = rotmat[0][0] * iitBody[0][2] + rotmat[0][1] * iitBody[1][2] +
                rotmat[0][2] * iitBody[2][2];
    float t28 = rotmat[1][0] * iitBody[0][0] + rotmat[1][1] * iitBody[1][0] +
                rotmat[1][2] * iitBody[2][0];
    float t33 = rotmat[1][0] * iitBody[0][1] + rotmat[1][1] * iitBody[1][1] +
                rotmat[1][2] * iitBody[2][1];
    float t38 = rotmat[1][0] * iitBody[0][2] + rotmat[1][1] * iitBody[1][2] +
                rotmat[1][2] * iitBody[2][2];
    float t52 = rotmat[2][0] * iitBody[0][0] + rotmat[2][1] * iitBody[1][0] +
                rotmat[2][2] * iitBody[2][0];
    float t57 = rotmat[2][0] * iitBody[0][1] + rotmat[2][1] * iitBody[1][1] +
                rotmat[2][2] * iitBody[2][1];
    float t62 = rotmat[2][0] * iitBody[0][2] + rotmat[2][1] * iitBody[1][2] +
                rotmat[2][2] * iitBody[2][2];

    iitWorld[0][0] = t4 * rotmat[0][0] + t9 * rotmat[0][1] + t14 * rotmat[0][2];
    iitWorld[0][1] = t4 * rotmat[1][0] + t9 * rotmat[1][1] + t14 * rotmat[1][2];
    iitWorld[0][2] = t4 * rotmat[2][0] + t9 * rotmat[2][1] + t14 * rotmat[2][2];
    iitWorld[1][0] =
        t28 * rotmat[0][0] + t33 * rotmat[0][1] + t38 * rotmat[0][2];
    iitWorld[1][1] =
        t28 * rotmat[1][0] + t33 * rotmat[1][1] + t38 * rotmat[1][2];
    iitWorld[1][2] =
        t28 * rotmat[2][0] + t33 * rotmat[2][1] + t38 * rotmat[2][2];
    iitWorld[2][0] =
        t52 * rotmat[0][0] + t57 * rotmat[0][1] + t62 * rotmat[0][2];
    iitWorld[2][1] =
        t52 * rotmat[1][0] + t57 * rotmat[1][1] + t62 * rotmat[1][2];
    iitWorld[2][2] =
        t52 * rotmat[2][0] + t57 * rotmat[2][1] + t62 * rotmat[2][2];
  }

  bool hasFiniteMass() { return 0.0f < this->mass < 100000.0f; }

  void updatePhysics(float timeStep) {
    glm::vec3 lastAccel = acceleration;
    lastAccel += (inverseMass * forceAccum );

    glm::vec3 angularAccel = inverseInertiaTensor * torqueAccum; 
    
    velocity += (lastAccel * timeStep);
    rotation += (angularAccel * timeStep);

    velocity *= pow(linearDamping, timeStep); 
    rotation *= pow(angularDamping, timeStep); 

    position += (velocity * timeStep);
    //conv rotation to a quaternion
    glm::quat q = glm::quat(0.0f, rotation) * timeStep;
    q *= orientation;
    orientation += (q * 0.5f);

    calculateDerivedData();
    clearAccumulators();
    
  }
  bool partialEq(Object &obj) {
    return mass == obj.mass && position == obj.position &&
           velocity == obj.velocity && acceleration == obj.acceleration;
  }

  virtual bool operator==(Object &object) { return this->partialEq(object); };

  template <typename T> void ptr() { static_cast<T *>(this); }
};

class Sphere : public Object {
public:
  float radius;
  Sphere(ObjectArgs object, float radius)
      : Object(object, ObjectType::Sphere), radius(radius){};
  bool operator==(Object &obj) override {
    Sphere &sph = static_cast<Sphere &>(obj);
    if (sph.partialEq(sph) && sph.radius == radius) {
      return true;
    }
    return false;
  }
};

class Cube : public Object {
public:
  float size;
  Cube(ObjectArgs object, float size)
      : Object(object, ObjectType::Cube), size(size){};
  bool operator==(Object &obj) override {
    Cube &cub = static_cast<Cube &>(obj);
    if (cub.partialEq(cub) && cub.size == size) {
      return true;
    }
    return false;
  }
};

class Cylinder : public Object {
public:
  float radius;
  float height;
  Cylinder(ObjectArgs object, float radius, float height)
      : Object(object, ObjectType::Cylinder), radius(radius), height(height){};
  bool operator==(Object &obj) override {
    Cylinder &cyl = static_cast<Cylinder &>(obj);
    if (cyl.partialEq(cyl) && cyl.radius == radius && cyl.height == height) {
      return true;
    }
    return false;
  }
};

class Cone : public Object {
public:
  float radius;
  float height;
  Cone(ObjectArgs object, float radius, float height)
      : Object(object, ObjectType::Cone), radius(radius), height(height){};

  bool operator==(Object &obj) override {
    Cone &con = static_cast<Cone &>(obj);
    if (con.partialEq(con) && con.radius == radius && con.height == height) {
      return true;
    }
    return false;
  }
};

class Torus : public Object {
public:
  float radius;
  float thickness;
  Torus(ObjectArgs object, float radius, float thickness)
      : Object(object, ObjectType::Torus), radius(radius),
        thickness(thickness){};

  bool operator==(Object &obj) override {
    Torus &tor = static_cast<Torus &>(obj);
    if (tor.partialEq(tor) && tor.radius == radius &&
        tor.thickness == thickness) {
      return true;
    }
    return false;
  }
};

class Cuboid : public Object {
public:
  float width;
  float height;
  float length;
  Cuboid(ObjectArgs object, float width, float height, float length)
      : Object(object, ObjectType::Cuboid), width(width), height(height),
        length(length){};
  bool operator==(Object &obj) override {
    Cuboid &cuboid = static_cast<Cuboid &>(obj);
    if (cuboid.partialEq(cuboid) && cuboid.width == width &&
        cuboid.height == height && cuboid.length == length) {
      return true;
    }
    return false;
  }
};

template <typename T, typename = std::enable_if_t<std::is_base_of_v<Object, T>>>
T *objectPtrTo(std::unique_ptr<Object> &ptr) {
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
