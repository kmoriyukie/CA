#ifndef AUXFUNCTIONS_H
#define AUXFUNCTIONS_H

#include "defines.h"

Vec3 project(Vec3 v, Vec3 norm, double dist);
Vec3 project(Vec3 v, Vec3 norm, Vec3 A);
double distance(Vec3 A, Vec3 B);

Vec3 abs(Vec3 A);

double sum(Vecd A);
double cot(Vec3 A, Vec3 B);

double powerof3(double);
double powerof2(double);

double smoothingKernel(Vec3 r, double h);
Vec3 gradSmoothingKernel(Vec3 r, double h);
double secondDerivativeSmoothingKernel(Vec3 r, double h);

double dot(std::vector<double>, std::vector<double>);
#endif // AUXFUNCTIONS_H
