#ifndef FORCES_H
#define FORCES_H

#include <vector>
#include "particle.h"

class Force
{
public:
    Force(void) {}
    virtual ~Force(void) {}

    virtual void apply() = 0;

    void addInfluencedParticle(Particle* p) {
        particles.push_back(p);
    }

    void setInfluencedParticles(const std::vector<Particle*>& vparticles) {
        particles = vparticles;
    }

    void clearInfluencedParticles() {
        particles.clear();
    }

    std::vector<Particle*> getInfluencedParticles() const {
        return particles;
    }

protected:
    std::vector<Particle*>	particles;
};


class ForceConstAcceleration : public Force
{
public:
    ForceConstAcceleration() { acceleration = Vec3(0,0,0); }
    ForceConstAcceleration(const Vec3& a) { acceleration = a; }
    virtual ~ForceConstAcceleration() {}

    virtual void apply();

    void setAcceleration(const Vec3& a) { acceleration = a; }
    Vec3 getAcceleration() const { return acceleration; }

protected:
    Vec3 acceleration;
};


class ForceDrag : public Force //Only works for sphere
{
public:
    ForceDrag() { mu = 0.009;}
    virtual ~ForceDrag() {}

    void setDragConstant(const double mu_1) {mu = mu_1;}

    virtual void apply();

protected:
    double mu;
};

class ForceSpring : public Force //Only works for sphere
{
public:
    ForceSpring() : ks(0), kd(0), d0(0) {};
    virtual ~ForceSpring() {}

    void set(const double ks_, const double kd_, int x_, int y_, double d0_)
    {
        ks = ks_;
        kd = kd_;
        x = x_;
        y = y_;
        d0 = d0_;
    };

    virtual void apply();

    Vec3 elasticForce(Particle *p1, Particle *p2);
    Vec3 dampingForce(Particle *p1, Particle *p2);

protected:
    double ks, kd;
    int x, y;
    double d0;
};

#endif // FORCES_H
