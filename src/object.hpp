#ifndef __OBJECT_HPP_
#define __OBJECT_HPP_

#include <glm/glm.hpp>

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
// TODO: we need to store diff attributes for diff object types, I'll leave that to you

struct Object
{
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    glm::vec3 force;
    float mass;
    float size; // radius or length
    float height;
    float width;
    ObjectType type;

    Object()
    {
        position = glm::vec3(0.0f);
        velocity = glm::vec3(0.0f);
        acceleration = glm::vec3(0.0f);
        force = glm::vec3(0.0f);
        mass = 1.0f;
        size = 1.0f;
        type = ObjectType::Sphere;
    }

    Object(const glm::vec3 &pos, float m, float l, ObjectType t)
    {
        position = pos;
        velocity = glm::vec3(0.0f);
        acceleration = glm::vec3(0.0f);
        force = glm::vec3(0.0f);
        mass = m;
        size = l;
        type = t;
    }

    Object(const glm::vec3 &pos, float m, float l, float h, ObjectType t)
    {
        position = pos;
        velocity = glm::vec3(0.0f);
        acceleration = glm::vec3(0.0f);
        force = glm::vec3(0.0f);
        mass = m;
        size = l;
        height = h;
        type = t;
    }

    Object(const glm::vec3 &pos, float m, float l, float h, float w, ObjectType t)
    {
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

    void setVelocity(const glm::vec3 &vel)
    {
        this->velocity = vel;
    }

    void setAccel(const glm::vec3 &accel)
    {
        this->acceleration = accel;
    }

    void setForce(const glm::vec3 &f)
    {
        this->force = f;
    }

    void applyForce(const glm::vec3 &f)
    {
        this->force += f;
    }

    void updatePhysics(float timeStep)
    {
        acceleration = force / mass;
        velocity += acceleration * 0.9999f * timeStep;
        position += velocity * timeStep;
    }

    bool operator==(Object &obj)
    {
        return mass == obj.mass && size == obj.size && position == obj.position && velocity == obj.velocity && acceleration == obj.acceleration;
    }
};

// void load_obj(const char* filename, vector<glm::vec4> &vertices, vector<glm::vec3> &normals, vector<GLushort> &elements)
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
//            elements.push_back(a); elements.push_back(b); elements.push_back(c);
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