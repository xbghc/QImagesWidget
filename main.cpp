#include "qimageswidget.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>


#include "mrdparser.h"
void testMrd(){
    QString fpath = "D:/Projects/QImagesWidget/data/20230528103740-T2_TSE-T-3k#1.mrd";
    auto data = MrdParser::parseFile(fpath);
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QImagesWidget w;

    testMrd();
    w.show();
    return a.exec();
}
