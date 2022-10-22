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
            x0 = system.getParticle((i + 2)*y + j + 0)->pos;
            x1 = system.getParticle((i + 0)*y + j - 2)->pos;
            x2 = system.getParticle((i - 2)*y + j + 0)->pos;
            x3 = system.getParticle((i + 0)*y + j + 2)->pos;

            e0 = x1 - x0;
            e1 = x2 - x1;
            e2 = x0 - x2;
            e3 = x3 - x0;
            e4 = x3 - x1;

            c01 = 1/tan(acos(e0.dot(e1)/(e1.norm() * e0.norm())));
            c02 = 1/tan(acos(e0.dot(e2)/(e2.norm() * e0.norm())));
            c03 = 1/tan(acos(e0.dot(e3)/(e3.norm() * e0.norm())));
            c04 = 1/tan(acos(e0.dot(e4)/(e4.norm() * e0.norm())));

            K[0] =   c01 + c04;
            K[1] =   c02 + c03;
            K[2] = - c01 - c02;
            K[3] = - c03 - c04;

            x20 = x2 - x0;
            A0 = 0.5 * x20.norm() * ((x1 - x0) - (x1 - x0).dot(x20/x20.norm()) * x20/x20.norm()).norm();
            A1 = 0.5 * x20.norm() * ((x3 - x0) - (x3 - x0).dot(x20/x20.norm()) * x20/x20.norm()).norm();

            Q.push_back(1/(A1 + A0) * K * K.transpose());
        }
    }
}


void ConstraintsCloth::step(ParticleSystem &system, int x, int y){
    Vecd p = system.getPositions();

    Vecd aux;
    Particle* xi, *xj;
    std::vector<Vec3> C;
    //CHECK CONSTRAINTS FOR ALL PARTICLES CONNECTED TO CENTER PARTICLE!!

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
//    bendingConstraints(system, x, y);
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
    int idx[4];
    Vec3 x0, x1, x2, x3;
    Particle *p[4];
    Eigen::Vector4d X[4];
    Eigen::Matrix<double, 4, 4> lambda, C;

    Eigen::Matrix<double, 4, 4> a {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
    Eigen::Vector4d b (0,0,0,0), aux;
    Eigen::Vector4d dC[4];
    for(int s =2; s < x-2; s++){
        for (int t = 2; t < y-2; t++){
            idx[0] = (s + 2)*y + t + 0;
            idx[1] = (s + 0)*y + t - 2;
            idx[2] = (s - 2)*y + t + 0;
            idx[3] = (s + 0)*y + t + 2;

            p[0] = system.getParticle(idx[0]);
            p[1] = system.getParticle(idx[1]);
            p[2] = system.getParticle(idx[2]);
            p[3] = system.getParticle(idx[3]);

            x0 = p[0]->pos;
            x1 = p[1]->pos;
            x2 = p[2]->pos;
            x3 = p[3]->pos;

            X[0] = Eigen::Vector4d(x0.x(), x0.y(), x0.z(), 1);
            X[1] = Eigen::Vector4d(x1.x(), x1.y(), x1.z(), 1);
            X[2] = Eigen::Vector4d(x2.x(), x2.y(), x2.z(), 1);
            X[3] = Eigen::Vector4d(x3.x(), x3.y(), x3.z(), 1);

            C = a;
            dC[0] = b;
            dC[1] = b;
            dC[2] = b;
            dC[3] = b;

            double denom = 0;
            for(int i = 0; i < 4; i++){
                for(int j = 0; j < 4; j++){
                    C += Q[i*y + j] * X[i] * X[j].transpose();
                    dC[i] += Q[i*y + j] * X[j];
                }
            }

            for(int i = 0; i < 4; i++){
                denom += (1./p[i]->mass) * (dC[i]).norm() * (dC[i]).norm();
            }

            lambda = C / denom;

            for(int i = 0; i < 4; i++){
                aux = - lambda * (1/p[i]->mass) * dC[i];
                p[i]->pos += Vec3(aux.x(), aux.y(), aux.z());
            }

        }
    }

}
