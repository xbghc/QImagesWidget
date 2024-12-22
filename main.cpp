#include "qimageswidget.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

namespace {
void addSetFromMrdFile(QImagesWidget& w, QString filepath){

}

} // namespace

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QImagesWidget w;

    addSetFromMrdFile(w, "./data/20230528103740-T2_TSE-T-3k#1.mrd");
    w.show();
    return a.exec();
}
