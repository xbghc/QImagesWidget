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

class QImagesWidget : public QWidget
{
    Q_OBJECT
public:
    QImagesWidget(QWidget *parent = nullptr);

    size_t colNum();
    void setColNum(size_t cols);
    size_t rowNum();
    void setRowNum(size_t lins);
    size_t pageIndex();
    void setPageIndex(size_t index);
    size_t width();
    void setWidth(size_t width);
    size_t height();
    void setHeight(size_t height);

    void setImages(QList<QImage> images);

    void addLine(int row, int col, QGraphicsLineItem* line);
    void addLine(int index, QGraphicsLineItem* line);

    virtual void updateMarkers();
    void init(size_t r, size_t c, size_t w, size_t h);

signals:

protected:
    std::pair<int, int> viewPosition(QGraphicsView* view) const;

private:
    size_t m_colNum=1;
    size_t m_rowNum=1;
    size_t m_pageIndex=0;
    size_t m_width=256;
    size_t m_height=256;

    QList<QImage> m_images;
    
    QVector<QGraphicsLineItem*> m_lines;

    // Utility functions
    QGridLayout* gridLayout() const;
    void updateGrid();
};

#endif // QIMAGESWIDGET_H
