#include "qimageswidget.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>


#include "mrdparser.h"
void testMrd(QImagesWidget& widget){
    QString fpath = "D:/Projects/QImagesWidget/data/20230528103740-T2_TSE-T-3k#1.mrd";
    auto data = MrdParser::parseFile(fpath);

    auto images = MrdParser::reconImages(data, 256, 256);
    // auto images = MrdParser::reconImages(data, data->samples, data->views);
    widget.addSet(images, "test");

    QList<QString> showedSets = {"test"};
    QList<int> curImages;
    for(int i=0;i<12;i++){
        curImages.push_back(i);
    }

    widget.selectImages(showedSets, curImages);
    widget.setLins(3);
    widget.setCols(4);
    widget.updateMarkers();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QImagesWidget w;

    testMrd(w);
    w.show();
    return a.exec();
}
