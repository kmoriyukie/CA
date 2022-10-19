#include "constraints.h"

void ConstraintsCloth::update(ParticleSystem &system, int x, int y){
    Vecd p = system.getPositions();

    Vecd aux;
    double X_i, X_j;
    //CHECK CONSTRAINTS FOR ALL PARTICLES CONNECTED TO CENTER PARTICLE!!
    for(int i = 1; i < x -1; i++){
        for(int j = 1; j <  y -1; j++){
            X_i = aux[i*x + j];
            X_j = aux[i*x + j + 1];


        }
    }
}
