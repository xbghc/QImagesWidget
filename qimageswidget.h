#ifndef QIMAGESWIDGET_H
#define QIMAGESWIDGET_H

#include <QGraphicsScene>
#include <QGridLayout>
#include <QImage>
#include <QList>
#include <QWidget>

struct ImageSet
{
    QString name;
    QList<QImage> images;
    ImageSet() {}
    ImageSet(QString _name, QList<QImage> _images):name(_name), images(_images){}
};

class QImagesWidget : public QWidget
{
    Q_OBJECT

public:
    QImagesWidget(QWidget *parent = nullptr);
    ~QImagesWidget();

    int addSet(QList<QImage> images, QString setName);
    int addSetFromMrdFile(QString fpath, QString setName);

    int getHeight();
    int getWidth();
    void setHeight(int height);
    void setWidth(int width);

    void setCols(int cols);
    void setLins(int lins);

    void selectImages(QList<QString> sets, QList<int> images);
    void updateMarkers();
private:
    QList<ImageSet> m_imageSets;
    QList<QGraphicsScene> m_scenes;

    QGridLayout grid;

    int m_cols;
    int m_lins;
    int curPageNum;

    int m_height;
    int m_width;

    int diffDirection; // 1：纵向对比, 0: 横向对比

    QList<QString> curSets;
    QList<int> curImages;

    void clearGrid();
    void addScene(const ImageSet& set, int index, int lin, int col);

    ImageSet* getSet(QString name);
    QList<ImageSet> getCurSets();

};
#endif // QIMAGESWIDGET_H
