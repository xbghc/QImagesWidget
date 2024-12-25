#ifndef MRDPARSER_H
#define MRDPARSER_H

#include <QString>
#include <fftw3.h>

struct MrdData{
    fftw_complex* kdata;

    MrdData(){}
    ~MrdData(){
        fftw_free(kdata);
    }
};

class MrdParser
{
private:
    MrdParser() = delete;
    MrdParser(const MrdParser&) = delete;
    MrdParser& operator=(const MrdParser&) = delete;

public:
    static MrdData* parseFile(QString fpath);
};

#endif // MRDPARSER_H
