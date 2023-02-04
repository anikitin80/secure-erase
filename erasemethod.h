#pragma once
#include "QByteArray.h"

#define FILL_RANDOM_DATA    0x1

enum EraseMethod
{
    EraseMethodSimple = 0,
    EraseMethodDoD,
    EraseMethodDoDII,
    EraseMethodVSITR,
    EraseMethodGutmann,
    /////////////////////////////////////
    EraseMethodFirst = EraseMethodSimple,
    EraseMethodLast = EraseMethodGutmann
};

class CEraseMethodBase
{
public:
    CEraseMethodBase();
    virtual ~CEraseMethodBase() {};

    static QString GetMethodName(EraseMethod method);
    static CEraseMethodBase* GetMethod(EraseMethod method);

    virtual void Initialize() = 0;

    int GetIterationsCount() { return IterationsData.size(); }
    void ResetIteration()    { CurrentIteration = 0; }
    bool NextIteration();

    const QByteArray& GetOverwriteBlock() const;
    int   GetBlockSize() const { return BlockSize; }

protected:
    QByteArray IterationsData;
    int CurrentIteration = 0;

    QByteArray OverwriteBlock;
    int OverwriteBlockSize = 0;

    const int BlockSize = 512*1024;  // 512 KB

private:
    CEraseMethodBase(const CEraseMethodBase&) = delete;
    CEraseMethodBase& operator= (const CEraseMethodBase&) = delete;
};

class CEraseMethodSimple : public CEraseMethodBase
{
public:
    virtual void Initialize() override;
};

class CEraseMethodDoD : public CEraseMethodBase
{
public:
     virtual void Initialize() override;
};

class CEraseMethodDoDII : public CEraseMethodBase
{
public:
     virtual void Initialize() override;
};

class CEraseMethodGutmann : public CEraseMethodBase
{
public:
     virtual void Initialize() override;
};

class CEraseMethodVSITR : public CEraseMethodBase
{
public:
     virtual void Initialize() override;
};

