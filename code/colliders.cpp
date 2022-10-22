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
    double f;
    Vec3 vecAB, vecAC, plane;

    for (unsigned int i = 0; i < 6; i++) { //faces
        f = faces[6*i];
        A = Vec3(vertices[3*f], vertices[3*f + 1], vertices[3*f + 2]);

        planeD[i] = -Vec3(normals[3*i], normals[3*i + 1], normals[3*i + 2]).dot(A);
    }

}


ColliderBB::ColliderBB(Vec3 scale, Vec3 translation)
{
    for (unsigned int i = 0; i < 8; i++) {
        vertices[3*i  ] = 1.1*scale.x() * originalVert[3*i  ] + translation.x();
        vertices[3*i+1] = 1.1*scale.y() * originalVert[3*i+1] + translation.y();
        vertices[3*i+2] = 1.1*scale.z() * originalVert[3*i+2] + translation.z();
    }
    calculatePlane();

}

void ColliderBB::setBB(Vec3 scale, Vec3 translation)
{
    for (unsigned int i = 0; i < 8; i++) {
        vertices[3*i  ] = 1.0*scale.x() * originalVert[3*i  ] + translation.x();
        vertices[3*i+1] = 1.0*scale.y() * originalVert[3*i+1] + translation.y();
        vertices[3*i+2] = 1.0*scale.z() * originalVert[3*i+2] + translation.z();
    }
    calculatePlane();
}


bool intersect(double a, double b, double c, double d, double x, double y, bool i){
    if((a < x && b >= x) || (b < x && a >= x))
        if(c + (x - a)/(b - a)*(d - c) < y)
            return !i;
    return i;
}



void ColliderBB::collision(Particle* p, double kElastic, double kFriction) const
{
    Vec3 norm, A, B, C, D;
    Vec3 point, point_2, f;
    double a, b, d;
    double x,y,z;
    for (uint8_t i = 0; i < 6; i++){ // test which face collided
        norm = Vec3(normals[3*i], normals[3*i + 1], normals[3*i + 2]);

        d = planeD[i];
        a = norm.dot(p->pos) + d;
        b = norm.dot(p->prevPos) + d;

        if (a*b <= 0){ // intersects plane!!
            point = project(p->pos, norm, d); // projected point
            x = point.x();
            y = point.y();
            z = point.z();

            f = Vec3(faces[6*i], faces[6*i + 1], faces[6*i + 2]);
            // Projected vertices
            A = project(Vec3(vertices[3*f.x()], vertices[3*f.x() + 1], vertices[3*f.x() + 2]), norm, d);
            B = project(Vec3(vertices[3*f.y()], vertices[3*f.y() + 1], vertices[3*f.y() + 2]), norm, d);
            C = project(Vec3(vertices[3*f.z()], vertices[3*f.z() + 1], vertices[3*f.z() + 2]), norm, d);
            D = project(Vec3(vertices[3*faces[6*i + 5]], vertices[3*faces[6*i + 5] + 1], vertices[3*faces[6*i + 5] + 2]), norm, d);

            bool intersection = false;
                if(norm.z() == 1 || norm.z() == -1){
                    if((x <= std::max({A.x(), B.x(), C.x(), D.x()}) &&
                        y <= std::max({A.y(), B.y(), C.y(), D.y()})) )
                        if((x > std::min({A.x(), B.x(), C.x(), D.x()}) &&
                            y > std::min({A.y(), B.y(), C.y(), D.y()})))
                                intersection = true;
                }
                else if(norm.x() == 1 || norm.x() == -1){
                    if((z <= std::max({A.z(), B.z(), C.z(), D.z()}) &&
                        y <= std::max({A.y(), B.y(), C.y(), D.y()})) )
                        if((z > std::min({A.z(), B.z(), C.z(), D.z()}) &&
                            y > std::min({A.y(), B.y(), C.y(), D.y()})) )
                                intersection = true;
                }
                else if(norm.y() == 1 || norm.y() == -1){
                    if((x <= std::max({A.x(), B.x(), C.x(), D.x()}) &&
                        z <= std::max({A.z(), B.z(), C.z(), D.z()})) )
                        if((x > std::min({A.x(), B.x(), C.x(), D.x()}) &&
                            z > std::min({A.z(), B.z(), C.z(), D.z()})) )
                                intersection = true;
                }

            if(intersection){
                p->pos = p->pos - (1 + kElastic)*a*norm;
                p->vel = p->vel - (1 + kElastic)*norm.dot(p->vel)*norm - kFriction*(p->vel - norm.dot(p->vel)*norm);
                return;
            }
        }
    }
}



void ColliderSphere::collision(Particle *p, double kElastic, double kFriction){
    double lambda = 1;
    double a, b, g, lambda1, lambda2;
    // Check distances
    if( distance(p->pos, center) < radius) return;
    if( (distance(p->pos, center) >= radius) && (distance(p->prevPos, center) >= radius)) return;

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
