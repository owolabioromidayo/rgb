#ifndef __OBJECT_HPP_
#define __OBJECT_HPP_

#include <glm/glm.hpp>


enum class ObjectType {
    Sphere,
    Cube,
    Cylinder,
    Cone,
    Torus,
    Cuboid 
};

// very temp, change at will!
// TODO: we need to store diff attributes for diff object types, I'll leave that to you

struct Object {
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    glm::vec3 force;
    float mass;
    float size; // radius or length
    float height;
    float width;
    ObjectType type;

    Object() {
        position = glm::vec3(0.0f);
        velocity = glm::vec3(0.0f);
        acceleration = glm::vec3(0.0f);
        force = glm::vec3(0.0f);
        mass = 1.0f;
        size = 1.0f;
        type = ObjectType::Sphere;
    }

    Object(const glm::vec3& pos, float m, float l, ObjectType t ) {
        position = pos;
        velocity = glm::vec3(0.0f);
        acceleration = glm::vec3(0.0f);
        force = glm::vec3(0.0f);
        mass = m;
        size = l;
        type = t;
    }

    Object(const glm::vec3& pos, float m, float l, float h, ObjectType t) {
        position = pos;
        velocity = glm::vec3(0.0f);
        acceleration = glm::vec3(0.0f);
        force = glm::vec3(0.0f);
        mass = m;
        size = l;
        height = h;
        type = t;
    }

    Object(const glm::vec3& pos, float m, float l, float h, float w, ObjectType t) {
        position = pos;
        velocity = glm::vec3(0.0f);
        acceleration = glm::vec3(0.0f);
        force = glm::vec3(0.0f);
        mass = m;
        size = l;
        height = h;
        width = w;
        type = t;
    }

    void setVelocity(const glm::vec3& vel) {
        this->velocity = vel;
    }

    void setAccel(const glm::vec3& accel) {
        this->acceleration = accel;
    }
    
    void setForce(const glm::vec3& f) {
        this->force = f;
    }

    void applyForce(const glm::vec3& f) {
        this->force += f;
    }
};


#endif