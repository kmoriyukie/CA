#include "scenecloth.h"
#include "glutils.h"
#include "model.h"
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLVersionFunctionsFactory>



SceneCloth::SceneCloth() {
    widget = new WidgetCloth();
    connect(widget, SIGNAL(updatedParameters()), this, SLOT(updateSimParams()));
}


SceneCloth::~SceneCloth() {
    if (widget)     delete widget;
    if (shader)     delete shader;
    if(sphere)   delete sphere;
    if (vaoMesh)    delete vaoMesh;
    if (fGravity)   delete fGravity;
}


void SceneCloth::initialize() {
    // load shader
    shaderSphere = glutils::loadShaderProgram(":/shaders/phong.vert", ":/shaders/phong.frag");
    shader = glutils::loadShaderProgram(":/shaders/cloth.vert", ":/shaders/cloth.geom", ":/shaders/cloth.frag");

    sphere = new Sphere(shaderSphere);
    glutils::checkGLError();

    shader->bind();
    vaoMesh = new QOpenGLVertexArrayObject();
    vaoMesh->create();
    vaoMesh->bind();

    vboMesh = new QOpenGLBuffer(QOpenGLBuffer::Type::VertexBuffer);
    vboMesh->create();
    vboMesh->bind();
    vboMesh->setUsagePattern(QOpenGLBuffer::UsagePattern::DynamicDraw);
    vboMesh->allocate(1000*1000*3*3*sizeof(float));

    shader->setAttributeBuffer("vertex", GL_FLOAT, 0, 3, 0);
    shader->enableAttributeArray("vertex");

    iboMesh = new QOpenGLBuffer(QOpenGLBuffer::Type::IndexBuffer);
    iboMesh->create();
    iboMesh->bind();
    iboMesh->setUsagePattern(QOpenGLBuffer::UsagePattern::DynamicDraw);
    iboMesh->allocate(1000*1000*2*3*sizeof(int));
    //iboMesh->release();

    vaoMesh->release();
    glutils::checkGLError();
    //shader->release();

    fGravity = new ForceConstAcceleration();
    system.addForce(fGravity);

    fDrag = new ForceDrag();
    system.addForce(fDrag);

    numParticlesX = 5;
    numParticlesY = 10;

    numParticles = numParticlesX * numParticlesY;
    numMeshIndices = (numParticlesX - 1) * (numParticlesY - 1) * 2 * 3;

    createParticles();

    updateSimParams();

}


void SceneCloth::reset()
{
    // update values from UI
    updateSimParams();

    // reset random seed
    Random::seed(1337);

    // erase all particles
    fGravity->clearInfluencedParticles();
    system.deleteParticles();
    //deadParticles.clear();
//    fDrag->clearInfluencedParticles();

}


void SceneCloth::updateSimParams()
{
    // get gravity from UI and update force
    double g = widget->getGravity();
    fGravity->setAcceleration(Vec3(0, -g, 0));

    withDrag = widget->withDrag();
    double mu = widget->getDragConst();
    fDrag->setDragConstant(mu);
    // get other relevant UI values and update simulation params
    kBounce = 0.5;
    kFriction = 0.1;
    maxParticleLife = 10.0;
    emitRate = 100;


    updateIndices();

}


void SceneCloth::paint(const Camera& camera) {

    QOpenGLFunctions_3_3_Core* glFuncs = nullptr;

    glFuncs = QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_3_3_Core>(QOpenGLContext::currentContext());

    shader->bind();

    // camera matrices
    QMatrix4x4 camProj = camera.getPerspectiveMatrix();
    QMatrix4x4 camView = camera.getViewMatrix();
    shader->setUniformValue("ProjMatrix", camProj);
    shader->setUniformValue("ViewMatrix", camView);

    // lighting
    const int numLights = 1;
    const QVector3D lightPosWorld[numLights] = {QVector3D(100,500,100)};
    const QVector3D lightColor[numLights] = {QVector3D(1,1,1)};
    QVector3D lightPosCam[numLights];
    for (int i = 0; i < numLights; i++) {
        lightPosCam[i] = camView.mapVector(lightPosWorld[i]);
    }
    shader->setUniformValue("numLights", numLights);
    shader->setUniformValueArray("lightPos", lightPosCam, numLights);
    shader->setUniformValueArray("lightColor", lightColor, numLights);

    updateCoordsBuffer();
    shader->setUniformValue("ModelMatrix", QMatrix4x4());
    shader->setUniformValue("matspecFront", 1.0f, 1.0f, 1.0f);
    shader->setUniformValue("radius", 3);
    shader->setUniformValue("matdiffFront", GLfloat(1.0), GLfloat(1.0), GLfloat(0));
    shader->setUniformValue("matshinFront", 100.f);
    shader->setUniformValue("matspecBack", 0.0f, 0.0f, 0.0f);
    shader->setUniformValue("matdiffBack", GLfloat(0.0), GLfloat(1.0), GLfloat(0));
    shader->setUniformValue("matshinBack", 0.f);

    vaoMesh->bind();
    glFuncs->glDrawElements(GL_TRIANGLES, numMeshIndices, GL_UNSIGNED_INT, nullptr);
//    glFuncs->glDrawArrays(GL_TRIANGLES, 0, numParticles);

    vaoMesh->release();
    glutils::checkGLError();

    shader->release();

    shaderSphere->bind();

    shaderSphere->setUniformValue("ProjMatrix", camProj);
    shaderSphere->setUniformValue("ViewMatrix", camView);
    shaderSphere->setUniformValue("numLights", numLights);
    shaderSphere->setUniformValueArray("lightPos", lightPosCam, numLights);
    shaderSphere->setUniformValueArray("lightColor", lightColor, numLights);

    for (const Particle* particle : system.getParticles()) {
        Vec3   p = particle->pos;
        Vec3   c = particle->color;
        double r = particle->radius;

        sphere->draw(glFuncs, Vec3(r, r, r), p, c);
    }
    shaderSphere->release();
}


