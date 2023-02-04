#include "erasemethod.h"
#include <QRandomGenerator>
#include <QString>

CEraseMethodBase::CEraseMethodBase()
{
    OverwriteBlock.resize(BlockSize);
}

QString CEraseMethodBase::GetMethodName(EraseMethod method)
{
    switch(method)
    {
     case EraseMethodSimple:
        return QString("Overwrite with random numbers");
    case EraseMethodDoD:
        return QString("Overwrite 3-times (DoD method)");
    case EraseMethodDoDII:
        return QString("Overwrite 6-times (DoD II method)");
    case EraseMethodVSITR:
        return QString("Overwrite 7-times (VSITR method)");
    case EraseMethodGutmann:
        return QString("Overwrite 35-times (Peter Gutmann method)");
    }

    return QString();
}

CEraseMethodBase* CEraseMethodBase::GetMethod(EraseMethod method)
{
    CEraseMethodBase* pMethod = nullptr;
    switch(method)
    {
    case EraseMethodDoD:
        pMethod = new CEraseMethodDoD();
        break;
    case EraseMethodDoDII:
        pMethod = new CEraseMethodDoDII();
        break;
    case EraseMethodVSITR:
        pMethod = new CEraseMethodVSITR();
        break;
    case EraseMethodGutmann:
        pMethod = new CEraseMethodGutmann();
        break;
    default:
        pMethod = new CEraseMethodSimple();
    }

    if(pMethod)
        pMethod->Initialize();

    return pMethod;
}

const QByteArray& CEraseMethodBase::GetOverwriteBlock() const
{
    return OverwriteBlock;
}

bool CEraseMethodBase::NextIteration()
{
    bool bRes = false;

    if (CurrentIteration < IterationsData.size())
    {
        if (IterationsData[CurrentIteration] == FILL_RANDOM_DATA)
            QRandomGenerator::global()->fillRange((quint64*)OverwriteBlock.data(), OverwriteBlock.size()/sizeof(quint64));
        else
            OverwriteBlock.fill(IterationsData[CurrentIteration]);

        ++CurrentIteration;
        bRes = true;
    }

    return bRes;
}

void CEraseMethodSimple::Initialize()
{
    IterationsData.push_back(FILL_RANDOM_DATA);
}

void CEraseMethodDoD::Initialize()
{
    IterationsData.push_back((char)0);
    IterationsData.push_back(0xFF);
    IterationsData.push_back(FILL_RANDOM_DATA);
}

void CEraseMethodDoDII::Initialize()
{
    IterationsData.push_back((char)0);
    IterationsData.push_back(0xFF);
    IterationsData.push_back(FILL_RANDOM_DATA);

    IterationsData.push_back((char)0);
    IterationsData.push_back(0xFF);
    IterationsData.push_back(FILL_RANDOM_DATA);
}

void CEraseMethodGutmann::Initialize()
{
    IterationsData.push_back(FILL_RANDOM_DATA);
    IterationsData.push_back(FILL_RANDOM_DATA);
    IterationsData.push_back(FILL_RANDOM_DATA);
    IterationsData.push_back(FILL_RANDOM_DATA);

    // see Gutmann specification: http://en.wikipedia.org/wiki/Gutmann_method
    IterationsData.push_back(0x55);
    IterationsData.push_back(0xAA);
    IterationsData.push_back(0x92); // 92 49 24...
    IterationsData.push_back(0x49); // 49 24 92...
    IterationsData.push_back(0x24); // 24 92 49...
    IterationsData.push_back((char)0);
    IterationsData.push_back(0x11);
    IterationsData.push_back(0x22);
    IterationsData.push_back(0x33);
    IterationsData.push_back(0x44);
    IterationsData.push_back(0x55);
    IterationsData.push_back(0x66);
    IterationsData.push_back(0x77);
    IterationsData.push_back(0x88);
    IterationsData.push_back(0x99);
    IterationsData.push_back(0xAA);
    IterationsData.push_back(0xBB);
    IterationsData.push_back(0xCC);
    IterationsData.push_back(0xDD);
    IterationsData.push_back(0xEE);
    IterationsData.push_back(0xFF);
    IterationsData.push_back(0x92); // 92 49 24...
    IterationsData.push_back(0x49); // 49 24 92...
    IterationsData.push_back(0x24); // 24 92 49...
    IterationsData.push_back(0x6D); // 6D B6 DB...
    IterationsData.push_back(0xB6); // B6 DB 6D...
    IterationsData.push_back(0xDB); // DB 6D B6...

    IterationsData.push_back(FILL_RANDOM_DATA);
    IterationsData.push_back(FILL_RANDOM_DATA);
    IterationsData.push_back(FILL_RANDOM_DATA);
    IterationsData.push_back(FILL_RANDOM_DATA);
}

void CEraseMethodVSITR::Initialize()
{
    IterationsData.push_back((char)0);
    IterationsData.push_back(0xFF);
    IterationsData.push_back((char)0);
    IterationsData.push_back(0xFF);
    IterationsData.push_back((char)0);
    IterationsData.push_back(0xFF);
    IterationsData.push_back(0xAA);
}
