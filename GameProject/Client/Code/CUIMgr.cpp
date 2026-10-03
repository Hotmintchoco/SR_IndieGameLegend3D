#include "pch.h"
#include "CUIMgr.h"
#include "CUI.h"
#include "CGameStatus.h"
#include "CManagement.h"
#include "CStage.h"
#include "CHitCreenUI.h"

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

void CUIMgr::Update_HPUI(int iHP)
{
    const _int iSlotCount = 3;
    const _int iHpPerSlot = 4;

    for (_int i = 0; i < iSlotCount; ++i)
    {
        wstring wstrTag = L"PlayerHp_" + to_wstring(i);

        CUI* pUI = static_cast<CUI*>(CManagement::GetInstance()->Get_GameObject(L"UI_Layer", wstrTag.c_str()));
        if (nullptr == pUI)
            continue;

        _int iSlotHP = iHP - (i * iHpPerSlot);
        if (iSlotHP < 0)
            iSlotHP = 0;
        else if (iSlotHP > iHpPerSlot)
            iSlotHP = iHpPerSlot;

        pUI->Set_Texture((_uint)iSlotHP);
    }
}

void CUIMgr::RequestHitEffect()
{
    CGameObject* pHitUI = CManagement::GetInstance()->Get_GameObject(L"UI_Layer", L"HitScreen");
    if (nullptr != pHitUI)
    {
        CHitCreenUI* pHitScreen = dynamic_cast<CHitCreenUI*>(pHitUI);
        if (nullptr != pHitScreen)
            pHitScreen->Hit();
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