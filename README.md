# LAB 3

## New Files (only relevant files mentioned)
- scenefluid.h
- scenefluid.cpp
- fluidsystem.cpp
- fluidsystem.h
- particle.h

# Interaction
## ALT + Right Click
Changes cube dimensions

## CTRL + Right Click
Changes fluid cube position

# Notes:
## Implementation
- Added density and pressure attributes to particle class
- Made a FluidSystem that's derived from the ParticleSysten subclass
- Used the formulas in the linked article (Particle-Based Fluid Simulation for Interactive Applications by Matthias Muller et al.) to implement the particle interactions, using the following steps:
    - Compute particle neighbors.
    - Calculate particle density.
    - Calculate particle pressure, with sound speed (SPEED_SOUND) equal to 10.
    - Calculate accelerations, using the data obtained previously.

## Ideas on how to improve
- Fix cube hitbox bug, possibly rewriting the code in its entirety,
- Reduce loops, or find another way to make code less computationally expensive.

## Issues
### Runtine Issues
The application runs really slowly if using more than 1000 particles in the fluid system.
### Cube Hitbox
There is an expected interaction between my cloth, sometimes the particles will fall off the edges of the cube. That is possibly due to an error in the logic I used for the hitbox calculation.

## Github
This project can also be found in my Github repository:
https://github.com/kmoriyukie/CA