#include <vector>
#include "types.hpp"
#include <glm/glm.hpp>


// const float gravity = -9.81f;
// const float timeStep = 1.0f / 60.0f;

// const float velocityDampingFactor = 0.9999f;

//Rotation physics reworking is up to you!


// void applyForce(Object& obj, const glm::vec3& force) {
//     obj.force += force;
// }

// void updatePhysics(std::vector<Object>& objects, float floorY) {
//     for (auto& obj : objects) {
//         obj.force = glm::vec3(0.0f);

//         applyForce(obj, glm::vec3(0.0f, obj.mass * gravity, 0.0f));

//         obj.acceleration = obj.force / obj.mass;
//         obj.velocity += obj.acceleration * timeStep + velocityDampingFactor * timeStep;
//         obj.position += obj.velocity * timeStep;


//         //Floor plane collision
//         switch (obj.type) {
//             case ObjectType::Sphere:
//                 if (obj.position.y - obj.size < floorY) {
//                     obj.position.y = floorY + obj.size;
//                     obj.velocity.y = -obj.velocity.y * 0.8f; // Simple bounce with damping
//                 }
//                 break;
//             case ObjectType::Cube:
//                 if (obj.position.y - obj.size / 2.0f < floorY) {
//                     obj.position.y = floorY + obj.size / 2.0f;
//                     obj.velocity.y = -obj.velocity.y * 0.8f; 
//                 }
//                 break;
//             case ObjectType::Cylinder:
//                 if (obj.position.y - obj.height / 2.0f < floorY) {
//                     obj.position.y = floorY + obj.height / 2.0f;
//                     obj.velocity.y = -obj.velocity.y * 0.8f; 
//                 }
//                 break;
//             case ObjectType::Cone:
//                 if (obj.position.y - obj.height / 2.0f < floorY) {
//                     obj.position.y = floorY + obj.height / 2.0f;
//                     obj.velocity.y = -obj.velocity.y * 0.8f;
//                 }
//                 break;
//             case ObjectType::Torus:
//                 if (obj.position.y - obj.size < floorY) {
//                     obj.position.y = floorY + obj.size;
//                     obj.velocity.y = -obj.velocity.y * 0.8f; 
//                 }
//                 break;
//             case ObjectType::Cuboid:
//                 if (obj.position.y - obj.height / 2.0f < floorY) {
//                     obj.position.y = floorY + obj.height / 2.0f;
//                     obj.velocity.y = -obj.velocity.y * 0.8f; 
//                 }
//                 break;
//         }
//     }
// }