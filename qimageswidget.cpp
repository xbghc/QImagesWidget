#include "qimageswidget.h"
#include "ui_qimageswidget.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QGraphicsView>

#include "mrdparser.h"

namespace {
QStringList getAllChannelsFile(QString path){
    QFileInfo fileInfo(path);
    if(!fileInfo.exists()){
        qWarning() << "File path does not exist: " << path;
        return {};
    }

    auto dir = fileInfo.absoluteDir();
    auto fileName = fileInfo.fileName();

    static QRegularExpression namePattern("^(.*)#(\\d+)\\.(\\w+)$");
    QRegularExpressionMatch match = namePattern.match(fileName);
    if (!match.hasMatch()) {
        qWarning() << "Invalid file name pattern, expected format: prefix#number.suffix";
        return {};
    }

    QString prefix = QRegularExpression::escape(match.captured(1));
    QString suffix = QRegularExpression::escape(match.captured(3));

    const QStringList files = dir.entryList(QDir::Files | QDir::Readable);
    auto newPattern = QString("^%1#\\d+\\.%2$").arg(prefix, suffix);
    auto result = files.filter(QRegularExpression(newPattern));
    for(auto& fn:result){
        fn = dir.filePath(fn);
    }
    return result;
}

template<typename T>
QList<T*> getPointerList(QList<T> objList){
    QList<T*> list;
    for(int i=0;i<objList.size();i++){
        auto obj = objList[i];
        qDebug() << obj;
        list.emplace_back(&obj);
    }

    // for(auto& obj:objList){
    //     list.push_back(&obj);
    // }
    return list;
}

} // namespace


QImagesWidget::QImagesWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::QImagesWidget)
{
    ui->setupUi(this);
}

QImagesWidget::~QImagesWidget()
{
}

int QImagesWidget::loadMrdFiles(QString fpath)
{
    auto files = getAllChannelsFile(fpath);
    for(const auto& file:files){
        auto content = MrdParser::parseFile(file);
        m_channels.emplace_back(MrdParser::reconImages(content));

        // TODO 在用户界面显示
        // auto label = file.split("#")[1].split(".")[0];
    }
    // 控制显示
    ui->contentWidget->setImages(m_channels[0]);

    return files.size();
}

void QImagesWidget::setRowNum(int row)
{
    ui->contentWidget->setRowNum(row);
}

void QImagesWidget::setColNum(int col)
{
    ui->contentWidget->setColNum(col);
}

void QImagesWidget::setHeight(int height)
{
    ui->contentWidget->setHeight(height);
}

void QImagesWidget::setWidth(int width)
{
    ui->contentWidget->setWidth(width);
}

