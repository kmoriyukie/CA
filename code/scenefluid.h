#ifndef SceneFluid_H
#define SceneFluid_H

#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <list>
#include "constraintsPBD.h"
#include "scene.h"
#include "widgetfluid.h"
//#include "particlesystem.h"
#include "fluidsystem.h"
#include "integrators.h"
#include "colliders.h"
#include "myobject.h"

class SceneFluid : public Scene
{
    Q_OBJECT

public:
    SceneFluid();

    void createParticles();
    void updateIndices();
    void updateCoordsBuffer();

    virtual ~SceneFluid();

    virtual void initialize();
    virtual void reset();
    virtual void update(double dt);
    virtual void paint(const Camera& cam);

    virtual void mousePressed(const QMouseEvent* e, const Camera& cam);
    virtual void mouseMoved(const QMouseEvent* e, const Camera& cam);

    virtual void getSceneBounds(Vec3& bmin, Vec3& bmax) {
        bmin = Vec3(-50, -10, -50);
        bmax = Vec3( 50, 100, 50);
    }
    virtual unsigned int getNumParticles() { return system.getNumParticles(); }

    virtual QWidget* sceneUI() { return widget; }

public slots:
    void updateSimParams();

protected:
    WidgetFluid* widget = nullptr;

    QOpenGLShaderProgram* shader = nullptr;

    QOpenGLShaderProgram* shaderSphere = nullptr;

    QOpenGLBuffer * normMesh = nullptr;

    Sphere *sphere = nullptr;
    Cube *cube = nullptr;
    Floor *floor = nullptr;
    ColliderBB collider;

    Vec3 sizeBB = Vec3(6, 2, 6);

    unsigned int numFacesSphereS = 0;
    int numParticlesX = 7;
    int numParticlesY = 7;
    int numParticlesZ = 7;
    int numParticles = numParticlesX * numParticlesY * numParticlesZ;


    IntegratorSymplecticEuler integrator;
    FluidSystem system;
    std::list<Particle*> deadParticles;

    ForceConstAcceleration* fGravity;
    ColliderPlane colliderFloor;

    double kBounce, kFriction;
    double emitRate;
    double maxParticleLife;

    Vec3 fountainPos = Vec3(2.5,10,-2.5);
    int mouseX, mouseY;
};

#endif // SceneFluid_H
