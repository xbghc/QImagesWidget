#include "mrdparser.h"

#include <QFile>
#include <QDebug>
#include <qendian.h>

namespace {

template <typename T>
fftw_complex* getKdata(QByteArray mrd, int num, bool isComplex){
    auto out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * num);

    auto buf = mrd.constData() + 512;
    int elementSize = sizeof(T);

    // check
    int posPPR = mrd.lastIndexOf('\x00');
    if (posPPR == -1) {
        qDebug() << "Corrupted MRD file: can't find posPPR";
        return nullptr;
    }

    if(511 + elementSize * (isComplex+1) * num != posPPR - 120){
        qDebug() << "Corrupted MRD file!";
        qDebug() << "elementSize * (isComplex+1) * num - posPPR should be -120, but: " << 511 + elementSize * (isComplex+1) * num - posPPR;
        return nullptr;
    }

    for(int i=0;i<num;i++){
        if(isComplex){
            auto real = qFromLittleEndian<T>(buf + elementSize*i);
            auto imag = qFromLittleEndian<T>(buf + elementSize*(i+1));
            out[i][0] = real;
            out[i][1] = imag;
            i++;
        }else{
            auto real = qFromLittleEndian<T>(buf + elementSize*i);
            out[i][0] = real;
            out[i][1] = 0;
        }
    }
    return out;
}

} // namespace

MrdData* MrdParser::parseFile(QString fpath)
{
    // ref:
    //   url: https://github.com/hongmingjian/mrscan/blob/master/smisscanner.py
    //   function: SmisScanner.parseMrd
    QFile file(fpath);
    if(!file.open(QIODevice::ReadOnly)){
        qDebug() << "Faild to open file: " << fpath;
        return nullptr;
    }

    auto mrd = file.readAll();
    if (mrd.size() < 512) {
        qDebug() << "MRD data too short!";
        return nullptr;
    }

    int samples = qFromLittleEndian<qint32>(mrd.constData());
    int views = qFromLittleEndian<qint32>(mrd.constData() + 4);
    int views2 = qFromLittleEndian<qint32>(mrd.constData() + 8);
    int slices = qFromLittleEndian<qint32>(mrd.constData() + 12);
    // 16-18 Unspecified
    int datatype = qFromLittleEndian<qint16>(mrd.constData() + 18);
    // 20-152 Unspecified
    int echoes = qFromLittleEndian<qint32>(mrd.constData() + 152);
    int experiments = qFromLittleEndian<qint32>(mrd.constData() + 156);

    int nele = experiments * echoes * slices * views * views2 * samples;

    int eleSize = 0;
    char dt;

    int offset = 512;
    fftw_complex* data;
    bool isComplex = datatype & 0x10;
    if ((datatype & 0xf) == 0) {
        data = getKdata<quint8>(mrd, nele, isComplex);
    } else if ((datatype & 0xf) == 1) {
        data = getKdata<qint8>(mrd, nele, isComplex);
    } else if ((datatype & 0xf) == 2) {
        data = getKdata<qint16>(mrd, nele, isComplex);
    } else if ((datatype & 0xf) == 3) {
        data = getKdata<qint16>(mrd, nele, isComplex);
    } else if ((datatype & 0xf) == 4) {
        data = getKdata<qint32>(mrd, nele, isComplex);
    } else if ((datatype & 0xf) == 5) {
        data = getKdata<float>(mrd, nele, isComplex);
    } else if ((datatype & 0xf) == 6) {
        data = getKdata<double>(mrd, nele, isComplex);
    } else {
        qDebug() << "Unknown data type in the MRD file!";
        return nullptr;
    }

    int posPPR = mrd.lastIndexOf(QByteArray(1, '\0'));
    if (posPPR == -1) {
        qDebug() << "Corrupted MRD file! (PPR not found)";
        return nullptr;
    }
    posPPR += 1;

    qint64 dataSize = posPPR - 512 - 120;
    if (dataSize < nele * eleSize) {
        qDebug() << "Corrupted MRD file! (dataSize < nele * eleSize)";
        return nullptr;
    }

    auto mrdData = new MrdData;
    mrdData->kdata = data;
    return mrdData;
}
