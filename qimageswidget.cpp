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

    setMouseTracking(true);

}

QImagesWidget::~QImagesWidget() {
    clearLines();
    
    auto grid = gridLayout();
    if (grid) {
        while (grid->count() > 0) {
            auto item = grid->takeAt(0);
            if (auto widget = item->widget()) {
                if (auto view = qobject_cast<QGraphicsView*>(widget)) {
                    delete view->scene();
                }
                delete widget;
            }
            delete item;
        }
    }
}

void QImagesWidget::clearLines()
{
    for (auto line : m_lines) {
        delete line;
    }
    m_lines.clear();
}

void QImagesWidget::setColNum(size_t cols)
{
    if (m_colNum == cols || cols <= 0) {
        return;
    }
    
    m_colNum = cols;
    updateGrid();
    updateMarkers();
    emit gridConfigChanged();
}

void QImagesWidget::setRowNum(size_t rows)
{
    if (m_rowNum == rows || rows <= 0) {
        return;
    }
    
    m_rowNum = rows;
    updateGrid();
    updateMarkers();
    emit gridConfigChanged();
}

void QImagesWidget::setWidth(size_t width)
{
    if (m_width == width || width <= 0) {
        return;
    }
    
    m_width = width;
    updateMarkers();
    emit gridConfigChanged();
}

void QImagesWidget::setHeight(size_t height)
{
    if (m_height == height || height <= 0) {
        return;
    }
    
    m_height = height;
    updateMarkers();
    emit gridConfigChanged();
}

void QImagesWidget::setImages(const QList<QImage>& images)
{
    m_images = images;
    m_pageIndex = 0;
    updateGrid();
    updateMarkers();
    emit pageChanged(m_pageIndex);
}

bool QImagesWidget::addLine(int row, int col, QGraphicsLineItem* line)
{
    if (!isValidIndex(row, col) || !line) {
        return false;
    }
    
    auto view = this->view(row, col);
    if (view && view->scene()) {
        view->scene()->addItem(line);
        m_lines.append(line);
        return true;
    }
    return false;
}

bool QImagesWidget::addLine(int index, QGraphicsLineItem* line)
{
    if (index < 0 || !line) {
        return false;
    }
    
    int row = index/m_colNum;
    int col = index%m_colNum;
    return addLine(row, col, line);
}

const QGraphicsView *QImagesWidget::view(int row, int col) const
{
    if (!isValidIndex(row, col)) {
        return nullptr;
    }

    auto grid = gridLayout();
    if (!grid) {
        return nullptr;
    }

    auto item = grid->itemAtPosition(row, col);
    if (!item) {
        return nullptr;
    }

    return qobject_cast<QGraphicsView *>(item->widget());
}

size_t QImagesWidget::colNum() const
{
    return m_colNum;
}

size_t QImagesWidget::rowNum() const
{
    return m_rowNum;
}

size_t QImagesWidget::pageIndex() const
{
    return m_pageIndex;
}

bool QImagesWidget::setPageIndex(size_t index)
{
    if (index >= pageCount()) {
        return false;
    }
    
    if (m_pageIndex != index) {
        m_pageIndex = index;
        updateMarkers();
        emit pageChanged(index);
    }
    return true;
}

size_t QImagesWidget::width() const
{
    return m_width;
}

size_t QImagesWidget::height() const
{
    return m_height;
}

void QImagesWidget::updateMarkers()
{
    clearLines();

    if (m_images.isEmpty()) {
        return;
    }

    size_t page_offset = m_pageIndex * m_rowNum * m_colNum;
    for (int row = 0; row < m_rowNum; row++) {
        for (int col = 0; col < m_colNum; col++) {
            size_t index = page_offset + row * m_colNum + col;
            if (index >= m_images.size()) {
                return;
            }

            auto view = this->view(row, col);
            if (!view || !view->scene()) {
                continue;
            }
            
            auto scene = view->scene();
            scene->clear();
            auto image = m_images[index];
            auto pixmap = scene->addPixmap(QPixmap::fromImage(image));
            pixmap->setOffset(-scene->width()/2, -scene->height()/2);
        }
    }
}

