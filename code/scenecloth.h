#ifndef SceneCloth_H
#define SceneCloth_H

#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <list>
#include "scene.h"
#include "widgetcloth.h"
#include "particlesystem.h"
#include "integrators.h"
#include "colliders.h"
#include "myobject.h"

class SceneCloth : public Scene
{
    Q_OBJECT

public:
    SceneCloth();

    void createParticles();
    void updateIndices();
    void updateCoordsBuffer();

    virtual ~SceneCloth();

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
    WidgetCloth* widget = nullptr;

    QOpenGLShaderProgram* shader = nullptr;

    QOpenGLShaderProgram* shaderSphere = nullptr;

    QOpenGLBuffer * normMesh = nullptr;

    Cloth *cloth = nullptr;

    Sphere *sphere = nullptr;


    unsigned int numFacesSphereS = 0;
    int numMeshIndices = 0;
    int numParticlesX = 0;
    int numParticlesY = 0;
    int numParticles = 0;


    IntegratorEuler integrator;
    ParticleSystem system;
    std::list<Particle*> deadParticles;
    ForceConstAcceleration* fGravity;
    ForceDrag *fDrag;
    ColliderPlane colliderFloor;

    double kBounce, kFriction;
    double emitRate;
    double maxParticleLife;

    bool withDrag;

    Vec3 fountainPos;
    int mouseX, mouseY;
};

#endif // SceneCloth_H
