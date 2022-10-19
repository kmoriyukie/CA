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
    if(sphere)      delete sphere;
    if (cloth)      delete cloth;
    if (fGravity)   delete fGravity;
}


void SceneCloth::initialize() {
    // load shader
    shaderSphere = glutils::loadShaderProgram(":/shaders/phong.vert", ":/shaders/phong.frag");
    shader = glutils::loadShaderProgram(":/shaders/cloth.vert", ":/shaders/cloth.geom", ":/shaders/cloth.frag");

    sphere = new Sphere(shaderSphere);
    glutils::checkGLError();

    shader->bind();
    numParticlesX = 5;
    numParticlesY = 10;

    cloth = new Cloth(shader, numParticlesX, numParticlesY);
    glutils::checkGLError();

    fGravity = new ForceConstAcceleration();
    system.addForce(fGravity);

    fDrag = new ForceDrag();
    system.addForce(fDrag);

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


    cloth->updateIndices(numParticlesX, numParticlesY);

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

    cloth->updatePositions(system);
    cloth->draw(glFuncs, Vec3(0,0,0));
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

    cloth->updateIndices(numParticlesX, numParticlesY);

    glutils::checkGLError();
}
