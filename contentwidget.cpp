#include "contentwidget.h"

#include <qgridlayout.h>
#include <QGraphicsScene>
#include <QGraphicsView>

ContentWidget::ContentWidget(QWidget *parent)
    : QWidget{parent}
{
    setLayout(new QGridLayout);
}

void ContentWidget::setColNum(size_t cols)
{
    m_colNum = cols;
    updateGrid();
    updateMarkers();
}

void ContentWidget::setRowNum(size_t lins)
{
    m_rowNum = lins;
    updateGrid();
    updateMarkers();
}

void ContentWidget::setWidth(size_t width)
{
    m_width = width;
    updateMarkers();
}

void ContentWidget::setHeight(size_t height)
{
    m_height = height;
    updateMarkers();
}

void ContentWidget::setImages(QList<QImage> images)
{
    m_images = images;
    m_pageIndex = 0;
    updateMarkers();
}

size_t ContentWidget::colNum()
{
    return m_colNum;
}

size_t ContentWidget::rowNum()
{
    return m_rowNum;
}

void ContentWidget::updateMarkers()
{

    int size = m_images.size();
    size_t page_offset = m_pageIndex * m_rowNum * m_colNum;
    auto grid = qobject_cast<QGridLayout *>(this->layout());
    if(!grid){
        qWarning() << "Layout is not a QGridLayout";
        return;
    }

    for(int row=0;row<m_rowNum;row++){
        for(int col=0;col<m_colNum;col++){
            size_t index = page_offset + row * m_colNum + col;
            if(index >= size){
                return;
            }

            auto view = qobject_cast<QGraphicsView *>(grid->itemAtPosition(row, col)->widget());
            auto scene = view->scene();
            scene->clear();
            auto image = m_images[index].scaled(m_width, m_height,Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            scene->addPixmap(QPixmap::fromImage(image));
        }
    }
}

void ContentWidget::updateGrid()
{
    auto grid = (QGridLayout *)this->layout();
    while(grid->count() > 0){
        auto item = grid->takeAt(0);
        if(auto widget = item->widget()){
            delete widget;
        }
        delete item;
    }

    for(size_t row=0;row<m_rowNum;row++){
        for(size_t col=0;col<m_colNum;col++){
            auto scene = new QGraphicsScene(0, 0, m_width, m_height);
            auto view = new QGraphicsView(scene);
            grid->addWidget(view, row, col);
        }
    }
}
