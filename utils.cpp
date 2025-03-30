#include "utils.h"

#include <QDebug>
#include <QFile>

QByteArray read(QString filepath)
{
    QFile file(filepath);
    if(!file.open(QIODevice::ReadOnly)){
        qDebug() << "Faild to open file: " << filepath;
        return nullptr;
    }

    return file.readAll();
}
