#ifndef MYOBJECT_H
#define MYOBJECT_H

#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLBuffer>
#include "defines.h"

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
    virtual ~Cube (){};
    virtual void draw(QOpenGLFunctions_3_3_Core* &glFuncs, Vec3 size);
protected:
};

class Floor : public myObject{
public:
    Floor(QOpenGLShaderProgram *sh);
    virtual ~Floor (){};
    virtual void draw(QOpenGLFunctions_3_3_Core* &glFuncs, Vec3 size);
};

class Sphere : public myObject{
public:
    Sphere(QOpenGLShaderProgram *sh);
    virtual ~Sphere (){};
    virtual void draw(QOpenGLFunctions_3_3_Core* &glFuncs, Vec3 size);
    void draw(QOpenGLFunctions_3_3_Core* &glFuncs, Vec3 size, Vec3 pos, Vec3 color);
protected:
    int numFaces = 0;
};

class Cloth : public myObject{
public:
    Cloth(QOpenGLShaderProgram *sh);
    virtual ~Cloth (){};
    virtual void draw(QOpenGLFunctions_3_3_Core* &glFuncs, Vec3 size);

    void update(int x, int y);
protected:

    QOpenGLBuffer * vbo = nullptr;
    QOpenGLBuffer * ibo = nullptr;
};

#endif // MYOBJECT_H
