#pragma once

#include "Engine_Define.h"

class CUI;

class CUIMgr
{
    DECLARE_SINGLETON(CUIMgr)

private:
    explicit CUIMgr();
    virtual ~CUIMgr();

public:
    

private:
    // 특수 공격 UI vector
	vector<CUI*>    m_vecSpecialAtkUI;


private:
    void Free();
};