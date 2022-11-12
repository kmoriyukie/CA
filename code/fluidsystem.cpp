#include "fluidsystem.h"
#include "auxfunctions.h"

void FluidSystem::updateNeighbors(){
    for(std::vector<Particle *> v: neighborlist){
        v.clear();
    }
    neighborlist.clear();
    for(Particle *p1 : particles){
        std::vector<Particle *> list;
        for(Particle *p2 : particles){
            if(p1 == p2) continue;
            if(distance(p1->pos, p2->pos) <= h){
                list.push_back(p2);
            }
        }
        neighborlist.push_back(list);
    }
}

void FluidSystem::clearDensity(){
    for(Particle *p : particles){
        p->density = 0;
    }
}

void FluidSystem::calculateDensity(){
    double kernel;
    Particle *p;
    clearDensity();
    for(int i = 0; i < particles.size(); i ++){
        p = particles[i];
        if(neighborlist[i].empty()) continue;
        for(Particle *n : neighborlist[i]){
            kernel = smoothingKernel(p->pos - n->pos, h);
            p->density += n->mass * kernel;
        }
    }
}


void FluidSystem::calculatePressure(){
    for(Particle *p: particles){
        p->pressure = SPEED_SOUND * SPEED_SOUND * (p->density - standard_density);
    }
}

Vecd FluidSystem::getSecondDerivative() const {
    Vecd deriv(this->getStateSize());
    Vec3 kernel;
    double kernel2;
    Vec3  ri, rj, vi, vj;
    double pi, pj, phoj, aux, phoi;
    for (unsigned int i = 0; i < particles.size(); i++) {
        if(neighborlist[i].empty()) {
            deriv[Particle::PhaseDimension*i + 0] =  particles[i]->force[0]/particles[i]->mass;
            deriv[Particle::PhaseDimension*i + 1] =  particles[i]->force[1]/particles[i]->mass;
            deriv[Particle::PhaseDimension*i + 2] =  particles[i]->force[2]/particles[i]->mass;
            deriv[Particle::PhaseDimension*i + 3] = 0;
            deriv[Particle::PhaseDimension*i + 4] = 0;
            deriv[Particle::PhaseDimension*i + 5] = 0;
            continue;
        }
        Vec3 ap = {0,0,0};
        Vec3 av = {0,0,0};

        pi = particles[i]->pressure;
        ri = particles[i]->pos;
        vi = particles[i]->vel;
        phoi = particles[i]->density;
        for(Particle *n : neighborlist[i]){
            rj = n->pos;
            pj = n->pressure;
            phoj = n->density;
            vj = n->vel;
            kernel = gradSmoothingKernel(ri - rj, h);
            kernel2 = secondDerivativeSmoothingKernel(ri - rj, h);

            if(phoj < 1e-10){
                ap[0] -= 0;
                ap[1] -= 0;
                ap[2] -= 0;

                av[0] += 0;
                av[1] += 0;
                av[2] += 0;

            }
            else{
                aux = n->mass * ((pi + pj)/2*phoj);// ((pi/powerof2(phoi) + pj/powerof2(phoj)));
                ap[0] += aux * kernel.x();
                ap[1] += aux * kernel.y();
                ap[2] += aux * kernel.z();

                av[0] += n->mass *  ((vj.x() - vi.x())/phoj) * kernel2;
                av[1] += n->mass *  ((vj.y() - vi.y())/phoj) * kernel2;
                av[2] += n->mass *  ((vj.z() - vi.z())/phoj) * kernel2;
            }
        }

        deriv[Particle::PhaseDimension*i + 0] =  ap.x() + standard_viscosity*av.x()/phoi + particles[i]->force[0]/particles[i]->mass;
        deriv[Particle::PhaseDimension*i + 1] =  ap.y() + standard_viscosity*av.y()/phoi + particles[i]->force[1]/particles[i]->mass;
        deriv[Particle::PhaseDimension*i + 2] =  ap.z() + standard_viscosity*av.z()/phoi + particles[i]->force[2]/particles[i]->mass;
        deriv[Particle::PhaseDimension*i + 3] = 0;
        deriv[Particle::PhaseDimension*i + 4] = 0;
        deriv[Particle::PhaseDimension*i + 5] = 0;
    }
    return deriv;
}


void FluidSystem::updateState(){
    updateNeighbors();
    calculateDensity();
    calculatePressure();
}
