#pragma once

#include "Engine_Define.h"

class CUI;

enum UI_TYPE
{
    UI_SPECIAL,

    UI_END
};

class CUIMgr
{
    DECLARE_SINGLETON(CUIMgr)

private:
    explicit CUIMgr();
    virtual ~CUIMgr();

public:
    void            Update_UI();

public:
    void            Add_UI(UI_TYPE eType, CUI* pUI) { m_UIList[eType].push_back(pUI); }
    void Update_HPUI(int iHP);
    void RequestHitEffect();

private:
    void            SpecialAtkCheck();

private:
    // UI list
	list<CUI*>      m_UIList[UI_END];

private:
    void Free();
};