QT       += core gui opengl
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets openglwidgets


CONFIG += c++11

INCLUDEPATH += code
INCLUDEPATH += extlibs

VPATH += code

SOURCES += \
    code/auxfunctions.cpp \
    code/camera.cpp \
    code/colliders.cpp \
    code/constraintsPBD.cpp \
    code/forces.cpp \
    code/glutils.cpp \
    code/glwidget.cpp \
    code/integrators.cpp \
    code/main.cpp \
    code/mainwindow.cpp \
    code/model.cpp \
    code/myobject.cpp \
    code/particlesystem.cpp \
    code/scenebb.cpp \
    code/scenecloth.cpp \
    code/sceneclothPBD.cpp \
    code/scenefountain.cpp \
    code/sceneprojectiles.cpp \
    code/scenesphere.cpp \
    code/widgetboundingbox.cpp \
    code/widgetcloth.cpp \
    code/widgetfountain.cpp \
    code/widgetprojectiles.cpp

HEADERS += \
    code/auxfunctions.h \
    code/camera.h \
    code/colliders.h \
    code/constraintsPBD.h \
    code/defines.h \
    code/forces.h \
    code/glutils.h \
    code/glwidget.h \
    code/integrators.h \
    code/mainwindow.h \
    code/model.h \
    code/myobject.h \
    code/particle.h \
    code/particlesystem.h \
    code/scene.h \
    code/scenebb.h \
    code/scenecloth.h \
    code/sceneclothPBD.h \
    code/scenefountain.h \
    code/sceneprojectiles.h \
    code/scenesphere.h \
    code/widgetboundingbox.h \
    code/widgetcloth.h \
    code/widgetfountain.h \
    code/widgetprojectiles.h

FORMS += \
    forms/mainwindow.ui \
    forms/widgetBoundingBox.ui \
    forms/widgetCloth.ui \
    forms/widgetfountain.ui \
    forms/widgetprojectiles.ui \
    forms/widgetsphere.ui

RESOURCES += shaders.qrc

