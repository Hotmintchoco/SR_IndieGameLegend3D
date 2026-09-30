#include "pch.h"
#include "CUIMgr.h"
#include "CUI.h"
#include "CGameStatusMgr.h"
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

void CUIMgr::SpecialAtkCheck()
{
    for (auto pUI : m_UIList[UI_SPECIAL])
    {
        _bool bSwitch = CGameStatusMgr::GetInstance()->GetSpecialAttackSwitch();
        pUI->Set_OnSwitch(bSwitch);

        if (pUI->Get_SyncSwitchToActive())
            pUI->Set_IsActive(bSwitch);

		// 특수 공격 게이지 UI 업데이트
		auto pGaugeUI = dynamic_cast<CGaugeUI*>(pUI);
        if (pGaugeUI)
        {
            _float fPercent = CGameStatusMgr::GetInstance()->GetSpecialAttackGauge();
            pGaugeUI->Set_Percent(fPercent);
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
            _float fPercent = CGameStatusMgr::GetInstance()->GetUltimateGauge();
            pGaugeUI->Set_Percent(fPercent);
		}
	}
}


void CUIMgr::Free()
{
    
}