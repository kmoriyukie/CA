# LAB 1

## New Files (only relevant files mentioned)
- sceneBB.h
- sceneBB.cpp
- sceneSphere.h 
- sceneSphere.cpp

## Requirements
### Particle systems with 1000s of particles
All scenes contain 1000s of particles.
### Collisions: test & response (most relevant file: colliders.cpp)
    - Plane
        - ColliderPlane class. Added testCollision and resolveCollision implementation
    - Bounding box (AABB)
        - ColliderBB class. Only works with cubic bounding boxes. It is possible to scale and translate the bounding box. 
        - Relevant attributes added: scale + translation. 
        - For the collision, tested whether the particle would collide with the plane first, and then projected particle position + vertices to a plane and verified whether the particle would intersect with face.
        - Created the collision method, which tests and resolves collisions in one go.
    - Sphere
        - ColliderSphere class. Particles only bounce if inside the sphere. In the scene designed to test the sphere collision, decided to draw it using GL_TRIANGLES so it'd be easier to see what was happening inside the sphere. As with the Bounding Box, it is possible to scale and translate the sphere.
        - Relevant attributes added: radius and center.
        - Created the collision method, which tests and resolves collisions in one go.
        - For the collision, tested whether the particle is inside of the sphere, and whether it will collide. After that, calculated the plane tangent to the collision point and simply computed the plane-particle collision.
### Forces
    - Added drag force of a sphere through air. 
### Mouse interaction
    - ALT + right click + drag: 
        - AABB: scale + translate bounding box.
        - Sphere: scale sphere.
