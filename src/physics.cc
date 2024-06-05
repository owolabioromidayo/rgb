#include <vector>
#include <cmath>
#include <iostream>
#include "types.hpp"
#include <glm/glm.hpp>

#include "object.hpp"
#include "render_utils.hpp"
#include "types.hpp"

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
        obj.forceAccum += glm::vec3(0.0f, obj.mass * gravity, 0.0f);
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
            obj.forceAccum += glm::vec3(0.0f, 0.0f, obj.mass * 1.1f);
        }
        else
        {
            obj.forceAccum += glm::vec3(obj.mass * 1.1f, 0.0f, 0.0f);
        }
        obj.forceAccum += glm::vec3(0.0f, obj.mass * -gravity * k, 0.0f);
    }
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
    PSpring(Object *other, float k, float l)
    {
        this->k = k;
        this->l = l;
        this->other = other;
    }
    void updateForce(Object &obj, float timestep)
    {
        // glm::vec3 force = glm::vec3(obj.position.x, obj.position.y , obj.position.z);
        if(other != nullptr) {

            glm::vec3 lws = obj.getPointInWorldSpace(connectionPoint);
            glm::vec3 ows = other->getPointInWorldSpace(otherConnectionPoint);

            glm::vec3 force = lws - ows;
            float magnitude = glm::length(force); 
            magnitude = abs(magnitude - l) * k;
            force = glm::normalize(force) * -magnitude; 
            obj.applyForceAtPoint(force, lws);
        } else {
            std::cout << "Ref otehr in PSpring has been dropped \n ";
        }
    }
};

class PBungee: public ForceGenerator
{
protected:
    Object *other;
    float k; //spring constant
    float l; // resting length

public:
    PBungee(Object* other, float k, float l)
    {
        this->k = k;
        this->l = l;
        this->other = other;
    }
    void updateForce(Object &obj, float timestep)
    {
     
        // glm::vec3 force = glm::vec3(obj.position.x, obj.position.y , obj.position.z);
        if (other != nullptr){
            glm::vec3 force =  obj.position - other->position;
            float magnitude = glm::length(force); 
        
            //check bungee compressed
            if (magnitude <= l) return;

            magnitude = (l - magnitude) * k;
            force = glm::normalize(force) * -magnitude; 
            obj.applyForce(force);
        }
    }
};
class PBuoyancy: public ForceGenerator
{
protected:
    float maxDepth;
    float volume; 
    float waterHeight;
    float liquidDensity;


public:
    PBuoyancy(float maxDepth, float volume, float waterHeight, float liquidDensity)
    {
        this->maxDepth = maxDepth;
        this->volume = volume; 
        this->waterHeight = waterHeight;
        this->liquidDensity = liquidDensity;
    }
    void updateForce(Object &obj, float timestep)
    {
        float depth = obj.position.y;

        // not submerged
        if (depth >= waterHeight + maxDepth) return;
        glm::vec3 force = glm::vec3(0.0f, 0.0f, 0.0f);

        // check at min depth
        if (depth <= waterHeight - maxDepth){
            force.y = liquidDensity * volume;
            obj.applyForce(force);            
            return;
        } 

        //  otherwise partially submerged
        force.y = liquidDensity * volume * (depth - maxDepth - waterHeight) / 2*maxDepth ;
        obj.applyForce(force);            

    }
};



class PStiffSpring: public ForceGenerator
{
protected:
    float k; //spring constant
    float damping; // damping factor 
    glm::vec3 anchorPoint;

public:
    PStiffSpring(float k, float damping, glm::vec3 anchorPoint)
    {
        this->k = k;
        this->damping = damping;
        this->anchorPoint = anchorPoint;
    }
    void updateForce(Object &obj, float timestep)
    {
        if (!obj.hasFiniteMass()) return;

        glm::vec3 position = obj.position - anchorPoint;
        float gamma = 0.5f* sqrtf(4* k * damping*damping);
        if (gamma == 0.0f) return;
        glm::vec3 c = position * (damping / (2.0f * gamma) + obj.velocity * (1.0f / gamma));

        glm::vec3 target = position * cosf(gamma * timestep) + c * sinf(gamma * timestep);
        target *= expf(-0.5f * timestep * damping);

        glm::vec3 accel = (target - position) * (1.0f / timestep*timestep) - obj.velocity * timestep;
        obj.applyForce(accel * obj.mass);

    }
};

///skpping mass aggregate collision physics, will work on rigid body instead
