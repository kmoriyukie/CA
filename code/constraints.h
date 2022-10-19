#ifndef CONSTRAINTS_H
#define CONSTRAINTS_H
#include "particlesystem.h"


class Constraints
{
public:
    Constraints(int k_ = 0) : k(k_) {};
protected:
    int k;
};

class ConstraintsCloth: public Constraints
{
    ConstraintsCloth(int k_ = 0) {k = k_;};

    void update(ParticleSystem &system, int x, int y);
};

#endif // CONSTRAINTS_H
