# LAB 2

## New Files (only relevant files mentioned)
- constraintsPBD.h
- constraintsPBD.cpp
- myobject.cpp
- myobject.h
- scenecloth.cpp
- scenecloth.h
- sceneclothPBD.cpp
- sceneclothPBD.h

# Interaction
## ALT + Right Click
Changes cube dimensions

## CTRL + Right Click
Changes cloth position

# Notes:
## PBD
My first attempt to simulate cloth was made using PBD, the code used for it can be found in the constraintsPBD class. Unfortunately, it did not work as it should, as sometimes it would stretch too much and behave almost like a very viscuous fluid. Because of this, and because of feedback received from the professor, I have decided to change my approach and use the method that was originally proposed. 

## Implementation
* Myobject class was added to simplify rendering of objects. Most previous scenes have been modified to use my new class.
* To render the cloth, I created the derived class Cloth from MyObject, using the code provided by the professor in the class slides. 
* Used a different shader for the cloth and the other objects.

## Issues
# Cube Hitbox
There is an expected interaction between my cloth and the hitbox of the cube. When the cloth touches the "upper right edge" or "back edge" (considering the initial view of the scene), the cloth will slip and fall off the cube.

# Couldn't make floor color appear correctly
There is probably an issue with the way I'm rendering it. It has definitely something to do with the shaders.

## Github
This project can also be found in my Github repository:
https://github.com/kmoriyukie/CA