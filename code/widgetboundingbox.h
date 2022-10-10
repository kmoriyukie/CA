#ifndef WIDGETBOUNDINGBOX_H
#define WIDGETBOUNDINGBOX_H

#include <QWidget>

namespace Ui {
class WidgetBoundingBox;
}

class WidgetBoundingBox : public QWidget
{
    Q_OBJECT
public:
    explicit WidgetBoundingBox(QWidget *parent = nullptr);
    ~WidgetBoundingBox();

    double getGravity()    const;
    bool withDrag() const;
    double getDragConst() const;

signals:
    void updatedParameters();

private:
    Ui::WidgetBoundingBox *ui;
};

#endif // WIDGETBOUNDINGBOX_H
