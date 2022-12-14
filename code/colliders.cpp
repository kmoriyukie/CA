#include "colliders.h"
#include <cmath>
#include <iostream>
#include <algorithm>

#include "auxfunctions.h"

/*
 * Plane
 */

bool ColliderPlane::testCollision(const Particle* p) const
{
    double aux, aux2;
    aux = planeN.dot(p->pos) + planeD;
    aux2 = planeN.dot(p->prevPos) + planeD;
    if(aux*aux2 <= 0) return true;
    else return false;
}

void ColliderPlane::resolveCollision(Particle* p, double kElastic, double kFriction) const
{
    p->pos.array() = p->pos.array() - (1 + kElastic)*(planeN.array()*p->pos.array() + planeD).array()*planeN.array();
    p->vel.array() = p->vel.array() - (1 + kElastic)*(planeN.array()*p->vel.array())*planeN.array() - kFriction*(p->vel.array() - (planeN.array()*p->vel.array())*planeN.array());

}

/*
 * Bounding Box
 */

void ColliderBB::calculatePlane(){
    Vec3 A, B, C, D;
    double f, f2;
    Vec3 vecAB, vecAC, plane;

    for (unsigned int i = 0; i < 6; i++) { //faces
        f = faces[6*i];f2 = faces[6*i + 5];
        A = Vec3(vertices[3*f], vertices[3*f + 1], vertices[3*f + 2]);
        planeD[i] = -Vec3(normals[3*i], normals[3*i + 1], normals[3*i + 2]).dot(A);
    }

}


ColliderBB::ColliderBB(Vec3 scale, Vec3 translation)
{
    for (unsigned int i = 0; i < 8; i++) {
        vertices[3*i  ] = scale.x() * originalVert[3*i  ] + translation.x();
        vertices[3*i+1] = scale.y() * originalVert[3*i+1] + translation.y();
        vertices[3*i+2] = scale.z() * originalVert[3*i+2] + translation.z();
    }
    calculatePlane();

}

void ColliderBB::setBB(Vec3 scale, Vec3 translation)
{
    for (unsigned int i = 0; i < 8; i++) {
        vertices[3*i  ] = scale.x() * originalVert[3*i  ] + translation.x();
        vertices[3*i+1] = scale.y() * originalVert[3*i+1] + translation.y();
        vertices[3*i+2] = scale.z() * originalVert[3*i+2] + translation.z();
    }
    calculatePlane();
}


void ColliderBB::collision(Particle* p, double kElastic, double kFriction) const
{
    Vec3 norm, A, B, C, D;
    Vec3 normProj;
    Vec3 point, point_2;
    Eigen::Vector4d f;
    double a, b, d;
    double x,y,z;
    Vec3 maxBounds, minBounds;

//    Vec3 aux;
    for (uint8_t i = 0; i < 6; i++){ // test which face collided
        norm = Vec3(normals[3*i], normals[3*i + 1], normals[3*i + 2]);

        d = planeD[i];
        a = norm.dot(p->pos) + d;
        b = norm.dot(p->prevPos) + d;

        if (a*b <= 0){ // intersects plane!!
            normProj = norm;
            point = project(p->pos, normProj, d); // projected point
            x = point.x();
            y = point.y();
            z = point.z();

            f = {faces[6*i], faces[6*i + 1], faces[6*i + 2], faces[6*i + 5]};
            // Projected vertices

            A = project(Vec3(vertices[3*f.x()], vertices[3*f.x() + 1], vertices[3*f.x() + 2]), normProj, d);
            D = project(Vec3(vertices[3*f.w()], vertices[3*f.w() + 1], vertices[3*f.w() + 2]), normProj, d);

            maxBounds = Vec3(std::max({A.x(), D.x()}),
                             std::max({A.y(), D.y()}),
                             std::max({A.z(), D.z()}));

            minBounds = Vec3(std::min({A.x(), D.x()}),
                             std::min({A.y(), D.y()}),
                             std::min({A.z(), D.z()}));

            if(x > maxBounds.x() || x < minBounds.x()) continue;
            if(y > maxBounds.y() || y < minBounds.y()) continue;
            if(z > maxBounds.z() || z < minBounds.z()) continue;

            p->pos -= (1 + kElastic)*a*norm;
            p->vel -= (1 + kElastic)*norm.dot(p->vel)*norm - kFriction*(p->vel - norm.dot(p->vel)*norm);

        }
    }

}



void ColliderSphere::collision(Particle *p, double kElastic, double kFriction){
    double lambda = 1;
    double a, b, g, lambda1, lambda2;
    // Check distances
    if( (p->pos - center).norm() < radius) return;
    if( ((p->pos - center).norm() >= radius) && ((p->prevPos - center).norm() >= radius)) return;

    // Calculate segment
    a = p->vel.dot(p->vel);
    b = 2*p->vel.dot(p->pos - center);
    g = center.dot(center) + p->pos.dot(p->pos) - 2*p->pos.dot(center) - radius*radius;

    lambda1 = (-b + std::sqrt(b*b - 4*a*g) ) / 2*a;
    lambda2 = (-b - std::sqrt(b*b - 4*a*g) ) / 2*a;

    if (lambda1 >= 0 && lambda1 <= 1) lambda = lambda1;
    if (lambda2 >= 0 && lambda2 <= 1) lambda = lambda2;

    Vec3 P = p->pos + lambda*p->vel;

    // Norm equation
    Vec3 n_plane = P - center;
    n_plane /= n_plane.norm();
    double d = -n_plane.dot(p->pos); // distance of plane from origin

    // Particle-Plane collision
    p->pos = p->pos - (1 + kElastic)*(n_plane.dot(p->pos) + d)*n_plane;
    p->vel = p->vel - (1 + kElastic)*n_plane.dot(p->vel)*n_plane - kFriction*(p->vel - n_plane.dot(p->vel)*n_plane);

}
