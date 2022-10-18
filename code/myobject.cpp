#include "myobject.h"
#include "model.h"
#include "glutils.h"

Cube::Cube(QOpenGLShaderProgram *sh){
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

Floor::Floor(QOpenGLShaderProgram *sh){
    this->shader = sh;

    Model quad = Model::createQuad();
    vao = glutils::createVAO(shader, &quad);
}


void Floor::draw(QOpenGLFunctions_3_3_Core* &glFuncs, Vec3 size){
    vao->bind();
    QMatrix4x4 modelMat;
    modelMat.scale(size.x(), size.y(), size.z());
    shader->setUniformValue("ModelMatrix", modelMat);
    shader->setUniformValue("matdiff", 0.8f, 0.8f, 0.8f);
    shader->setUniformValue("matspec", 0.0f, 0.0f, 0.0f);
    shader->setUniformValue("matshin", 0.0f);
    glFuncs->glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    vao->release();
}

Sphere::Sphere(QOpenGLShaderProgram *sh){
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

Cloth::Cloth(QOpenGLShaderProgram *sh){
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
}

void Cloth::update(int x, int y){
    ibo->bind();
    int numMeshIndices = (x - 1) * (y - 1) * 2 * 3;
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
    void * bufptr = ibo->mapRange(0, sizeof(indices), QOpenGLBuffer::RangeInvalidateBuffer | QOpenGLBuffer::RangeWrite);
    memcpy(bufptr, (void*)(indices), sizeof(indices));

    ibo->unmap();
    ibo->release();
    delete[] indices;
    glutils::checkGLError();
}


void Cloth::draw(QOpenGLFunctions_3_3_Core* &glFuncs, Vec3 size){
//    vbo->bind();
//    float* pos = new float[3*size.x()];
//    for (int i = 0; i < size.x(); i++){
//        pos[3*i + 0] = system.getParticle(i)->pos.x();
//        pos[3*i + 1] = system.getParticle(i)->pos.y();
//        pos[3*i + 2] = system.getParticle(i)->pos.z();

////        pos[3*i + 3] = 0;
////        pos[3*i + 4] = 1;
////        pos[3*i + 5] = 0;
//    }


//    void *bufptr = vboMesh->mapRange(0, sizeof(pos), QOpenGLBuffer::RangeInvalidateBuffer | QOpenGLBuffer::RangeWrite);
//    memcpy(bufptr, (void*)(pos), sizeof(pos));


//    vboMesh->unmap();
//    vboMesh->release();
//    delete[] pos;
}