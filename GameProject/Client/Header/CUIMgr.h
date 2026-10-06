#pragma once

#include "Engine_Define.h"

class CUI;

enum UI_TYPE
{
    UI_SPECIAL,
	UI_ULTIMATE,
    UI_BOSS,

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
    void            Active_Boss(_bool isFlag);
    void            Set_BossHp(_float fHp);
    void            Update_HPUI(int iHP, bool bAddTextureOffset);
    void            RequestHitEffect();

private:
    void            SpecialAtkCheck();
	void			UltimateCheck();

private:
    // UI list
	list<CUI*>      m_UIList[UI_END];

private:
    void Free();
};