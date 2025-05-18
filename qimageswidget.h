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

/**
 * @brief 图像网格控件
 * 
 * 用于显示和管理图像网格的小部件，支持分页浏览和事件处理
 */
class QImagesWidget : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief 构造图像网格控件
     * @param parent 父控件
     */
    explicit QImagesWidget(QWidget *parent = nullptr);
    
    /**
     * @brief 析构函数，释放所有资源
     */
    ~QImagesWidget() override;

    // getter和setter
    /**
     * @brief 获取列数
     * @return 当前列数
     */
    size_t colNum() const;
    
    /**
     * @brief 设置列数
     * @param cols 列数
     */
    void setColNum(size_t cols);

    /**
     * @brief 获取行数
     * @return 当前行数
     */
    size_t rowNum() const;
    
    /**
     * @brief 设置行数
     * @param rows 行数
     */
    void setRowNum(size_t rows);

    /**
     * @brief 获取当前页索引
     * @return 当前页索引
     */
    size_t pageIndex() const;
    
    /**
     * @brief 设置当前页索引
     * @param index 页索引
     * @return 设置是否成功
     */
    bool setPageIndex(size_t index);

    /**
     * @brief 获取图像宽度
     * @return 图像宽度
     */
    size_t width() const;
    
    /**
     * @brief 设置图像宽度
     * @param width 宽度
     */
    void setWidth(size_t width);

    /**
     * @brief 获取图像高度
     * @return 图像高度
     */
    size_t height() const;
    
    /**
     * @brief 设置图像高度
     * @param height 高度
     */
    void setHeight(size_t height);

    /**
     * @brief 设置图片列表
     * @param images 图片列表
     */
    void setImages(const QList<QImage>& images);

    /**
     * @brief 添加直线到指定位置
     * @param row 行索引
     * @param col 列索引
     * @param line 直线对象
     * @return 添加是否成功
     */
    bool addLine(int row, int col, QGraphicsLineItem* line);
    
    /**
     * @brief 添加直线到指定索引位置
     * @param index 线性索引
     * @param line 直线对象
     * @return 添加是否成功
     */
    bool addLine(int index, QGraphicsLineItem* line);

    /**
     * @brief 返回指定位置的QGraphicsView对象
     * @param row 行索引
     * @param col 列索引
     * @return 对应位置的视图对象
     */
    const QGraphicsView* view(int row, int col) const;

    /**
     * @brief 更新视图标记
     */
    virtual void updateMarkers();

    /**
     * @brief 初始化控件
     * @param rows 行数
     * @param cols 列数
     * @param width 宽度
     * @param height 高度
     */
    void init(size_t rows, size_t cols, size_t width, size_t height);

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
    const QGraphicsScene* scene(int row, int col) const;
    
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

signals:
    /**
     * @brief 当前页改变信号
     * @param index 新的页索引
     */
    void pageChanged(size_t index);
    
    /**
     * @brief 图像网格配置改变信号
     */
    void gridConfigChanged();

protected:
    /**
     * @brief 获取视图的位置
     * @param view 视图对象
     * @return 行列位置对
     */
    std::pair<int, int> viewPosition(QGraphicsView* view) const;

    /**
     * @brief 获取视图窗口的位置
     * @param view 视图窗口对象
     * @return 行列位置对
     */
    std::pair<int, int> viewPortPosition(QWidget* view) const;

    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    size_t m_colNum = 1;
    size_t m_rowNum = 1;
    size_t m_pageIndex = 0;
    size_t m_width = 128;
    size_t m_height = 128;

    QList<QImage> m_images;
    
    QVector<QGraphicsLineItem*> m_lines;

    // Utility functions
    /**
     * @brief 获取网格布局
     * @return 网格布局对象
     */
    QGridLayout* gridLayout() const;
    
    /**
     * @brief 更新网格布局
     */
    void updateGrid();

    /**
     * @brief 清除所有线条
     */
    void clearLines();
    
    /**
     * @brief 检查索引是否有效
     * @param row 行索引
     * @param col 列索引
     * @return 索引是否有效
     */
    bool isValidIndex(int row, int col) const;
};

#endif // QIMAGESWIDGET_H
