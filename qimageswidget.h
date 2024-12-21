#ifndef QIMAGESWIDGET_H
#define QIMAGESWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class QImagesWidget;
}
QT_END_NAMESPACE

class QImagesWidget : public QWidget
{
    Q_OBJECT

public:
    QImagesWidget(QWidget *parent = nullptr);
    ~QImagesWidget();

private:
    Ui::QImagesWidget *ui;
};
#endif // QIMAGESWIDGET_H
