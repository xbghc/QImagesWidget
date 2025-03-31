#ifndef CONTENTWIDGET_H
#define CONTENTWIDGET_H

#include <QGridLayout>
#include <QImage>
#include <QList>
#include <QWidget>


class ContentWidget : public QWidget
{
    Q_OBJECT
public:
    ContentWidget(QWidget *parent = nullptr);

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

    void updateMarkers();
signals:

private:
    size_t m_colNum=1;
    size_t m_rowNum=1;
    size_t m_pageIndex=0;
    size_t m_width=256;
    size_t m_height=256;

    // 功能函数
    void updateGrid();
    QList<QImage> m_images;
};

#endif // CONTENTWIDGET_H
