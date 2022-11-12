#include "myobject.h"
#include "model.h"
#include "glutils.h"
#include "auxfunctions.h"

Cube::Cube(QOpenGLShaderProgram *&sh){
    this->shader = sh;

    Model cube = Model::createCube();
    vao = glutils::createVAO(sh, &cube);
    glutils::checkGLError();
}

void Cube::draw(QOpenGLFunctions_3_3_Core* &glFuncs, Vec3 size){
    QMatrix4x4 modelMat2;
    vao->bind();
    modelMat2.translate(0, std::abs(size.y()), 0);
    modelMat2.scale(size.x(), size.y(), size.z());
    shader->setUniformValue("ModelMatrix", modelMat2);
    shader->setUniformValue("matdiff", 1.f, 1.f, 1.f);
    shader->setUniformValue("matspec", 0.0f, 0.0f, 0.0f);
    shader->setUniformValue("matshin", 0.0f);
    glFuncs->glDrawElements(GL_LINES, 36, GL_UNSIGNED_INT, 0);
    glutils::checkGLError();
}

Floor::Floor(QOpenGLShaderProgram *&sh){
    this->shader = sh;

    Model quad = Model::createQuad();
    vao = glutils::createVAO(sh, &quad);
}


void Floor::draw(QOpenGLFunctions_3_3_Core* &glFuncs, Vec3 size){
    QMatrix4x4 modelMat;
    vao->bind();
    modelMat.scale(size.x(), size.y(), size.z());
    shader->setUniformValue("ModelMatrix", modelMat);
    shader->setUniformValue("matdiff", 1.f, 1.f, 1.f);
    shader->setUniformValue("matspec", 0.0f, 0.0f, 0.0f);
    shader->setUniformValue("matshin", 0.0f);
    glFuncs->glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    vao->release();
}

Sphere::Sphere(QOpenGLShaderProgram *&sh){
    shader = sh;
    Model sphere = Model::createIcosphere(1);
    vao = glutils::createVAO(shader, &sphere);
    numFaces = sphere.numFaces();
}

void Sphere::draw(QOpenGLFunctions_3_3_Core* &glFuncs, Vec3 size, Vec3 pos, Vec3 color){
    QMatrix4x4 modelMat;
    vao->bind();
    modelMat.translate(pos.x(), pos.y(), pos.z());
    modelMat.scale(size.x(), size.y(), size.z());
    shader->setUniformValue("ModelMatrix", modelMat);

    shader->setUniformValue("matdiff", GLfloat(color.x()), GLfloat(color.y()), GLfloat(color.z()));
    shader->setUniformValue("matspec", 1.0f, 1.0f, 1.0f);
    shader->setUniformValue("matshin", 100.f);

    glFuncs->glDrawElements(GL_TRIANGLES, 3*numFaces, GL_UNSIGNED_INT, 0);
    vao->release();

}
void Sphere::draw(QOpenGLFunctions_3_3_Core* &glFuncs, Vec3 size){
    QMatrix4x4 modelMat;
    vao->bind();
    modelMat.scale(size.x());
    shader->setUniformValue("ModelMatrix", modelMat);
    shader->setUniformValue("matdiff", 0.8f, 0.8f, 0.8f);
    shader->setUniformValue("matspec", 1.0f, 1.0f, 1.0f);
    shader->setUniformValue("matshin", 100.f);

    glFuncs->glDrawElements(GL_TRIANGLES, 3*numFaces, GL_UNSIGNED_INT, 0);
    vao->release();
}

