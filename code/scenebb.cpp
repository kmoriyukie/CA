#include "scenebb.h"
#include "glutils.h"
#include "model.h"
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLVersionFunctionsFactory>
#include "myobject.h"


SceneBoundingBox::SceneBoundingBox() {
    widget = new WidgetBoundingBox();
    connect(widget, SIGNAL(updatedParameters()), this, SLOT(updateSimParams()));

}


SceneBoundingBox::~SceneBoundingBox() {
    if (widget)     delete widget;
    if (shader)     delete shader;
    if (sphere)     delete sphere;
    if (floor)      delete floor;
    if (cube)    delete cube;
    if (fGravity)   delete fGravity;
}


void SceneBoundingBox::initialize() {
    // load shader
    shader = glutils::loadShaderProgram(":/shaders/phong.vert", ":/shaders/phong.frag");

    // create floor VAO
    floor = new Floor(shader);

    // create sphere VAOs
    sphere = new Sphere(shader);
    glutils::checkGLError();

    // Create Cube BB
    cube = new Cube(shader);
    glutils::checkGLError();

    // create forces
    fGravity = new ForceConstAcceleration();
    system.addForce(fGravity);
    fDrag = new ForceDrag();
    system.addForce(fDrag);

    sizeBB = 10;

    // scene
    SourcePos = Vec3(0, 40, 0);

    collider.setBB(Vec3(sizeBB, sizeBB, sizeBB), Vec3(0, sizeBB, 0));
}


void SceneBoundingBox::reset()
{
    // update values from UI
    updateSimParams();

    // reset random seed
    Random::seed(1337);

    // erase all particles
    fGravity->clearInfluencedParticles();
    system.deleteParticles();
    deadParticles.clear();
    fDrag->clearInfluencedParticles();
}


void SceneBoundingBox::updateSimParams()
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
}


void SceneBoundingBox::paint(const Camera& camera) {

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
        lightPosCam[i] = camView.map(lightPosWorld[i]);
    }
    shader->setUniformValue("numLights", numLights);
    shader->setUniformValueArray("lightPos", lightPosCam, numLights);
    shader->setUniformValueArray("lightColor", lightColor, numLights);

    // draw floor

    floor->draw(glFuncs, Vec3(100, 1, 100));


    cube->draw(glFuncs, Vec3(sizeBB, sizeBB, sizeBB));
    collider.setBB(Vec3(sizeBB, sizeBB, sizeBB), Vec3(0, sizeBB, 0));

    QMatrix4x4 modelMat;
    // draw the different spheres
    for (const Particle* particle : system.getParticles()) {
        Vec3   p = particle->pos;
        Vec3   c = particle->color;
        double r = particle->radius;

        sphere->draw(glFuncs, Vec3(r, r, r), p, c);
    }

    shader->release();
}


void SceneBoundingBox::update(double dt) {

    // emit new particles, reuse dead ones if possible
    int emitParticles = std::max(1, int(std::round(emitRate * dt)));
    for (int i = 0; i < emitParticles; i++) {
        Particle* p;
        if (!deadParticles.empty()) {
            // reuse one dead particle
            p = deadParticles.front();
            deadParticles.pop_front();
        }
        else {
            // create new particle
            p = new Particle();
            system.addParticle(p);

            // don't forget to add particle to forces that affect it
            fGravity->addInfluencedParticle(p);
            if(withDrag) fDrag->addInfluencedParticle(p);
        }

        p->color = Vec3(153/255.0, 217/255.0, 234/255.0);
        p->radius = 1.0;
        p->life = maxParticleLife;

        double alpha = 36.0*(Random::get(-10, 10)*1.0 - 5.0);
        double beta = 9.0*Random::get(-10, 10);

        double x = cos(alpha)*cos(beta);
        double y = sin(beta);
        double z = sin(alpha)*cos(beta);
        p->pos = Vec3(x, y, z) + SourcePos;
        p->vel.array() = 3*Vec3(x, y, z).array();
    }

    // integration step
    Vecd ppos = system.getPositions();
    integrator.step(system, dt);
    system.setPreviousPositions(ppos);

    // collisions
    for (Particle* p : system.getParticles()) {
        collider.collision(p, kBounce, kFriction);
    }

    // check dead particles
    for (Particle* p : system.getParticles()) {
        if (p->life > 0) {
            p->life -= dt;
            if (p->life < 0) {
                deadParticles.push_back(p);
            }
        }
    }
}

void SceneBoundingBox::mousePressed(const QMouseEvent* e, const Camera&)
{
    mouseX = e->pos().x();
    mouseY = e->pos().y();
}

void SceneBoundingBox::mouseMoved(const QMouseEvent* e, const Camera& cam)
{
    int dx = e->pos().x() - mouseX;
    int dy = e->pos().y() - mouseY;
    mouseX = e->pos().x();
    mouseY = e->pos().y();

    Vec3 disp = cam.worldSpaceDisplacement(dx, -dy, cam.getEyeDistance());

    // example
    if (e->modifiers() & Qt::ControlModifier) {
        // move BoundingBox
        SourcePos += disp;//Vec3(dx, dy, 0);
//        SourcePos += disp;

    }
    else if(e->modifiers() & Qt::AltModifier){
        sizeBB += disp.y();
        collider.setBB(Vec3(sizeBB, sizeBB, sizeBB), Vec3(0, sizeBB, 0));
        // do something else: e.g. move colliders
    }
}
