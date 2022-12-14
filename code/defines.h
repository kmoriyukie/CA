#ifndef DEFINES_H
#define DEFINES_H

#include <Eigen/Core>
#include <Eigen/Geometry>

typedef Eigen::Vector2d Vec2;
typedef Eigen::Vector3d Vec3;
typedef Eigen::Vector3i Vec2i;
typedef Eigen::Vector3i Vec3i;
typedef Eigen::VectorXd Vecd;
typedef Eigen::MatrixXd Matd;

#include "Random/random.hpp"

using Random = effolkronium::random_static;

#define PI 3.14159
#define SPEED_SOUND 10


namespace Math {
    template<typename T> int signum(T val) {
        return (T(0) < val) - (val < T(0));
    }

    inline double toRad(double a) {
        return a*M_PI/180.0;
    }

    inline double toDeg(double a) {
        return a*180.0/M_PI;
    }        

    inline int ceil(double a){
        double aux = a - int(a);
        if(aux > 0.5) return int(a) + 1;
        return int(a);
    }
}


#endif // DEFINES_H