Cloth::Cloth(QOpenGLShaderProgram *sh, int x, int y){
    shader = sh;

    vao = new QOpenGLVertexArrayObject();
    vao->create();
    vao->bind();

    vbo = new QOpenGLBuffer(QOpenGLBuffer::Type::VertexBuffer);
    vbo->create();
    vbo->bind();
    vbo->setUsagePattern(QOpenGLBuffer::UsagePattern::DynamicDraw);
    vbo->allocate(1000*1000*3*3*sizeof(float));

    shader->setAttributeArray("vertex", GL_FLOAT, 0, 3, 0);
    shader->enableAttributeArray("vertex");

    ibo = new QOpenGLBuffer(QOpenGLBuffer::Type::IndexBuffer);
    ibo->create();
    ibo->bind();
    ibo->setUsagePattern(QOpenGLBuffer::UsagePattern::DynamicDraw);
    ibo->allocate(1000*1000*2*3*sizeof(int));

    vao->release();
    updateIndices(x, y);
}

void Cloth::updateIndices(int x, int y){
    ibo->bind();
    numMeshIndices = (x - 1) * (y - 1) * 2 * 3;
    numParticles = x*y;
    int * indices = new int[numMeshIndices];
    int idx = 0;
    for (int i = 0; i < x - 1; i++){
        for(int j = 0; j < y - 1; j++){
            indices[idx + 0] = (i + 0)*y + j;
            indices[idx + 1] = (i + 1)*y + j;
            indices[idx + 2] = (i + 0)*y + j + 1;
            indices[idx + 3] = (i + 0)*y + j + 1;
            indices[idx + 4] = (i + 1)*y + j;
            indices[idx + 5] = (i + 1)*y + j + 1;
            idx +=6;
        }
    }
    void * bufptr = ibo->mapRange(0, numMeshIndices*sizeof(int), QOpenGLBuffer::RangeInvalidateBuffer | QOpenGLBuffer::RangeWrite);
    memcpy(bufptr, (void*)(indices), numMeshIndices*sizeof(int));

    ibo->unmap();
    ibo->release();
    delete[] indices;
    glutils::checkGLError();
}


void Cloth::updatePositions(ParticleSystem &system){
    vbo->bind();
    float* pos = new float[3*numParticles];
    for (int i = 0; i < numParticles; i++){
        pos[3*i + 0] = system.getParticle(i)->pos.x();
        pos[3*i + 1] = system.getParticle(i)->pos.y();
        pos[3*i + 2] = system.getParticle(i)->pos.z();
    }

    void *bufptr = vbo->mapRange(0, 3*numParticles*sizeof(float), QOpenGLBuffer::RangeInvalidateBuffer | QOpenGLBuffer::RangeWrite);
    memcpy(bufptr, (void*)(pos), 3*numParticles*sizeof(float));

    vbo->unmap();
    vbo->release();
    delete[] pos;
    glutils::checkGLError();
}

void Cloth::draw(QOpenGLFunctions_3_3_Core* &glFuncs, Vec3 size){
    shader->setUniformValue("ModelMatrix", QMatrix4x4());
    shader->setUniformValue("matspecFront", 1.0f, 1.0f, 1.0f);
    shader->setUniformValue("radius", 3);
    shader->setUniformValue("matdiffFront", GLfloat(1.0), GLfloat(1.0), GLfloat(0));
    shader->setUniformValue("matshinFront", 100.f);
    shader->setUniformValue("matspecBack", 0.0f, 0.0f, 0.0f);
    shader->setUniformValue("matdiffBack", GLfloat(0.0), GLfloat(1.0), GLfloat(0));
    shader->setUniformValue("matshinBack", 0.f);

    vao->bind();
    glFuncs->glDrawElements(GL_TRIANGLES, numMeshIndices, GL_UNSIGNED_INT, nullptr);
    vao->release();
    glutils::checkGLError();
}

Particle * Cloth::getClosestParticle(ParticleSystem system, Vec3 pos){
    Particle *p2 = nullptr;
    double d = 1000000;

    for(Particle *p : system.getParticles()){
        if(d > distance(Vec3(p->pos.x(), 0, p->pos.y()), pos)) {
            d = distance(Vec3(p->pos.x(), 0, p->pos.y()), pos);
            p2 = p;
        }
    }
    return p2;
}
