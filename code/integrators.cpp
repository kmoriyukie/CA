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
    Vec3 p1,v1;
    if(p0 == p_1){ // IMPORTANT
        p1 = p0 + v0*dt + (dt*dt)*a/2.0;
        v1 = v0 + a*dt;
    }
    else{
        p1 = p0 + this->kd*(p0 - p_1) + (dt*dt)*a;
        v1 = 1.0*(p1 - p0)/dt;
    }

    system.setPositions(p1);
    system.setVelocities(v1);

    system.updateForces();
}

void IntegratorCloth::step(ParticleSystem& system, double dt, int x, int y) {
    Vecd x0 = system.getState();
    Vecd v0 = system.getDerivative();
    Vecd v1 = v0 + dt*system.getSecondDerivative();
    Vecd x1 = x0 + dt*v1;

    Vec3 xi, xj;
    double l0 = 1;

    Vec3 p0, p1;
    int idx, idx2;
    for(int i = 0; i < x; i++){
        for(int j = 0; j < y; j++){

            idx2 = i * y + j;
            xi = Vec3(x1[6*idx2 + 0],
                      x1[6*idx2 + 1],
                      x1[6*idx2 + 2]);

            if(i < x -2){
                idx = (i + 2) * y + j + 0;
                relaxation(system, idx, idx2, x1, xi, 2*l0);
            }

            if(j < y -2){
                idx = (i + 0) * y + j + 2;
                relaxation(system, idx, idx2, x1, xi, 2*l0);
            }


            if(j < y -1){
                idx = i * y + j + 1;
                relaxation(system, idx, idx2, x1, xi, l0);
            }
            if(j > 0){
                idx = i * y + j - 1;
                relaxation(system, idx, idx2, x1, xi, l0);
            }

            if(i<x -1){
                idx = (i + 1) * y + j + 0;
                relaxation(system, idx, idx2, x1, xi, l0);

            }
            if(i>0){
                idx = (i - 1) * y + j + 0;
                relaxation(system, idx, idx2, x1, xi, l0);
            }



        }
    }

    system.setState(x1);
}

void IntegratorCloth::relaxation(ParticleSystem system, int idx1, int idx2,Vecd &x1, Vecd xi, int l0){
    Vec3 xj = Vec3(x1[6*idx1 + 0],
                   x1[6*idx1 + 1],
                   x1[6*idx1 + 2]);
    double d = (xj - xi).norm();
    double epsilon = 1;
    int k = 0;
    while(k < 50 && (d > (1 + epsilon) * l0 || d < (1 - epsilon) * l0)){
        d = (xj - xi).norm();
        if(system.getParticle(idx2)->is_static)
            xj +=  (d - l0) * (xj - xi).normalized();
        else if(system.getParticle(idx1)->is_static)
            xi -=  (d - l0) * (xj - xi).normalized();
        else{
            xj -= 0.5* (d - l0) * (xj - xi).normalized();
            xi += 0.5* (d - l0)* (xj - xi).normalized();
        }
        k ++;
    }


    x1[6*idx1 + 0] = xj.x();
    x1[6*idx1 + 1] = xj.y();
    x1[6*idx1 + 2] = xj.z();

    x1[6*idx2 + 0] = xi.x();
    x1[6*idx2 + 1] = xi.y();
    x1[6*idx2 + 2] = xi.z();
}
