#include "constraints.h"
#include "defines.h"
#include "auxfunctions.h"

ConstraintsCloth::ConstraintsCloth(double k_, ParticleSystem system,int x, int y){
    k = k_;
    preCalc(system, x, y);
}


void ConstraintsCloth::UpdateMesh(ParticleSystem system, int x, int y){
    preCalc(system, x, y);
}

void ConstraintsCloth::preCalc(ParticleSystem system,int x, int y){
    Vec3 x0, x1, x2, x3;
    Eigen::Vector4d K;
    double c01, c02, c03, c04;
    Vec3 e0, e1, e2, e3, e4;
    double A0, A1;
    Vec3 x20;

    for(int i = 2; i < x-2; i++){
        for (int j = 2; j < y-2; j++){
            x0 = system.getParticle((i - 2)*y + j + 0)->pos;
            x1 = system.getParticle((i + 2)*y + j + 0)->pos;
            x2 = system.getParticle((i + 0)*y + j - 2)->pos;
            x3 = system.getParticle((i + 0)*y + j + 2)->pos;

            e0 = x1 - x0;
            e1 = x2 - x1;
            e2 = x0 - x2;
            e3 = x3 - x0;
            e4 = x1 - x3;

            c01 = cot(e0, -1 * e1);
            c02 = cot(e0, -1 * e2);
            c03 = cot(e0, e3);
            c04 = cot(e0, e4);

            K[0] =   c01 + c04;
            K[1] =   c02 + c03;
            K[2] = - c01 - c02;
            K[3] = - c03 - c04;

            A0 = 0.5 * (e0.cross(e1)).norm();
            A1 = 0.5 * (e0.cross(e3)).norm();

            Q.push_back((3.0/(A1 + A0)) * K * K.transpose());
        }
    }
}


void ConstraintsCloth::step(ParticleSystem &system, int x, int y){
    Vecd p = system.getPositions();

    Vecd aux;
    Particle* xi, *xj;
    std::vector<Vec3> C;
    //CHECK CONSTRAINTS FOR ALL PARTICLES CONNECTED TO CENTER PARTICLE!!
    bendingConstraints(system, x, y);
    for(int i = 0; i < x ; i++){
        for(int j = 0; j <  y ; j++){
            xi = system.getParticle((i + 0) * y + (j + 0));
            if(j > 0){
                xj = system.getParticle((i + 0) * y + (j - 1));
                distanceConstraints(xi, xj);
            }

            if(i < x -1){
                xj = system.getParticle((i + 1) * y + (j + 0));
                distanceConstraints(xi, xj);
            }

            if(j < y -1){
                xj = system.getParticle((i + 0) * y + (j + 1));
                distanceConstraints(xi, xj);
            }
            if(i > 0){
                xj = system.getParticle((i - 1) * y + (j - 0));
                distanceConstraints(xi, xj);
            }
        }
    }


}


void Constraints::distanceConstraints(Particle *&xi, Particle *&xj){
    Vec3 gC;
    double l = (xi->pos - xj->pos).norm();
    gC =  (xj->pos - xi->pos)*1.0/l;
    double d = 1;
    double w1, w2;
    w1 = 1./xi->mass;
    w2 = 1./xj->mass;
    double lambda = (l - d)/(w1 + w2);

//    Vec3 dxi = - w1 * lambda * gC;
    xi->pos += k *w1 * lambda * gC;
    xj->pos -= k *w2 * lambda * gC;
}




void ConstraintsCloth::bendingConstraints(ParticleSystem &system,int x, int y)
{
    Vec3 x_[4];
    Particle *p[4];
    double C, lambda;
    Eigen::Vector4d aux;
    Vec3 sum;
    Vec3 dC[4];
//    preCalc(system, x, y);
    for(int s =2; s < x-2; s++){
        for (int t = 2; t < y-2; t++){
            p[0] = system.getParticle((s - 2)*y + t - 0);
            p[1] = system.getParticle((s + 2)*y + t + 0);
            p[2] = system.getParticle((s - 0)*y + t - 2);
            p[3] = system.getParticle((s + 0)*y + t + 2);

            x_[0] = p[0]->pos;
            x_[1] = p[1]->pos;
            x_[2] = p[2]->pos;
            x_[3] = p[3]->pos;

            C = 0;
            for(int i = 0; i < 4; i++){
                sum = {0,0,0};
                for(int j = 0; j < 4; j++){
                    C   += Q[s*(y-2) + t](i,j) * (x_[i].dot(x_[j]));
                    sum += Q[s*(y-2) + t](i,j) * x_[j];
                }
                dC[i] = sum;
            }
            if(C < 1e-12) continue;
            double denom = 0;
            for(int i = 0; i < 4; i++){
                denom += (1./p[i]->mass) * (dC[i]).norm() * (dC[i]).norm();
            }

            lambda =   -0.5*C / denom;

            for(int i = 0; i < 4; i++){
                p[i]->pos +=  k * lambda * (1.0/p[i]->mass) * dC[i];
                p[i]->color = Vec3(1.0, 0, 0);
            }

        }
    }

}
