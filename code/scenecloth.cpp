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
    if(floor)       delete floor;
    if (fGravity)   delete fGravity;
    if(constraints) delete constraints;
}


void SceneCloth::initialize() {
    // load shader
    shaderSphere = glutils::loadShaderProgram(":/shaders/phong.vert", ":/shaders/phong.frag");
    shader = glutils::loadShaderProgram(":/shaders/cloth.vert", ":/shaders/cloth.geom", ":/shaders/cloth.frag");
    shaderSphere->bind();
    sphere = new Sphere(shaderSphere);
    floor = new Floor(shaderSphere);
    cube = new Cube(shaderSphere);
    glutils::checkGLError();

    shader->bind();
    numParticlesX = widget->getNumParticlesX();
    numParticlesY = widget->getNumParticlesY();


    cloth = new Cloth(shader, numParticlesX, numParticlesY);
    glutils::checkGLError();


    fGravity = new ForceConstAcceleration();
    system.addForce(fGravity);

    fSpring = new ForceSpring();
    system.addForce(fSpring);

    fDrag = new ForceDrag();
    system.addForce(fDrag);
    createParticles();

    constraints = new ConstraintsClothPBD(0.1, system, numParticlesX, numParticlesY);
    numParticles = numParticlesX * numParticlesY;
    numMeshIndices = (numParticlesX - 1) * (numParticlesY - 1) * 2 * 3;

    updateSimParams();
    collider.setBB(sizeBB, Vec3(0, sizeBB.y(), 0));
    colliderFloor.setPlane(Vec3(0, 1, 0), 0);
}


void SceneCloth::reset()
{
    // update values from UI
    updateSimParams();

    // reset random seed
    Random::seed(1337);

    // erase all particles
    fGravity->clearInfluencedParticles();
    fSpring->clearInfluencedParticles();
    createParticles();

    constraints->UpdateMesh(system, numParticlesX, numParticlesY);
}


void SceneCloth::updateSimParams()
{
    // get gravity from UI and update force
//    double g = widget->getGravity();
    numParticlesX = widget->getNumParticlesX();
    numParticlesY = widget->getNumParticlesY();
    numParticles = numParticlesX * numParticlesY;
    if (system.getNumParticles() != (unsigned int)(numParticles))
            createParticles();

    fGravity->setAcceleration(Vec3(0, -9, 0));
    fDrag->setDragConstant(0.001);
    fSpring->set(0.1, 0.1 , numParticlesX, numParticlesY, 1);

    // get other relevant UI values and update simulation params
    kBounce = 0.5;
    kFriction = 0.1;
    maxParticleLife = 10.0;
    emitRate = 100;

    cloth->updateIndices(numParticlesX, numParticlesY);
    constraints->UpdateMesh(system, numParticlesX, numParticlesY);

}


void SceneCloth::paint(const Camera& camera) {

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

    shader->bind();

    shader->setUniformValue("ProjMatrix", camProj);
    shader->setUniformValue("ViewMatrix", camView);
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


void SceneCloth::update(double dt) {
    if (system.getNumParticles() != numParticles)
            createParticles();


    // integration step
    Vecd ppos = system.getPositions();
    integrator.step(system, dt, numParticlesX, numParticlesY);
    system.setPreviousPositions(ppos);



//    constraints->step(system, numParticlesX, numParticlesY);

    // collisions
    for (Particle* p : system.getParticles()) {
        collider.collision(p, 0, kFriction);
        if(colliderFloor.testCollision(p))
            colliderFloor.resolveCollision(p, 0.2, 0.5);
    }


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
    cloth->updatePositions(system);
}

void SceneCloth::createParticles(){
    system.deleteParticles();

    fGravity->clearInfluencedParticles();
    fSpring->clearInfluencedParticles();
    for (int i = 0; i < numParticlesX ; i++){
        for (int j = 0; j < numParticlesY; j++){
            Particle *p = new Particle();

            float x = 1*i;
            float z = 1*j;
            float y = 25.0;

            p->pos = Vec3(x, y, z) + fountainPos;//.Vec3(fountainPos.x(), 10,fountainPos.z());
            p->vel = Vec3(0, 0, 0);
            p->id = i * numParticlesY + j;

            p->radius = 0.1;
            system.addParticle(p);

//            if(j == numParticlesX - 1 && (i==0 || i ==numParticlesY -1)){
//                p->is_static = true;
//            }
//            else p->is_static = false;

            fGravity->addInfluencedParticle(p);
            fSpring->addInfluencedParticle(p);
        }
    }

    cloth->updateIndices(numParticlesX, numParticlesY);

    glutils::checkGLError();
}
