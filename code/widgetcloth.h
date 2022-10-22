#ifndef WIDGETCLOTH_H
#define WIDGETCLOTH_H

#include <QWidget>

namespace Ui {
class WidgetCloth;
}

class WidgetCloth : public QWidget
{
    Q_OBJECT
public:
    explicit WidgetCloth(QWidget *parent = nullptr);
    ~WidgetCloth();

    int getNumParticlesX();
    int getNumParticlesY();


signals:
    void updatedParameters();

private:
    Ui::WidgetCloth *ui;
};

#endif // WIDGETCLOTH_H
