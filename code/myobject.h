#ifndef MYOBJECT_H
#define MYOBJECT_H

#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLBuffer>
#include "defines.h"
#include "particlesystem.h"

class myObject
{
public:
    myObject() {};
    virtual ~myObject() {};

    virtual void draw(QOpenGLFunctions_3_3_Core* &glFuncs, Vec3 size) = 0;

    QOpenGLVertexArrayObject *vao = nullptr;
    QOpenGLShaderProgram *shader = nullptr;

};

class Cube : public myObject{
public:
    Cube(QOpenGLShaderProgram *sh);
    virtual ~Cube (){delete vao;};
    virtual void draw(QOpenGLFunctions_3_3_Core* &glFuncs, Vec3 size);
protected:
};

class Floor : public myObject{
public:
    Floor(QOpenGLShaderProgram *sh);
    virtual ~Floor (){delete vao;};
    virtual void draw(QOpenGLFunctions_3_3_Core* &glFuncs, Vec3 size);
};

class Sphere : public myObject{
public:
    Sphere(QOpenGLShaderProgram *sh);

    virtual ~Sphere (){delete vao;};
    virtual void draw(QOpenGLFunctions_3_3_Core* &glFuncs, Vec3 size);
    void draw(QOpenGLFunctions_3_3_Core* &glFuncs, Vec3 size, Vec3 pos, Vec3 color);
protected:
    int numFaces = 0;
};

class Cloth : public myObject{
public:
    Cloth(QOpenGLShaderProgram *sh, int x, int y);
    virtual ~Cloth (){delete vao; delete vbo; delete ibo;};
    virtual void draw(QOpenGLFunctions_3_3_Core* &glFuncs, Vec3 size);

    void updateIndices(int x, int y);
    void updatePositions(ParticleSystem &system);
protected:

    QOpenGLBuffer * vbo = nullptr;
    QOpenGLBuffer * ibo = nullptr;

    int numMeshIndices = 0;
    int numParticles = 0;
};

#endif // MYOBJECT_H
