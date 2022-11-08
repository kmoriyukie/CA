#ifndef AUXFUNCTIONS_H
#define AUXFUNCTIONS_H

#include "defines.h"

Vec3 project(Vec3 v, Vec3 norm, double dist);
Vec3 project(Vec3 v, Vec3 norm, Vec3 A);
double distance(Vec3 A, Vec3 B);

Vec3 abs(Vec3 A);

double sum(Vecd A);
double cot(Vec3 A, Vec3 B);
#endif // AUXFUNCTIONS_H
