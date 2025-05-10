#include "qimageswidget.h"
#include "utils.h"

#include <qgraphicsitem.h>
#include <qgridlayout.h>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMouseEvent>


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
    auto view = qobject_cast<QGraphicsView*>(item);
    if (view && view->scene()) {
        view->scene()->addItem(line);
        m_lines.append(line);
    }
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
    m_lines.clear();
    
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
    m_lines.clear();
    
    auto grid = this->gridLayout();

    while(grid->count() > 0){
        auto item = grid->takeAt(0);
        if(auto widget = item->widget()){
            widget->deleteLater();
        }
        delete item;
    }

    auto sceneWidth = m_images.length()?m_images[0].width():256;
    auto sceneHeight = m_images.length()?m_images[0].height():256;
    
    for(size_t row=0;row<m_rowNum;row++){
        for(size_t col=0;col<m_colNum;col++){
            auto scene = new QGraphicsScene(-sceneWidth/2, -sceneHeight/2, sceneWidth, sceneHeight);
            auto view = new QGraphicsView(scene, this);
            view->scale(static_cast<double>(m_width)/sceneWidth, static_cast<double>(m_height)/sceneHeight);
            view->installEventFilter(this);

            grid->addWidget(view, row, col, Qt::AlignCenter);
        }
    }
    this->setLayout(grid);
}

std::pair<int, int> QImagesWidget::getViewPos(QGraphicsView *view) const
{
    auto grid = this->gridLayout();

    for(int row=0;row<grid->rowCount();row++){
        for(int col=0;col<grid->columnCount();col++){
            auto item = grid->itemAtPosition(row, col);
            if(item && item->widget() == view){
                return {row, col};
            }
        }
    }

    LOG_ERROR("QGraphicsView not found in QImagesWidget's layout");
    throw std::runtime_error("QGraphicsView not found in QImagesWidget's layout");
}

bool QImagesWidget::eventFilter(QObject *obj, QEvent *event)
{
    auto view = qobject_cast<QGraphicsView*>(obj);
    if(!view){
        return false;
    }

    auto [row, col] = getViewPos(view);
    return viewEventFilter(row, col, event);
}

bool QImagesWidget::viewEventFilter(int row, int col, QEvent *event)
{
    return false;
}

QGridLayout *QImagesWidget::gridLayout() const
{
    auto currentlayout = this->layout();
    auto grid=qobject_cast<QGridLayout*>(currentlayout);
    if(!grid){
        LOG_ERROR("Layout is not a QGridLayout");
        throw std::runtime_error("Layout is not a QGridLayout in QImagesWidget::getViewPos");
    }
    return grid;
}

size_t QImagesWidget::pageIndex()
{
    return m_pageIndex;
}

void QImagesWidget::setPageIndex(size_t index)
{
    m_pageIndex = index;
    updateMarkers();
}

size_t QImagesWidget::width()
{
    return m_width;
}

size_t QImagesWidget::height()
{
    return m_height;
}
