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

double WidgetCloth::getGravity() const {
    return ui->gravity->value();
}

bool WidgetCloth::withDrag() const{
    return ui->drag->isChecked();
}

double WidgetCloth::getDragConst() const{
    return ui->DragConst->value();
}
