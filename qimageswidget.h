#ifndef QIMAGESWIDGET_H
#define QIMAGESWIDGET_H

#include <QGridLayout>
#include <QImage>
#include <QList>
#include <QWidget>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPointF>
#include <QVector>
#include <QEvent>
#include <QScrollArea>
#include <QClipboard>
#include <QApplication>


class QImagesWidgetItemView: public QGraphicsView{
    Q_OBJECT
public:
    QImagesWidgetItemView(QWidget* parent = nullptr);
    ~QImagesWidgetItemView() override;

    QGraphicsPixmapItem* setImage(const QImage& image);

    QPair<double, double> sceneOffset() const;
    void setSceneOffset(double hOffset, double vOffset);

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;

private:
    void saveImage(bool withObjects = false);
    void copyImage(bool withObjects = false);

    QImage m_image;
    QGraphicsScene m_scene;
};

/**
 * @brief 图像网格控件
 * 
 * 用于显示和管理图像网格的小部件，支持分页浏览和事件处理
 */
class QImagesWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QImagesWidget(QWidget *parent = nullptr);
    ~QImagesWidget() override;

    size_t colNum() const;
    void setColNum(size_t cols);

    size_t rowNum() const;
    void setRowNum(size_t rows);

    size_t pageIndex() const;
    bool setPageIndex(size_t index);

    size_t viewWidth() const;
    void setViewWidth(size_t width);

    size_t viewHeight() const;
    void setViewHeight(size_t height);

    size_t sceneWidth() const;
    void setSceneWidth(size_t width);

    size_t sceneHeight() const;
    void setSceneHeight(size_t height);

    void setImages(const QList<QImage>& images);
    
    int horizontalSpacing() const;
    void setHorizontalSpacing(int spacing);

    int verticalSpacing() const;
    void setVerticalSpacing(int spacing);

    bool isUpdateEnabled() const;

    /**
     * @brief 设置updateMarkers的启用状态，设置为false会停止画面更新
     * @param enable 是否启用更新
     */
    void setUpdateEnabled(bool enable);

    /**
     * @brief 获取指定位置的场景偏移量
     * @param row 行索引
     * @param col 列索引
     * @return 对应位置的场景偏移量
     */
    QPair<double, double> sceneOffset(int row, int col) const;

    /**
     * @brief 设置指定位置的场景偏移量
     * @param row 行索引
     * @param col 列索引
     * @param hOffset 水平偏移
     * @param vOffset 垂直偏移
     */
    void setSceneOffset(int row, int col, double hOffset, double vOffset);

    /**
     * @brief 添加图形项到指定位置
     * @param row 行索引
     * @param col 列索引
     * @param item 图形项对象
     * @return 添加是否成功
     */
    bool addItem(int row, int col, QGraphicsItem* item);
    
    /**
     * @brief 添加图形项到指定索引位置
     * @param index 线性索引
     * @param item 图形项对象
     * @return 添加是否成功
     */
    bool addItem(int index, QGraphicsItem* item);

    /**
     * @brief 返回指定位置的QGraphicsView对象
     * @param row 行索引
     * @param col 列索引
     * @return 对应位置的视图对象
     */
    const QGraphicsView* view(int row, int col) const;

    /**
     * @brief 返回指定位置的QImagesWidgetItemView对象
     * @param row 行索引
     * @param col 列索引
     * @return 对应位置的自定义视图对象
     */
    QImagesWidgetItemView* itemView(int row, int col) const;

    /**
     * @brief 更新视图标记
     */
    virtual void updateMarkers();

    /**
     * @brief 获取总页数
     * @return 总页数
     */
    size_t pageCount() const;

    /**
     * @brief 获取指定位置的场景对象
     * @param row 行索引
     * @param col 列索引
     * @return 对应位置的场景对象
     */
    QGraphicsScene* scene(int row, int col) const;
    
    /**
     * @brief 获取指定索引位置的图像
     * @param index 线性索引
     * @return 图像对象
     */
    const QImage& imageAt(size_t index) const;
    
    /**
     * @brief 获取指定位置的图像
     * @param row 行索引
     * @param col 列索引
     * @return 图像对象
     */
    const QImage& imageAt(int row, int col) const;

    void updateGrid();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    size_t m_colNum = 1;
    size_t m_rowNum = 1;
    size_t m_pageIndex = 0;
    size_t m_viewWidth = 256;
    size_t m_viewHeight = 256;
    size_t m_sceneWidth = 256;
    size_t m_sceneHeight = 256;
    bool m_enableUpdate = true;

    QList<QImage> m_images;

    QScrollArea* m_scrollArea;
    QWidget* m_contentWidget;
    QGridLayout* m_grid;

    void setupLayout();
    QGridLayout* gridLayout() const;
    
    bool isValidIndex(int row, int col) const;
};

#endif // QIMAGESWIDGET_H
