#include "integrators.h"
#include <iostream>

void IntegratorEuler::step(ParticleSystem &system, double dt) {
    Vecd x0 = system.getState();
    Vecd dx = system.getDerivative();
    Vecd x1 = x0 + dt*dx;
    system.setState(x1);
}


void IntegratorSymplecticEuler::step(ParticleSystem &system, double dt) {
    Vecd x0 = system.getState();
    Vecd v0 = system.getDerivative();
    Vecd v1 = v0 + dt*system.getSecondDerivative();
    Vecd x1 = x0 + dt*v1;

    system.setState(x1);
}


void IntegratorMidpoint::step(ParticleSystem &system, double dt) {
    Vecd x0 = system.getState();
    Vecd a = system.getSecondDerivative();
    Vecd fmid = system.getDerivative() + (dt/2) * a;

    Vecd x1 = x0 + dt * fmid;

    system.setState(x1);
}


void IntegratorVerlet::step(ParticleSystem &system, double dt) {
    Vecd p0 = system.getPositions();
    Vecd p_1 = system.getPreviousPositions();
    Vecd v0 = system.getVelocities();
    Vecd a = system.getAccelerations();
    if(p0 == p_1){ // IMPORTANT
        Vecd p1 = p0 + v0*dt + (dt*dt)*a/2.0;
        Vecd v1 = v0 + a*dt;

        system.setPositions(p1);
        system.setVelocities(v1);
    }
    else{
        Vecd p1 = p0 + this->kd*(p0 - p_1) + (dt*dt)*a;
        Vecd v1 = 1.0*(p1 - p0)/dt;

        system.setPositions(p1);
        system.setVelocities(v1);
    }

    system.updateForces();
}
