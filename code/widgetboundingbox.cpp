#include "widgetBoundingBox.h"
#include "ui_widgetBoundingBox.h"

WidgetBoundingBox::WidgetBoundingBox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetBoundingBox)
{
    ui->setupUi(this);

    connect(ui->btnUpdate, &QPushButton::clicked, this,
            [=] (void) { emit updatedParameters(); });
    setMouseTracking(true);

}

WidgetBoundingBox::~WidgetBoundingBox()
{
    delete ui;
}

double WidgetBoundingBox::getGravity() const {
    return ui->gravity->value();
}

bool WidgetBoundingBox::withDrag() const{
    return ui->drag->isChecked();
}

double WidgetBoundingBox::getDragConst() const{
    return ui->DragConst->value();
}
