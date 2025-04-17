#include "qimageswidget.h"

#include <qgraphicsitem.h>
#include <qgridlayout.h>
#include <QGraphicsScene>
#include <QGraphicsView>

QImagesWidget::QImagesWidget(QWidget *parent)
    : QWidget{parent}
{
    setLayout(new QGridLayout(this));
}

void QImagesWidget::setColNum(size_t cols)
{
    m_colNum = cols;
    updateGrid();
    updateMarkers();
}

void QImagesWidget::setRowNum(size_t lins)
{
    m_rowNum = lins;
    updateGrid();
    updateMarkers();
}

void QImagesWidget::setWidth(size_t width)
{
    m_width = width;
    updateMarkers();
}

void QImagesWidget::setHeight(size_t height)
{
    m_height = height;
    updateMarkers();
}

void QImagesWidget::setImages(QList<QImage> images)
{
    m_images = images;
    m_pageIndex = 0;
    updateGrid();
    updateMarkers();
}

void QImagesWidget::addLine(int row, int col, QGraphicsLineItem* line)
{
    auto grid = qobject_cast<QGridLayout*>(layout());
    auto item = grid->itemAtPosition(row, col)->widget();
    auto scene = qobject_cast<QGraphicsView*>(item)->scene();
    qDebug() << "height: " << scene->sceneRect();
    scene->addItem(line);
}

void QImagesWidget::addLine(int index, QGraphicsLineItem* line)
{
    int row = index/m_colNum;
    int col = index%m_colNum;
    addLine(row, col, line);
}

size_t QImagesWidget::colNum()
{
    return m_colNum;
}

size_t QImagesWidget::rowNum()
{
    return m_rowNum;
}

void QImagesWidget::updateMarkers()
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
            auto image = m_images[index];
            auto pixmap = scene->addPixmap(QPixmap::fromImage(image));
            pixmap->setOffset(-scene->width()/2, -scene->height()/2);
        }
    }
}

void QImagesWidget::init(size_t r, size_t c, size_t w, size_t h)
{
    m_rowNum = r;
    m_colNum = c;
    m_width = w;
    m_height = h;
    updateGrid();
}

void QImagesWidget::updateGrid()
{
    auto grid = qobject_cast<QGridLayout*>(this->layout());

    while(grid->count() > 0){
        auto item = grid->takeAt(0);
        if(auto widget = item->widget()){
            delete widget;
        }
        delete item;
    }

    auto sceneWidth = m_images.length()?m_images[0].width():256;
    auto sceneHeight = m_images.length()?m_images[0].height():256;
    for(size_t row=0;row<m_rowNum;row++){
        for(size_t col=0;col<m_colNum;col++){
            // auto scene = new QGraphicsScene(0, 0, sceneWidth, sceneHeight);
            auto scene = new QGraphicsScene(-sceneWidth/2, -sceneHeight/2, sceneWidth, sceneHeight);
            auto view = new QGraphicsView(scene, this);
            view->scale(static_cast<double>(m_width)/sceneWidth, static_cast<double>(m_height)/sceneHeight);

            grid->addWidget(view, row, col, Qt::AlignCenter);
        }
    }
    this->setLayout(grid);
}
