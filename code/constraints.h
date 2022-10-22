#ifndef CONSTRAINTS_H
#define CONSTRAINTS_H
#include "particlesystem.h"


class Constraints
{
public:
    Constraints(double k_ = 0) : k(k_) {};

    void distanceConstraints(Particle *&xi, Particle *&xj);

protected:
    double k;
};

class ConstraintsCloth: public Constraints
{
public:
    ConstraintsCloth(double k_ = 0) {k = k_;};
    ConstraintsCloth(double k_, ParticleSystem system,int x, int y);

    void UpdateMesh(ParticleSystem system, int x =0, int y = 0);
    void preCalc(ParticleSystem system, int x =0, int y = 0);

    void step(ParticleSystem &system, int x, int y);
    void bendingConstraints(ParticleSystem &system,int x, int y);
protected:
    std::vector<Eigen::Matrix<double, 4, 4>> Q;
//    std::vector<Eigen::Vector4d> dC;
};

#endif // CONSTRAINTS_H
