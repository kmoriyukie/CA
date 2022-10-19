#ifndef INTEGRATORS_H
#define INTEGRATORS_H

#include "particlesystem.h"

class Integrator {
public:
    Integrator() {};
    virtual ~Integrator() {};
    virtual void step(ParticleSystem& system, double dt) = 0;
};


class IntegratorEuler : public Integrator {
public:
    virtual void step(ParticleSystem& system, double dt);
};


class IntegratorSymplecticEuler : public Integrator {
public:
    virtual void step(ParticleSystem& system, double dt);
};


class IntegratorMidpoint : public Integrator {
public:
    virtual void step(ParticleSystem& system, double dt);
};


class IntegratorVerlet : public Integrator {
public:
    virtual void step(ParticleSystem& system, double dt);
    double kd = 0.99;
};

class IntegratorCloth : public Integrator{
public:
    virtual void step(ParticleSystem& system, double dt) {(void) system; (void) dt;};
    void step(ParticleSystem& system, double dt, int x, int y);

    double kd = 0.99;

};

#endif // INTEGRATORS_H
