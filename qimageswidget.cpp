#include "qimageswidget.h"
#include "utils.h"

#include <qgraphicsitem.h>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QContextMenuEvent>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QGraphicsPixmapItem>
#include <QPainter>


QImagesWidgetItemView::QImagesWidgetItemView(QWidget* parent)
    : QGraphicsView(parent)
    , m_scene(this)
{
    setScene(&m_scene);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setAlignment(Qt::AlignCenter);
    setFrameShape(QFrame::NoFrame);
    setContextMenuPolicy(Qt::DefaultContextMenu);
}

QImagesWidgetItemView::~QImagesWidgetItemView() = default;

QGraphicsPixmapItem* QImagesWidgetItemView::setImage(const QImage& image)
{
    m_image = image;
    m_scene.clear();

    if (image.isNull()) {
        LOG_ERROR("setImage: image is null");
        return nullptr;
    }
    
    QPixmap pixmap = QPixmap::fromImage(image);
    return m_scene.addPixmap(pixmap);
}

void QImagesWidgetItemView::contextMenuEvent(QContextMenuEvent* event)
{
    if (m_image.isNull()) {
        QGraphicsView::contextMenuEvent(event);
        return;
    }

    QMenu contextMenu(this);
    
    // 保存图像子菜单
    QMenu* saveMenu = contextMenu.addMenu(tr("Save Image"));
    QAction* saveImageOnlyAction = saveMenu->addAction(tr("Save Original Image Only"));
    QAction* saveWithObjectsAction = saveMenu->addAction(tr("Save with Graphics Objects"));
    
    // 复制图像子菜单
    QMenu* copyMenu = contextMenu.addMenu(tr("Copy Image"));
    QAction* copyImageOnlyAction = copyMenu->addAction(tr("Copy Original Image Only"));
    QAction* copyWithObjectsAction = copyMenu->addAction(tr("Copy with Graphics Objects"));
    
    QAction* selectedAction = contextMenu.exec(event->globalPos());
    
    if (selectedAction == saveImageOnlyAction) {
        saveImage(false);
    } else if (selectedAction == saveWithObjectsAction) {
        saveImage(true);
    } else if (selectedAction == copyImageOnlyAction) {
        copyImage(false);
    } else if (selectedAction == copyWithObjectsAction) {
        copyImage(true);
    }
}

void QImagesWidgetItemView::saveImage(bool withObjects)
{
    if (m_image.isNull()) {
        return;
    }
    
    QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("Save Image"),
        QString("image.png"),
        tr("Image Files (*.png *.jpg *.jpeg *.bmp *.tiff)")
    );
    
    if (!fileName.isEmpty()) {
        QImage imageToSave;
        
        if (withObjects) {
            // 渲染整个场景（包含图形对象）
            QRectF sceneRect = m_scene.sceneRect();
            if (sceneRect.isEmpty()) {
                sceneRect = m_scene.itemsBoundingRect();
            }
            
            imageToSave = QImage(sceneRect.size().toSize(), QImage::Format_ARGB32);
            imageToSave.fill(Qt::transparent);
            
            QPainter painter(&imageToSave);
            painter.setRenderHint(QPainter::Antialiasing);
            m_scene.render(&painter, QRectF(), sceneRect);
            painter.end();
        } else {
            // 仅保存原图
            imageToSave = m_image;
        }
        
        if (imageToSave.save(fileName)) {
            QString message = withObjects ? 
                tr("Image with graphics objects saved to: %1").arg(fileName) :
                tr("Original image saved to: %1").arg(fileName);
            QMessageBox::information(this, tr("Success"), message);
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Failed to save image"));
        }
    }
}

void QImagesWidgetItemView::copyImage(bool withObjects)
{
    if (m_image.isNull()) {
        return;
    }
    
    QClipboard* clipboard = QApplication::clipboard();
    QImage imageToCopy;
    
    if (withObjects) {
        // 渲染整个场景（包含图形对象）
        QRectF sceneRect = m_scene.sceneRect();
        if (sceneRect.isEmpty()) {
            sceneRect = m_scene.itemsBoundingRect();
        }
        
        imageToCopy = QImage(sceneRect.size().toSize(), QImage::Format_ARGB32);
        imageToCopy.fill(Qt::transparent);
        
        QPainter painter(&imageToCopy);
        painter.setRenderHint(QPainter::Antialiasing);
        m_scene.render(&painter, QRectF(), sceneRect);
        painter.end();
    } else {
        // 仅复制原图
        imageToCopy = m_image;
    }
    
    clipboard->setImage(imageToCopy);
    
    QString message = withObjects ? 
        tr("Image with graphics objects copied to clipboard") :
        tr("Original image copied to clipboard");
    QMessageBox::information(this, tr("Success"), message);
}

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

QImagesWidget::~QImagesWidget() {}

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

bool QImagesWidget::addItem(int row, int col, QGraphicsItem* item)
{
    if (!isValidIndex(row, col) || !item) {
        return false;
    }
    
    auto view = this->view(row, col);
    if (view && view->scene()) {
        view->scene()->addItem(item);
        return true;
    }
    return false;
}

bool QImagesWidget::addItem(int index, QGraphicsItem* item)
{
    if (index < 0 || !item || m_colNum == 0) {
        return false;
    }
    
    int row = index/m_colNum;
    int col = index%m_colNum;
    return addItem(row, col, item);
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

QImagesWidgetItemView* QImagesWidget::itemView(int row, int col) const
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

    return qobject_cast<QImagesWidgetItemView*>(item->widget());
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
            
            auto itemView = this->itemView(static_cast<int>(row), static_cast<int>(col));
            if (!itemView) {
                continue;
            }
            
            if (index >= static_cast<size_t>(m_images.size())) {
                itemView->setImage(QImage());
                continue;
            }

            // 缩放图像并设置到自定义视图
            auto image = m_images[static_cast<int>(index)];
            image = image.scaled(static_cast<int>(currentSceneWidth), static_cast<int>(currentSceneHeight), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            auto pixmapItem = itemView->setImage(image);
            
            // 设置场景偏移
            auto [hOffset, vOffset] = sceneOffset(static_cast<int>(row), static_cast<int>(col));
            itemView->setSceneRect(hOffset, vOffset, static_cast<qreal>(currentSceneWidth), static_cast<qreal>(currentSceneHeight));
            
            // 设置pixmap位置（仅当pixmapItem不为空时）
            if (pixmapItem) {
                pixmapItem->setPos(hOffset, vOffset);
            }
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
            auto [hOffset, vOffset] = sceneOffset(static_cast<int>(row), static_cast<int>(col));
            
            auto view = new QImagesWidgetItemView(m_contentWidget);
            view->setSceneRect(hOffset, vOffset, static_cast<qreal>(currentSceneWidth), static_cast<qreal>(currentSceneHeight));

            view->setFixedSize(static_cast<int>(m_viewWidth), static_cast<int>(m_viewHeight));
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
            auto view = this->view(row, col);
            if (view && view->viewport() == viewport) {
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