void QImagesWidget::init(size_t rows, size_t cols, size_t width, size_t height)
{
    bool changed = false;
    
    if (m_rowNum != rows && rows > 0) {
        m_rowNum = rows;
        changed = true;
    }
    
    if (m_colNum != cols && cols > 0) {
        m_colNum = cols;
        changed = true;
    }
    
    if (m_width != width && width > 0) {
        m_width = width;
        changed = true;
    }
    
    if (m_height != height && height > 0) {
        m_height = height;
        changed = true;
    }
    
    if (changed) {
        updateGrid();
        emit gridConfigChanged();
    }
}

std::pair<int, int> QImagesWidget::viewPosition(QGraphicsView *view) const
{
    if (!view) {
        return {-1, -1};
    }
    
    auto grid = this->gridLayout();
    if (!grid) {
        return {-1, -1};
    }

    for (int row = 0; row < grid->rowCount(); row++) {
        for (int col = 0; col < grid->columnCount(); col++) {
            if (this->view(row, col) == view) {
                return {row, col};
            }
        }
    }

    return {-1, -1};
}

std::pair<int, int> QImagesWidget::viewPortPosition(QWidget *viewport) const
{
    if (!viewport) {
        return {-1, -1};
    }

    auto grid = this->gridLayout();
    if (!grid) {
        return {-1, -1};
    }

    for (int row = 0; row < grid->rowCount(); row++) {
        for (int col = 0; col < grid->columnCount(); col++) {
            if (this->view(row, col)->viewport() == viewport) {
                return {row, col};
            }
        }
    }

    return {-1, -1};
}

bool QImagesWidget::eventFilter(QObject *watched, QEvent *event)
{
    return QWidget::eventFilter(watched, event);
}

QGridLayout *QImagesWidget::gridLayout() const
{
    auto currentlayout = this->layout();
    auto grid = qobject_cast<QGridLayout*>(currentlayout);
    if (!grid) {
        LOG_ERROR("Layout is not a QGridLayout");
        return nullptr;
    }
    return grid;
}

size_t QImagesWidget::pageCount() const
{
    if (m_images.isEmpty() || m_rowNum == 0 || m_colNum == 0) {
        return 0;
    }
    
    size_t imagesPerPage = m_rowNum * m_colNum;
    return (m_images.size() + imagesPerPage - 1) / imagesPerPage; // 向上取整
}

void QImagesWidget::updateGrid()
{
    clearLines();
    
    auto grid = this->gridLayout();
    if (!grid) {
        return;
    }
    
    while (grid->count() > 0) {
        auto item = grid->takeAt(0);
        if (auto widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }

    auto sceneWidth = m_images.isEmpty() ? 256 : m_images[0].width();
    auto sceneHeight = m_images.isEmpty() ? 256 : m_images[0].height();
    
    size_t viewIndex = 0;
    
    for (size_t row = 0; row < m_rowNum; row++) {
        for (size_t col = 0; col < m_colNum; col++) {
            QGraphicsView* view = nullptr;
            QGraphicsScene* scene = nullptr;

            scene = new QGraphicsScene(-sceneWidth/2, -sceneHeight/2, sceneWidth, sceneHeight);
            view = new QGraphicsView(scene, this);
            view->viewport()->installEventFilter(this);

            view->scale(static_cast<double>(m_width)/sceneWidth, static_cast<double>(m_height)/sceneHeight);
            grid->addWidget(view, row, col, Qt::AlignCenter);
        }
    }
}

bool QImagesWidget::isValidIndex(int row, int col) const
{
    return (row >= 0 && row < static_cast<int>(m_rowNum) && 
            col >= 0 && col < static_cast<int>(m_colNum));
}

const QGraphicsScene* QImagesWidget::scene(int row, int col) const
{
    auto v = view(row, col);
    return v ? v->scene() : nullptr;
}

const QImage& QImagesWidget::imageAt(size_t index) const
{
    static QImage emptyImage;
    if (index >= m_images.size()) {
        return emptyImage;
    }
    return m_images[index];
}

const QImage& QImagesWidget::imageAt(int row, int col) const
{
    if (!isValidIndex(row, col)) {
        static QImage emptyImage;
        return emptyImage;
    }
    
    size_t page_offset = m_pageIndex * m_rowNum * m_colNum;
    size_t index = page_offset + row * m_colNum + col;
    return imageAt(index);
}
