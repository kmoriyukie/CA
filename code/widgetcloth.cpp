#include "widgetcloth.h"
#include "ui_widgetCloth.h"

WidgetCloth::WidgetCloth(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetCloth)
{
    ui->setupUi(this);

    connect(ui->btnUpdate, &QPushButton::clicked, this,
            [=] (void) { emit updatedParameters(); });
}

WidgetCloth::~WidgetCloth()
{
    delete ui;
}

int WidgetCloth::getNumParticlesX(){
    return ui->spinBox_npX->value();
}


int WidgetCloth::getNumParticlesY(){
    return ui->spinBox_npY->value();
}
