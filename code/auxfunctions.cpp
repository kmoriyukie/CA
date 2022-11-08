#include "auxfunctions.h"

Vec3 project(Vec3 v, Vec3 norm, double dist){
    return v - (norm.dot(v) + dist) * norm;
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
