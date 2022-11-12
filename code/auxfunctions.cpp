#include "auxfunctions.h"
#include "math.h"

Vec3 project(Vec3 v, Vec3 norm, double dist){
    return v - (norm.normalized().dot(v) + dist) * norm.normalized();
}


Vec3 project(Vec3 v, Vec3 norm, Vec3 A){
    return v - (norm.dot(v - A)) * norm;
}

double distance(Vec3 A, Vec3 B){
    return std::sqrt((A - B).dot((A-B)));
}


Vec3 abs(Vec3 A){
    return Vec3(std::abs(A.x()), std::abs(A.y()), std::abs(A.z()));
}


double sum(Vecd A){
    double aux = 0;
    for(double a : A){
        aux += a;
    }
    return aux;
}

double cot(Vec3 A, Vec3 B){
    return A.dot(B) / (A.cross(B)).norm();
}


double powerof3(double x) {
    return x*x*x;
}
double powerof2(double x) {
    return x*x;
}

double smoothingKernel(Vec3 r, double h){
    double norm = r.norm();
    if(norm < 0 || norm > h) return 0;
    // 4.921875 = 315/64
    return 4.921875 * (1.f/powerof3(powerof3(h))) * powerof3(powerof2(h) - powerof2(norm));
    //return (15/(PI * powerof3(h) * powerof3(h))) * powerof3(h - norm);
}

// CHECK DIMENSIONS //

Vec3 gradSmoothingKernel(Vec3 r, double h){
    double norm = r.norm();
    double aux = (45 / (PI * powerof3(h) * powerof3(h) * norm)) * powerof2(h - norm);
    return -aux * r;
}

double secondDerivativeSmoothingKernel(Vec3 r, double h){
    return (45/(PI*powerof3(h)*powerof3(h)))*(h - r.norm());;
}

double dot(std::vector<double> v1,  std::vector<double> v2){
    std::vector<double> v3;

    if(v1.size() != v2.size()) return 0;
    else{
        for(int i = 0; i < v1.size(); i++){
            v3.push_back(v1[i] * v2[i]);
        }
        double aux = 0;
        for(double val : v3){
            aux += val;
        }
        return aux;
    }
}


