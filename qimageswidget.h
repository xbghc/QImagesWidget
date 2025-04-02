#ifndef QIMAGESWIDGET_H
#define QIMAGESWIDGET_H

#include <QGraphicsScene>
#include <QGridLayout>
#include <QImage>
#include <QList>
#include <QWidget>

namespace Ui{
class QImagesWidget;
}

class QImagesWidget : public QWidget
{
    Q_OBJECT

public:
    QImagesWidget(QWidget *parent = nullptr);
    ~QImagesWidget();

    int loadMrdFiles(QString path); // path是任意一个通道文件的路径，但是所有文件都需要在同一文件夹中

public slots:
    void setRowNum(int row);
    void setColNum(int col);
    void setHeight(int height);
    void setWidth(int width);

private:
    QList<QList<QImage>> m_channels;

    Ui::QImagesWidget* ui;

    void updateMarkers();
};
#endif // QIMAGESWIDGET_H
