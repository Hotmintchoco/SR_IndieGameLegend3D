#include "pch.h"
#include "CUIMgr.h"
#include "CUI.h"
#include "CGameStatus.h"
#include "CManagement.h"
#include "CStage.h"
#include "CHitCreenUI.h"
#include "CGaugeUI.h"

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
		case UI_ULTIMATE:
            UltimateCheck();
			break;
        default:
            break;
        }
    }
}

void CUIMgr::Active_Boss(_bool isFlag)
{
    for (auto pUI : m_UIList[UI_BOSS])
        pUI->Set_IsActive(isFlag);
}

void CUIMgr::Set_BossHp(_float fHp)
{
    for (auto pUI : m_UIList[UI_BOSS])
    {
        auto pGaugeUI = dynamic_cast<CGaugeUI*>(pUI);
        if (pGaugeUI)
        {
            pGaugeUI->Set_Percent(fHp);
        }
    }
}

void CUIMgr::Update_HPUI(int iHP, bool bAddTextureOffset)
{
    const _int iSlotCount = 3;
    const _int iHpPerSlot = 4;
    const _int iDamagedHpSlot = iHP / iHpPerSlot;

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

        _uint iTexture = static_cast<_uint>(iSlotHP);

        if (bAddTextureOffset && i == iDamagedHpSlot)
            iTexture += 5;

        pUI->Set_Texture(iTexture);
    }
}

void CUIMgr::RequestHitEffect()
{
    CGameObject* pHitUI = CManagement::GetInstance()->Get_GameObject(L"Screen_Layer", L"HitScreen");
    if (nullptr != pHitUI)
    {
        CHitCreenUI* pHitScreen = dynamic_cast<CHitCreenUI*>(pHitUI);
        if (nullptr != pHitScreen)
            pHitScreen->Hit();
    }
}

void CUIMgr::EnterBossScreen()
{
    CGameObject* pHitUI = CManagement::GetInstance()->Get_GameObject(L"Screen_Layer", L"HitScreen");
    if (nullptr != pHitUI)
    {
        CHitCreenUI* pHitScreen = dynamic_cast<CHitCreenUI*>(pHitUI);
        if (nullptr != pHitScreen)
            pHitScreen->EnterBossRoom();
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

		// 특수 공격 게이지 UI 업데이트
		auto pGaugeUI = dynamic_cast<CGaugeUI*>(pUI);
        if (pGaugeUI)
        {
            // 게이지 크기만큼 그리기
            pGaugeUI->Set_Percent(pStage->GetStatus()->GetSpecialAttackGauge());
		}
    }
}

void CUIMgr::UltimateCheck()
{
    for (auto pUI : m_UIList[UI_ULTIMATE])
    {
		auto pGaugeUI = dynamic_cast<CGaugeUI*>(pUI);
        if (pGaugeUI)
        {
            CScene* pScene = CManagement::GetInstance()->GetCurrentScene();

            if (CStage* pStage = dynamic_cast<CStage*>(pScene))
            {
                // 게이지 크기만큼 그리기
                pGaugeUI->Set_Percent(pStage->GetStatus()->GetUltimateGauge());
            }
		}
	}
}


void CUIMgr::Free()
{
    
}