void SceneCloth::update(double dt) {
    if (system.getNumParticles() == 0)
            createParticles();
    // integration step
//    Vecd ppos = system.getPositions();
//    integrator.step(system, dt);
//    system.setPreviousPositions(ppos);

//    // collisions
//    for (Particle* p : system.getParticles()) {
//        collider.collision(p, kBounce, kFriction);
//    }

    // check dead particles

}

void SceneCloth::mousePressed(const QMouseEvent* e, const Camera&)
{
    mouseX = e->pos().x();
    mouseY = e->pos().y();
}

void SceneCloth::mouseMoved(const QMouseEvent* e, const Camera& cam)
{
    int dx = e->pos().x() - mouseX;
    int dy = e->pos().y() - mouseY;
    mouseX = e->pos().x();
    mouseY = e->pos().y();

    Vec3 disp = cam.worldSpaceDisplacement(dx, -dy, cam.getEyeDistance());

    // example
    if (e->modifiers() & Qt::ControlModifier) {
        // move fountain
        fountainPos += disp;
    }
    else {
        // do something else: e.g. move colliders
    }
}

void SceneCloth::createParticles(){
    for (int i = 0; i < numParticlesX ; i++){
        for (int j = 0; j < numParticlesY; j++){
            Particle *p = new Particle();


            float x = 10 + 10*i;
            float y = 10 + 10*j;
            float z = 0;

            p->pos = Vec3(x, y, z);
            p->vel = Vec3(0, 0, 0);
            system.addParticle(p);
        }
    }

    updateIndices();

    glutils::checkGLError();
}

void SceneCloth::updateIndices(){
    iboMesh->bind();
    numMeshIndices = (numParticlesX - 1) * (numParticlesY - 1) * 2 * 3;
    int * indices = new int[numMeshIndices];
    int idx = 0;
    for (int i = 0; i < numParticlesX - 1; i++){
        for(int j = 0; j < numParticlesY - 1; j++){
            indices[idx + 0] = (i + 0)*numParticlesY + j;
            indices[idx + 1] = (i + 1)*numParticlesY + j;
            indices[idx + 2] = (i + 0)*numParticlesY + j + 1;
            indices[idx + 3] = (i + 0)*numParticlesY + j + 1;
            indices[idx + 4] = (i + 1)*numParticlesY + j;
            indices[idx + 5] = (i + 1)*numParticlesY + j + 1;
            idx +=6;
        }
    }
    void * bufptr = iboMesh->mapRange(0, numMeshIndices*sizeof(int), QOpenGLBuffer::RangeInvalidateBuffer | QOpenGLBuffer::RangeWrite);
    memcpy(bufptr, (void*)(indices), numMeshIndices*sizeof(int));

    iboMesh->unmap();
    iboMesh->release();
    delete[] indices;
    glutils::checkGLError();
}

void SceneCloth::updateCoordsBuffer(){
    vboMesh->bind();
    float* pos = new float[3*numParticles];
    for (int i = 0; i < numParticles; i++){
        pos[3*i + 0] = system.getParticle(i)->pos.x();
        pos[3*i + 1] = system.getParticle(i)->pos.y();
        pos[3*i + 2] = system.getParticle(i)->pos.z();
    }


    void *bufptr = vboMesh->mapRange(0, 3*numParticles*sizeof(float), QOpenGLBuffer::RangeInvalidateBuffer | QOpenGLBuffer::RangeWrite);
    memcpy(bufptr, (void*)(pos), 3*numParticles*sizeof(float));


    vboMesh->unmap();
    vboMesh->release();
    delete[] pos;
}
