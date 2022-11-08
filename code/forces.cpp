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


void ForceSpring::apply(){
    Particle *p0, *p1;
    for(int i = 0; i < x; i++){
        for(int j = 0; j < y; j++){
            p0 = particles[i * y + j];
            if(p0->is_static) continue;
            if(p1->is_static) continue;


            if(j != y -1){
                p1 = particles[i * y + j + 1];
                p0->force += elasticForce(p0, p1) + dampingForce(p0, p1);
                p1->force -= elasticForce(p0, p1) + dampingForce(p0, p1);
            }
            if(j != 0){
                p1 = particles[i * y + j - 1];
                p0->force += elasticForce(p0, p1) + dampingForce(p0, p1);
                p1->force -= elasticForce(p0, p1) + dampingForce(p0, p1);
            }
//            if(i!=x -1){
//                p1 = particles[(i + 1)* y + j + 0];
//                p0->force += elasticForce(p0, p1) + dampingForce(p0, p1);
//                p1->force -= elasticForce(p0, p1) + dampingForce(p0, p1);
//            }
//            if(i!=0){
//                p1 = particles[(i - 1)* y + j + 0];
//                p0->force += elasticForce(p0, p1) + dampingForce(p0, p1);
//                p1->force -= elasticForce(p0, p1) + dampingForce(p0, p1);
//            }

//            // shear

//            if(j < y- 1 && i < x - 1){
//                p1 = particles[(i + 1) * y + j + 1];
//                p0->force += elasticForce(p0, p1) + dampingForce(p0, p1);
//                p1->force -= elasticForce(p0, p1) + dampingForce(p0, p1);
//            }
//            if(i < x -2){
//                p1 = particles[(i + 2) * y + j + 0];
//                p0->force += elasticForce(p0, p1) + dampingForce(p0, p1);
//                p1->force -= elasticForce(p0, p1) + dampingForce(p0, p1);
//            }


//            if(j > 2 && j < y -2){
//                p1 = particles[(i + 0) * y + j + 2];
//                p0->force += elasticForce(p0, p1) + dampingForce(p0, p1);
//                p1->force -= elasticForce(p0, p1) + dampingForce(p0, p1);
//            }
//            if(j < y - 1 && i < x - 1){
//                p0 = particles[(i + 1) * y + j - 0];
//                p1 = particles[(i - 0) * y + j + 1];
//                p0->force += elasticForce(p0, p1) + dampingForce(p0, p1);
//                p1->force -= elasticForce(p0, p1) + dampingForce(p0, p1);
//            }


        }
    }
}


Vec3 ForceSpring::elasticForce(Particle *p1, Particle *p2){
    double d = (p1->pos - p2->pos).norm();
    Vec3 L = (p2->pos - p1->pos)/d;
    return ks * L * (d - d0);
}

Vec3 ForceSpring::dampingForce(Particle *p1, Particle *p2){
    double d = (p2->pos - p1->pos).norm();
    if (d < 1e-12) return Vec3(0,0,0);
    return kd * (p2->vel - p1->vel).array() * (p2->pos - p1->pos).array() / d;
}
