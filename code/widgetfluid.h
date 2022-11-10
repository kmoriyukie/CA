#ifndef widgetFluid_H
#define widgetFluid_H

#include <QWidget>

namespace Ui {
class widgetFluid;
}

class widgetFluid : public QWidget
{
    Q_OBJECT
public:
    explicit widgetFluid(QWidget *parent = nullptr);
    ~widgetFluid();


signals:
    void updatedParameters();

private:
    Ui::widgetFluid *ui;
};

#endif // widgetFluid_H
