#ifndef QIMAGESWIDGET_H
#define QIMAGESWIDGET_H

#include <QGraphicsScene>
#include <QGridLayout>
#include <QImage>
#include <QList>
#include <QWidget>

struct ImageSet
{
    QString name;
    QList<QImage> images;
    ImageSet() {}
};

class QImagesWidget : public QWidget
{
    Q_OBJECT

public:
    QImagesWidget(QWidget *parent = nullptr);
    ~QImagesWidget();

    int addSet(QList<QImage> images, QString setName);
    int addSet(QList<ImageSet> imageSet, QString setName);
    int addSetFromMrdFile(QString fpath, QString setName);

    void updateMarkers();
private:
    QList<ImageSet> m_imageSets;
    QList<QGraphicsScene> m_scenes;

    QGridLayout grid;

    int m_cols;
    int m_lins;

    void clearGrid();
};
#endif // QIMAGESWIDGET_H
