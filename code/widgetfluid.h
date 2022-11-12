#ifndef WidgetFluid_H
#define WidgetFluid_H

#include <QWidget>

namespace Ui {
class WidgetFluid;
}

class WidgetFluid : public QWidget
{
    Q_OBJECT
public:
    explicit WidgetFluid(QWidget *parent = nullptr);
    ~WidgetFluid();


signals:
    void updatedParameters();

private:
    Ui::WidgetFluid *ui;
};

#endif // WidgetFluid_H
