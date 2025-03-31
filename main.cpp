#include "qimageswidget.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QImagesWidget w;

    // test part start
    w.loadMrdFiles("D:\\Projects\\QImagesWidget\\data\\20230528103740-T2_TSE-T-3k#1.mrd");
    w.setColNum(3);
    w.setRowNum(4);

    // test part end

    w.show();
    return a.exec();
}
