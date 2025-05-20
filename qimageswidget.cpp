#include "qimageswidget.h"
#include "utils.h"

#include <qgraphicsitem.h>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QScrollArea>
#include <QVBoxLayout>


// Constructor & Destructor
QImagesWidget::QImagesWidget(QWidget *parent)
    : QWidget{parent}
    , m_viewWidth(256)
    , m_viewHeight(256)
    , m_sceneWidth(0)
    , m_sceneHeight(0)
    , m_scrollArea(nullptr)
    , m_contentWidget(nullptr)
    , m_grid(nullptr)
{
    setupLayout();
}

QImagesWidget::~QImagesWidget() {
    // Qt's parent-child ownership handles deletion of m_scrollArea,
    // m_contentWidget, and m_grid (as m_contentWidget's layout).
    // QGraphicsView instances added to m_grid are children of m_contentWidget.
    // If QGraphicsScene instances are children of their respective QGraphicsView instances (as per the updated updateGrid),
    // then they will be automatically deleted when their parent view is deleted.
    // No explicit cleanup of scenes is needed here or in updateGrid if that parentage is set.
}

// Public Getters & Setters
size_t QImagesWidget::colNum() const
{
    return m_colNum;
}

void QImagesWidget::setColNum(size_t cols)
{
    if (m_colNum == cols || cols <= 0) {
        return;
    }
    
    m_colNum = cols;
    updateGrid();
    updateMarkers();
}

size_t QImagesWidget::rowNum() const
{
    return m_rowNum;
}

void QImagesWidget::setRowNum(size_t rows)
{
    if (m_rowNum == rows || rows <= 0) {
        return;
    }
    
    m_rowNum = rows;
    updateGrid();
    updateMarkers();
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
    }
    return true;
}

size_t QImagesWidget::viewWidth() const
{
    return m_viewWidth;
}

void QImagesWidget::setViewWidth(size_t newViewWidth)
{
    if (newViewWidth == 0) {
        return;
    }
    if (m_viewWidth != newViewWidth) {
        m_viewWidth = newViewWidth;

        updateGrid();
        updateMarkers();
    }
}

size_t QImagesWidget::viewHeight() const
{
    return m_viewHeight;
}

void QImagesWidget::setViewHeight(size_t newViewHeight)
{
    if (newViewHeight == 0) {
        return;
    }
    if (m_viewHeight != newViewHeight) {
        m_viewHeight = newViewHeight;
        
        updateGrid();
        updateMarkers();
    }
}

size_t QImagesWidget::sceneWidth() const
{
    if (m_sceneWidth == 0 && m_viewWidth > 0) {
        return m_viewWidth;
    }
    return m_sceneWidth;
}

void QImagesWidget::setSceneWidth(size_t newSceneWidth)
{
    // Set scene width to 0 to re-enable tracking view width
    if (m_sceneWidth != newSceneWidth) {
        m_sceneWidth = newSceneWidth;
        updateGrid();
        updateMarkers();
    }
}

size_t QImagesWidget::sceneHeight() const
{
    if (m_sceneHeight == 0 && m_viewHeight > 0) {
        return m_viewHeight;
    }
    return m_sceneHeight;
}

void QImagesWidget::setSceneHeight(size_t newSceneHeight)
{
    // Set scene height to 0 to re-enable tracking view height
    if (m_sceneHeight != newSceneHeight) {
        m_sceneHeight = newSceneHeight;
        updateGrid();
        updateMarkers();
    }
}

void QImagesWidget::setImages(const QList<QImage>& images)
{
    m_images = images;
    m_pageIndex = 0;
    updateGrid();
    updateMarkers();
}

int QImagesWidget::horizontalSpacing() const
{
    return m_grid->horizontalSpacing();
}

void QImagesWidget::setHorizontalSpacing(int spacing)
{
    if (m_grid->horizontalSpacing() == spacing || spacing < 0) {
        return;
    }
    m_grid->setHorizontalSpacing(spacing);
    updateGrid();
    updateMarkers();
}

int QImagesWidget::verticalSpacing() const
{
    return m_grid->verticalSpacing();
}

void QImagesWidget::setVerticalSpacing(int spacing)
{
    if (m_grid->verticalSpacing() == spacing || spacing < 0) {
        return;
    }
    m_grid->setVerticalSpacing(spacing);
    updateGrid();
    updateMarkers();
}

bool QImagesWidget::isUpdateEnabled() const
{
    return m_enableUpdate;
}

void QImagesWidget::setUpdateEnabled(bool enable)
{
    m_enableUpdate = enable;
}

QPair<double, double> QImagesWidget::sceneOffset(int r, int c) const
{
    if (!isValidIndex(r, c)) {
        return qMakePair(0.0, 0.0);
    }
    int linear_index = r * static_cast<int>(m_colNum) + c;

    if (linear_index < 0 || linear_index >= m_scenesOffsets.size()) { 
        return qMakePair(0.0, 0.0);
    }
    return m_scenesOffsets[linear_index];
}

void QImagesWidget::setSceneOffset(int r, int c, double hOffset, double vOffset)
{
    if (!isValidIndex(r, c)) {
        return;
    }

    int linear_index = r * static_cast<int>(m_colNum) + c;
    if (linear_index < 0) { 
        LOG_ERROR("Calculated negative linear_index in setSceneOffset. This should not happen.");
        return; 
    }

    if (linear_index >= m_scenesOffsets.size()) {
        m_scenesOffsets.resize(linear_index + 1); 
    }

    m_scenesOffsets[linear_index] = qMakePair(hOffset, vOffset);
    updateGrid();
    updateMarkers();
}

