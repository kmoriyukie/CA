#ifndef COLLIDERS_H
#define COLLIDERS_H

#include "defines.h"
#include "particle.h"


class Collider  // Abstract interface
{
public:
    Collider() {}
    virtual ~Collider() {}

    virtual bool testCollision(const Particle* p) const = 0;
    virtual void resolveCollision(Particle* p, double kElastic, double kFriction) const = 0;
};


class ColliderPlane : public Collider
{
public:
    ColliderPlane() { planeN = Vec3(0,0,0); planeD = 0; }
    ColliderPlane(const Vec3& n, double d) : planeN(n), planeD(d) {}
    virtual ~ColliderPlane() {}

    void setPlane(const Vec3& n, double d) { this->planeN = n; this->planeD = d; }

    virtual bool testCollision(const Particle* p) const;
    virtual void resolveCollision(Particle* p, double kElastic, double kFriction) const;

protected:
    Vec3 planeN;
    double planeD;
};

class ColliderBB : public Collider
{
public:
    ColliderBB() {
        vertices ={ -1,-1,-1,
                    -1,-1, 1,
                    -1, 1,-1,
                    -1, 1, 1,
                    1,-1,-1,
                    1,-1, 1,
                    1, 1,-1,
                    1, 1, 1};
        originalVert = { -1,-1,-1,
                         -1,-1, 1,
                         -1, 1,-1,
                         -1, 1, 1,
                         1,-1,-1,
                         1,-1, 1,
                         1, 1,-1,
                         1, 1, 1};
        normals = {-1, 0, 0,
                    1, 0, 0,
                    0,-1, 0,
                    0, 1, 0,
                    0, 0,-1,
                    0, 0, 1};
        faces = { 0, 1, 2, 2, 1, 3,
                  5, 4, 7, 7, 4, 6,
                  0, 4, 1, 1, 4, 5,
                  3, 7, 2, 2, 7, 6,
                  4, 0, 6, 6, 0, 2,
                  1, 5, 3, 3, 5, 7};
        planeD = { 0, 0, 0, 0, 0, 0};
    }
    ColliderBB(Vec3 scale, Vec3 translation);

    virtual ~ColliderBB() {}

    void setBB(Vec3 scale, Vec3 translation);
    void calculatePlane();
    void collision(Particle* p, double kElastic, double kFriction) const;

    virtual bool testCollision(const Particle* p) const {(void) p; return true;};
    virtual void resolveCollision(Particle* p, double kElastic, double kFriction) const {(void) p; (void) kElastic; (void)kFriction;};

protected:
    std::vector<double> vertices, normals, faces, planeD, originalVert;
};


class ColliderSphere : public Collider
{
public:
    ColliderSphere() { center = Vec3(0,0,0); radius = 0; }
    ColliderSphere(const Vec3& c, double r) : center(c), radius(r) {}
    virtual ~ColliderSphere() {}

    void setSphere(const Vec3& c, double r) { center = c; radius = r;}
    void collision(Particle *p, double kElastic, double kFriction);
    virtual bool testCollision(const Particle* p) const {(void) p; return true;};
    virtual void resolveCollision(Particle* p, double kElastic, double kFriction) const {(void) p; (void) kElastic; (void)kFriction;};


protected:
    Vec3 center;
    double radius;
};

class ColliderCloth : public Collider
{
public:
    ColliderCloth();
    ColliderCloth(double c, double l) : k(c), lambda(l) {}
    virtual ~ColliderCloth() {}

    virtual bool testCollision(const Particle* p) const {(void) p; return true;};
    virtual void resolveCollision(Particle* p, double kElastic, double kFriction) const {(void) p; (void) kElastic; (void)kFriction;};

protected:
    double k;
    double lambda;
};

#endif // COLLIDERS_H
