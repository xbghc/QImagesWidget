#include "qimageswidget.h"

#include <QFile>

QImagesWidget::QImagesWidget(QWidget *parent)
    : QWidget(parent)
{
    setLayout(&grid);
}

QImagesWidget::~QImagesWidget()
{
}

int QImagesWidget::addSet(QList<QImage> images, QString setName)
{
    return 0;
}

int QImagesWidget::addSet(QList<ImageSet> imageSet, QString setName)
{
    return 0;
}

int QImagesWidget::addSetFromMrdFile(QString fpath, QString setName)
{


    return 0;
}

void QImagesWidget::updateMarkers()
{

    m_scenes.clear();


}

void QImagesWidget::clearGrid()
{
    // while(grid.count() > 0){
    //     auto scene = grid.takeAt(0)->widget();
    // }
    return;
}
