#ifndef FLUIDSYSTEM_H
#define FLUIDSYSTEM_H
#include "particlesystem.h"


class FluidSystem: public ParticleSystem{
public:
    FluidSystem(){};
    virtual ~FluidSystem() {};

    void updateState();

    //Particles
    void updateNeighbors();
    void updateAccelerations();
    void clearDensity();
//    virtual Vecd getAccelerations() const;
    virtual Vecd getSecondDerivative() const;
    void calculatePressure();
    void calculateDensity();
protected:
    double h = 2;
    double standard_viscosity = 0.001;
    double standard_density = 1000; // water
    std::vector<std::vector<Particle *>> neighborlist;

};

#endif // FLUIDSYSTEM_H
