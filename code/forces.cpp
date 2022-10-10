#include "forces.h"


void ForceConstAcceleration::apply() {
    for (Particle* p : particles) {
        p->force += p->mass*this->getAcceleration();
    }
}

void ForceDrag::apply(){
    for(Particle *p : particles){
        p->force.array() += 6.0*PI*this->mu*p->radius*p->vel.array();//*Vec3(0,1,0).array();
    }
}
