#include "scenesphere.h"
#include "glutils.h"
#include "model.h"
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLVersionFunctionsFactory>



SceneSphere::SceneSphere() {
    widget = new WidgetBoundingBox();
    connect(widget, SIGNAL(updatedParameters()), this, SLOT(updateSimParams()));

}


SceneSphere::~SceneSphere() {
    if (widget)     delete widget;
    if (shader)     delete shader;
    if (vaoFloor)   delete vaoFloor;
    if (vaoSphereS) delete vaoSphereS;
    if (fGravity)   delete fGravity;
}


void SceneSphere::initialize() {
    // load shader
    shader = glutils::loadShaderProgram(":/shaders/phong.vert", ":/shaders/phong.frag");

    // create floor VAO
    Model quad = Model::createQuad();
    vaoFloor = glutils::createVAO(shader, &quad);
    glutils::checkGLError();

    // create sphere VAOs
    Model sphere = Model::createIcosphere(1);
    vaoSphereS = glutils::createVAO(shader, &sphere);
    numFacesSphereS = sphere.numFaces();
    glutils::checkGLError();

    // create forces
    fGravity = new ForceConstAcceleration();
    system.addForce(fGravity);
    fDrag = new ForceDrag();
    system.addForce(fDrag);

//    sizeSphere = 10;
    centerSphere = Vec3(0,40,0);
    radiusSphere = 10;
    // scene
    SourcePos = Vec3(0, 40, 0);

    collider.setSphere(centerSphere, radiusSphere);
}


void SceneSphere::reset()
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


void SceneSphere::updateSimParams()
{
    // get gravity from UI and update force
    double g = widget->getGravity();
    fGravity->setAcceleration(Vec3(0, -g, 0));

    withDrag = widget->withDrag();
    double mu = widget->getDragConst();
    fDrag->setDragConstant(mu);
    // get other relevant UI values and update simulation params
    kBounce = 0.6;
    kFriction = 0.1;
    maxParticleLife = 10.0;
    emitRate = 100;
}


void SceneSphere::paint(const Camera& camera) {

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

    QMatrix4x4 modelMat;

    vaoSphereS->bind();

    modelMat = QMatrix4x4();
    modelMat.translate(centerSphere.x(),centerSphere.y(), centerSphere.z());
    modelMat.scale(radiusSphere);
    shader->setUniformValue("ModelMatrix", modelMat);

    shader->setUniformValue("matdiff", 0.8, 0.8, 0.8);
    shader->setUniformValue("matspec", 1.0f, 1.0f, 1.0f);
    shader->setUniformValue("matshin", 100.f);

    collider.setSphere(centerSphere, radiusSphere);
    glFuncs->glDrawElements(GL_LINES, 3*numFacesSphereS, GL_UNSIGNED_INT, 0);


    for (const Particle* particle : system.getParticles()) {
        Vec3   p = particle->pos;
        Vec3   c = particle->color;
        double r = particle->radius;

        modelMat = QMatrix4x4();
        modelMat.translate(p[0], p[1], p[2]);
        modelMat.scale(r);
        shader->setUniformValue("ModelMatrix", modelMat);

        shader->setUniformValue("matdiff", GLfloat(c[0]), GLfloat(c[1]), GLfloat(c[2]));
        shader->setUniformValue("matspec", 1.0f, 1.0f, 1.0f);
        shader->setUniformValue("matshin", 100.f);

        glFuncs->glDrawElements(GL_TRIANGLES, 3*numFacesSphereS, GL_UNSIGNED_INT, 0);
    }

    shader->release();
}


void SceneSphere::update(double dt) {

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

void SceneSphere::mousePressed(const QMouseEvent* e, const Camera&)
{
    mouseX = e->pos().x();
    mouseY = e->pos().y();
}

void SceneSphere::mouseMoved(const QMouseEvent* e, const Camera& cam)
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
        radiusSphere += disp.y();
        // do something else: e.g. move colliders
    }
}
