#include "qimageswidget.h"

#include <QFile>
#include <QGraphicsView>

QImagesWidget::QImagesWidget(QWidget *parent)
    : QWidget(parent)
{
    setLayout(&grid);
    m_width = 256;
    m_height = 256;
    curPageNum = 0;
}

QImagesWidget::~QImagesWidget()
{
}

int QImagesWidget::addSet(QList<QImage> images, QString setName)
{
    m_imageSets.append(ImageSet(setName, images));
    return 0;
}

int QImagesWidget::addSetFromMrdFile(QString fpath, QString setName)
{
    return 0;
}

int QImagesWidget::getHeight()
{
    return m_height;
}

int QImagesWidget::getWidth()
{
    return m_width;
}

void QImagesWidget::setHeight(int height)
{
    m_height = height;
}

void QImagesWidget::setWidth(int width)
{
    m_width = width;
}

void QImagesWidget::setCols(int cols)
{
    m_cols = cols;
    updateMarkers();
}

void QImagesWidget::setLins(int lins)
{
    m_lins = lins;
    updateMarkers();
}

void QImagesWidget::selectImages(QList<QString> sets, QList<int> images)
{
    curSets = sets;
    curImages = images;
    updateMarkers();
}

void QImagesWidget::updateMarkers()
{
    clearGrid();

    if(curSets.length() == 1){
        // 仅有一个Set，逐个列出图片

        const ImageSet* curSet = getSet(curSets[0]);
        int size = curSet->images.size();
        int offset = curPageNum * m_lins * m_cols;
        for(int i=0;i<m_lins&&offset<size;i++){
            for(int j=0;j<m_cols&&offset<size;j++){
                addScene(*curSet, offset, i, j);
                offset++;
            }
        }
    }else if(curImages.length() == 1){
        // 仅有一个ImageNum，逐个列出图片

    }else{
        // 对比各个图片
    }

}

void QImagesWidget::clearGrid()
{
    while(grid.count() > 0){
        auto widget = grid.takeAt(0)->widget();
        widget->setParent(nullptr);
        delete widget;
    }
    return;
}

void QImagesWidget::addScene(const ImageSet &set, int index, int lin, int col)
{
    auto scene = new QGraphicsScene(0, 0, m_width, m_height);
    scene->addPixmap(QPixmap::fromImage(set.images[index]));

    auto view = new QGraphicsView(scene);
    grid.addWidget(view, lin, col);
}

ImageSet* QImagesWidget::getSet(QString name)
{
    for(auto& set:m_imageSets){
        if(set.name == name){
            return &set;
        }
    }
    return nullptr;
}

QList<ImageSet> QImagesWidget::getCurSets()
{
    QList<ImageSet> sets;
    for(const auto& name:curSets){
        sets.append(*getSet(name));
    }
    return sets;
}
