#include "widgetfluid.h"
#include "ui_widgetfluid.h"

widgetFluid::widgetFluid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widgetFluid)
{
    ui->setupUi(this);

    connect(ui->btnUpdate, &QPushButton::clicked, this,
            [=] (void) { emit updatedParameters(); });
}

widgetFluid::~widgetFluid()
{
    delete ui;
}