// Public Methods
bool QImagesWidget::addLine(int row, int col, QGraphicsLineItem* line)
{
    if (!isValidIndex(row, col) || !line) {
        return false;
    }
    
    auto view = this->view(row, col);
    if (view && view->scene()) {
        view->scene()->addItem(line);
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

void QImagesWidget::updateMarkers()
{
    if (!m_enableUpdate || m_images.isEmpty()) {
        return;
    }

    size_t currentSceneWidth = this->sceneWidth();
    size_t currentSceneHeight = this->sceneHeight();

    if (currentSceneWidth == 0 || currentSceneHeight == 0) { // If effective scene size is zero, do nothing
         LOG_ERROR("Effective scene width or height is zero in updateMarkers. Skipping update.");
        return;
    }

    size_t page_offset = m_pageIndex * m_rowNum * m_colNum;
    for (size_t row = 0; row < m_rowNum; row++) {
        for (size_t col = 0; col < m_colNum; col++) {
            size_t index = page_offset + row * m_colNum + col;
            if (index >= static_cast<size_t>(m_images.size())) {
                auto view_to_clear = this->view(static_cast<int>(row), static_cast<int>(col));
                if (view_to_clear && view_to_clear->scene()) {
                    view_to_clear->scene()->clear();
                }
                continue;
            }

            auto view = this->view(static_cast<int>(row), static_cast<int>(col));
            if (!view || !view->scene()) {
                continue;
            }
            
            auto scene = view->scene();
            scene->clear();
            auto image = m_images[static_cast<int>(index)];
            image = image.scaled(static_cast<int>(currentSceneWidth), static_cast<int>(currentSceneHeight), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            auto pixmap = scene->addPixmap(QPixmap::fromImage(image));
            
            auto [hOffset, vOffset] = sceneOffset(static_cast<int>(row), static_cast<int>(col));
            pixmap->setPos(hOffset, vOffset);
        }
    }
}

size_t QImagesWidget::pageCount() const
{
    if (m_images.isEmpty() || m_rowNum == 0 || m_colNum == 0) {
        return 0;
    }
    
    size_t imagesPerPage = m_rowNum * m_colNum;
    return (m_images.size() + imagesPerPage - 1) / imagesPerPage; // 向上取整
}

QGraphicsScene* QImagesWidget::scene(int row, int col) const
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

void QImagesWidget::updateGrid()
{
    if (!m_enableUpdate) {
        return;
    }
    
    auto grid = this->gridLayout();
    
    while (QLayoutItem* item = grid->takeAt(0)) {
        if (QWidget* widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }

    // Use getters to ensure correct scene dimensions are used
    size_t currentSceneWidth = this->sceneWidth();
    size_t currentSceneHeight = this->sceneHeight();

    if (m_rowNum == 0 || m_colNum == 0 || currentSceneWidth == 0 || currentSceneHeight == 0) {
        m_contentWidget->setFixedSize(0,0);
        return;
    }
        
    for (size_t row = 0; row < m_rowNum; row++) {
        for (size_t col = 0; col < m_colNum; col++) {
            QGraphicsScene* scene = nullptr;
            auto [hOffset, vOffset] = sceneOffset(static_cast<int>(row), static_cast<int>(col));
            
            // Use effective scene dimensions from getters
            scene = new QGraphicsScene(hOffset, vOffset, static_cast<qreal>(currentSceneWidth), static_cast<qreal>(currentSceneHeight)); 
            
            auto view = new QGraphicsView(scene, m_contentWidget);
            scene->setParent(view);

            view->setFixedSize(static_cast<int>(m_viewWidth), static_cast<int>(m_viewHeight));
            view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            view->setAlignment(Qt::AlignCenter);
            view->setFrameShape(QFrame::NoFrame); 

            view->viewport()->installEventFilter(this);
            grid->addWidget(view, static_cast<int>(row), static_cast<int>(col));
        }
    }

    int totalViewsWidth = 0;
    int totalViewsHeight = 0;

    if (m_colNum > 0) {
        totalViewsWidth = static_cast<int>(m_colNum * m_viewWidth);
        if (m_colNum > 1) {
             totalViewsWidth += static_cast<int>((m_colNum - 1) * grid->horizontalSpacing());
        }
    }
    if (m_rowNum > 0) {
        totalViewsHeight = static_cast<int>(m_rowNum * m_viewHeight);
        if (m_rowNum > 1) {
            totalViewsHeight += static_cast<int>((m_rowNum - 1) * grid->verticalSpacing());
        }
    }
    
    m_contentWidget->setFixedSize(totalViewsWidth, totalViewsHeight);
}

// Protected Methods
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

void QImagesWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateGrid();
    updateMarkers();
}

// Private Methods
void QImagesWidget::setupLayout()
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(mainLayout);

    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setAlignment(Qt::AlignCenter);

    m_contentWidget = new QWidget(m_scrollArea);

    m_grid = new QGridLayout(m_contentWidget);
    m_grid->setSpacing(1);
    m_grid->setContentsMargins(0, 0, 0, 0);

    m_scrollArea->setWidget(m_contentWidget);

    mainLayout->addWidget(m_scrollArea);
}

QGridLayout *QImagesWidget::gridLayout() const
{
    return m_grid;
}

bool QImagesWidget::isValidIndex(int row, int col) const
{
    return (row >= 0 && row < static_cast<int>(m_rowNum) && 
            col >= 0 && col < static_cast<int>(m_colNum));
}
