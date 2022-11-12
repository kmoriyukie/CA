#include "scenefluid.h"
#include "glutils.h"
#include "model.h"
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLVersionFunctionsFactory>



SceneFluid::SceneFluid() {
    widget = new WidgetFluid();
    connect(widget, SIGNAL(updatedParameters()), this, SLOT(updateSimParams()));
}


SceneFluid::~SceneFluid() {
    if (widget)     delete widget;
    if (shader)     delete shader;
    if(sphere)      delete sphere;
    if(floor)       delete floor;
    if (fGravity)   delete fGravity;
}


void SceneFluid::initialize() {
    // load shader
    shaderSphere = glutils::loadShaderProgram(":/shaders/phong.vert", ":/shaders/phong.frag");
//    shader = glutils::loadShaderProgram(":/shaders/cloth.vert", ":/shaders/cloth.geom", ":/shaders/cloth.frag");
    shaderSphere->bind();
    sphere = new Sphere(shaderSphere);
    floor = new Floor(shaderSphere);
    cube = new Cube(shaderSphere);
    glutils::checkGLError();


    fGravity = new ForceConstAcceleration();
    system.addForce(fGravity);

    numParticles = numParticlesX * numParticlesY * 10;
    createParticles();

    updateSimParams();
    collider.setBB(sizeBB, Vec3(0, sizeBB.y(), 0));
    colliderFloor.setPlane(Vec3(0, 1, 0), 0);
}


void SceneFluid::reset()
{
    // update values from UI
    updateSimParams();

    // reset random seed
    Random::seed(1337);

    // erase all particles
    createParticles();

}


void SceneFluid::updateSimParams()
{
    // get gravity from UI and update force
//    double g = widget->getGravity();
    numParticles = numParticlesX * numParticlesY * numParticlesZ;
    if (system.getNumParticles() != (unsigned int)(numParticles))
            createParticles();

    fGravity->setAcceleration(Vec3(0, -9.8, 0));

    // get other relevant UI values and update simulation params
    kBounce = 0.5;
    kFriction = 0.1;
    maxParticleLife = 10.0;
    emitRate = 100;


}


void SceneFluid::paint(const Camera& camera) {

    QOpenGLFunctions_3_3_Core* glFuncs = nullptr;

    glFuncs = QOpenGLVersionFunctionsFactory::get<QOpenGLFunctions_3_3_Core>(QOpenGLContext::currentContext());

    // camera matrices
    QMatrix4x4 camProj = camera.getPerspectiveMatrix();
    QMatrix4x4 camView = camera.getViewMatrix();


    // lighting
    const int numLights = 1;
    const QVector3D lightPosWorld[numLights] = {QVector3D(100,500,100)};
    const QVector3D lightColor[numLights] = {QVector3D(1,1,1)};
    QVector3D lightPosCam[numLights];
    for (int i = 0; i < numLights; i++) {
        lightPosCam[i] = camView.mapVector(lightPosWorld[i]);
    }


    shaderSphere->bind();

    shaderSphere->setUniformValue("ProjMatrix", camProj);
    shaderSphere->setUniformValue("ViewMatrix", camView);

    shaderSphere->setUniformValue("numLights", numLights);
    shaderSphere->setUniformValueArray("lightPos", lightPosCam, numLights);
    shaderSphere->setUniformValueArray("lightColor", lightColor, numLights);
    floor->draw(glFuncs, Vec3(20, 0, 20));

    cube->draw(glFuncs, sizeBB);

    for (const Particle* particle : system.getParticles()) {
        Vec3   p = particle->pos;
        Vec3   c = particle->color;
        double r = particle->radius;

        sphere->draw(glFuncs, Vec3(r, r, r), p, c);
    }

    shaderSphere->release();
    glutils::checkGLError();
}


void SceneFluid::update(double dt) {
    if (system.getNumParticles() != numParticles)
            createParticles();

    system.updateState();
    // integration step
    Vecd ppos = system.getPositions();
    integrator.step(system, dt);
    system.setPreviousPositions(ppos);

    for (Particle* p : system.getParticles()) {
        collider.collision(p, 0.1, kFriction);
        if(colliderFloor.testCollision(p))
            colliderFloor.resolveCollision(p, 0, 0.5);
    }


}

void SceneFluid::mousePressed(const QMouseEvent* e, const Camera&)
{
    mouseX = e->pos().x();
    mouseY = e->pos().y();
}

void SceneFluid::mouseMoved(const QMouseEvent* e, const Camera& cam)
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
        createParticles();

    }
    else if(e->modifiers() & Qt::AltModifier){
        sizeBB += disp;


        collider.setBB(sizeBB, Vec3(0, sizeBB.y(), 0));
        // do something else: e.g. move colliders
    }
    else if(e->modifiers() & Qt::Modifier::SHIFT){
//        Particle *p = cloth->getClosestParticle(system, camera, Vec3(mouseX, mouseY, 0));
//        p->pos += disp;
    }
    else {
        // do something else: e.g. move colliders
    }
}

void SceneFluid::createParticles(){
    system.deleteParticles();

    fGravity->clearInfluencedParticles();
    for (int i = 1; i <= numParticlesX ; i++){
        for (int j = 1; j <= numParticlesY; j++){
            for(int k = 1; k <= numParticlesZ; k ++){
                Particle *p = new Particle();

                float x = 1*i;
                float z = 1*j;
                float y = k;

                p->pos = Vec3(x, y, z) + fountainPos;//.Vec3(fountainPos.x(), 10,fountainPos.z());
                p->vel = Vec3(0, 0, 0);
                p->id = k * 10 + i * numParticlesY + j;

                p->radius = 0.5;
                p->mass = 0.01;
                system.addParticle(p);
                fGravity->addInfluencedParticle(p);
            }
        }
    }

    glutils::checkGLError();
}
