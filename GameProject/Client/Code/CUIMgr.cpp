#include "pch.h"
#include "CUIMgr.h"
#include "CUI.h"
#include "CGameStatus.h"
#include "CManagement.h"
#include "CStage.h"

IMPLEMENT_SINGLETON(CUIMgr)

CUIMgr::CUIMgr()
{
}

CUIMgr::~CUIMgr()
{
    Free();
}

void CUIMgr::Update_UI()
{
    for (_uint i = 0; i < UI_END; ++i)
    {
        switch (i)
        {
        // 특수 공격과 관련된 UI 처리
        case UI_SPECIAL:
            SpecialAtkCheck();
            break;
        default:
            break;
        }
    }
}

void CUIMgr::SpecialAtkCheck()
{
    CStage* pStage = dynamic_cast<CStage*>(CManagement::GetInstance()->GetCurrentScene());
    if (!pStage) return;

    for (auto pUI : m_UIList[UI_SPECIAL])
    {
        _bool bSwitch = pStage->GetStatus()->GetSpecialAttackSwitch();
        pUI->Set_OnSwitch(bSwitch);

        if (pUI->Get_SyncSwitchToActive())
            pUI->Set_IsActive(bSwitch);
    }

}


void CUIMgr::Free()
{
    
}