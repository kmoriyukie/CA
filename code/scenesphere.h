#ifndef SceneSphere_H
#define SceneSphere_H

#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <list>
#include "scene.h"
#include "widgetboundingbox.h"
#include "particlesystem.h"
#include "integrators.h"
#include "colliders.h"

class SceneSphere : public Scene
{
    Q_OBJECT

public:
    SceneSphere();
    virtual ~SceneSphere();

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
    WidgetBoundingBox* widget = nullptr;

    QOpenGLShaderProgram* shader = nullptr;
    QOpenGLVertexArrayObject* vaoSphereS = nullptr;
    QOpenGLVertexArrayObject* vaoFloor   = nullptr;
    unsigned int numFacesSphereS = 0;

    IntegratorEuler integrator;
    ParticleSystem system;
    std::list<Particle*> deadParticles;
    ForceConstAcceleration* fGravity;
    ForceDrag *fDrag;
    ColliderSphere collider;

    Vec3 centerSphere;
    double radiusSphere;

    double kBounce, kFriction;
    double emitRate;
    double maxParticleLife;

    bool withDrag;

    Vec3 BoundingBoxPos, SourcePos;
    int mouseX, mouseY;
    int sizeSphere;
};

#endif // SceneSphere_H
