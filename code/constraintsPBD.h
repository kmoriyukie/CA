#ifndef CONSTRAINTSPBD_H
#define CONSTRAINTSPBD_H
#include "particlesystem.h"


class ConstraintsPBD
{
public:
    ConstraintsPBD(double k_ = 0) : k(k_) {};

    void distanceConstraints(Particle *&xi, Particle *&xj);

protected:
    double k;
};

class ConstraintsClothPBD: public ConstraintsPBD
{
public:
    ConstraintsClothPBD(double k_ = 0) {k = k_;};
    ConstraintsClothPBD(double k_, ParticleSystem system,int x, int y);

    void UpdateMesh(ParticleSystem system, int x =0, int y = 0);
    void preCalc(ParticleSystem system, int x =0, int y = 0);

    void step(ParticleSystem &system, int x, int y);
    void bendingConstraints(ParticleSystem &system,int x, int y);
protected:
    std::vector<Eigen::Matrix<double, 4, 4>> Q;
};

#endif // CONSTRAINTSPBD_H
