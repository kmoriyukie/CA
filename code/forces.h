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

#endif // FORCES_H