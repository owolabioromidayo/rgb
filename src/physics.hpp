
#ifndef PHYSICS_HPP_
#define PHYSICS_HPP_

#include <vector>
#include <cmath>
#include <iostream>
#include "types.hpp"
#include <glm/glm.hpp>

#include "object.hpp"

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
    Gravity(float gravity);
    void updateForce(Object &obj, float timestep); 
};

class PlaneCollision : public ForceGenerator
{
protected:
    float floorY;

public:
    PlaneCollision(float floorY);

    void updateForce(Object &obj, float timestep);

    bool operator==(ForceGenerator &fg);
};

class Propulsion : public ForceGenerator
{
protected:
    float gravity;
    float k;

public:
    Propulsion(float k, float gravity);
    void updateForce(Object &obj, float timestep);
};


class PSpring: public ForceGenerator
{
protected:
    Object *other;
    float k; //spring constant
    float l; // resting length
    glm::vec3 connectionPoint;
    glm::vec3 otherConnectionPoint;

public:
    PSpring(Object *other, float k, float l);
    void updateForce(Object &obj, float timestep);
};

class PBungee: public ForceGenerator
{
protected:
    Object *other;
    float k; //spring constant
    float l; // resting length

public:
    PBungee(Object* other, float k, float l);
    void updateForce(Object &obj, float timestep);
};
class PBuoyancy: public ForceGenerator
{
protected:
    float maxDepth;
    float volume; 
    float waterHeight;
    float liquidDensity;


public:
    PBuoyancy(float maxDepth, float volume, float waterHeight, float liquidDensity);
    void updateForce(Object &obj, float timestep);
};



class PStiffSpring: public ForceGenerator
{
protected:
    float k; //spring constant
    float damping; // damping factor 
    glm::vec3 anchorPoint;

public:
    PStiffSpring(float k, float damping, glm::vec3 anchorPoint);
    void updateForce(Object &obj, float timestep);
};


#endif