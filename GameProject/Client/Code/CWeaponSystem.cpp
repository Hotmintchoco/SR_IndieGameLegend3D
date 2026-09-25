#include "pch.h"
#include "CWeaponSystem.h"
#include "CWeapon.h"
#include "CDefaultGun.h"
#include "CDInputMgr.h"
#include "CGameStatusMgr.h"
#include "Client_Struct.h"
#include "CLayer.h"

CWeaponSystem::CWeaponSystem(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CWeaponSystem::~CWeaponSystem()
{
}

HRESULT CWeaponSystem::Ready_GameObject()
{
    CDefaultGun* pDefault = CDefaultGun::Create(m_pGraphicDev);
    m_pOwner->Add_GameObject(L"DefaultGun", pDefault);
    
    if (pDefault)
    {
        m_vecWeapon.push_back(pDefault);
        m_iCurrentIndex = 0;
    }
    else
    {
        return E_FAIL;
    }

	return S_OK;
}

_int CWeaponSystem::Update_GameObject(const _float& fTimeDelta)
{
    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    GetKeyInput();

    return iExit;
}

void CWeaponSystem::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CWeaponSystem::Render_GameObject()
{
}


void CWeaponSystem::GetKeyInput()
{
    if (CDInputMgr::GetInstance()->Key_Down(DIK_Q))
    {
        SwitchWeaponTo((m_iCurrentIndex + 1) % (int)m_vecWeapon.size());
    }

    if (CDInputMgr::GetInstance()->Mouse_Press(DIM_LB) && !GetCurrentWeapon()->IsOnCoolTime())
    {
        if (m_bSpecialAttackSwitchOn)
        {
            GetCurrentWeapon()->SpecialAttack();
            m_fSpecialAtkGauge -= GetCurrentWeapon()->GetSpecialAtkGaugeConsume();
            m_fSpecialAtkGauge = clamp(m_fSpecialAtkGauge, 0.f, 1.f);
            if (m_fSpecialAtkGauge <= 0.f)
            {
                m_bSpecialAttackSwitchOn = false;
            }
            CGameStatusMgr::GetInstance()->SetSpecialAttackGauge(m_fSpecialAtkGauge);
        }
        else
        {
            GetCurrentWeapon()->DefaultAttack();
        }
    }

    if (CDInputMgr::GetInstance()->Key_Down(DIK_C))
    {
        if (m_bIsUltimateAttackReady)
        {
            GetCurrentWeapon()->UltimateAttack();    
            m_fUltimateAtkGauge = 0.f;
            CGameStatusMgr::GetInstance()->SetUltimateGauge(m_fUltimateAtkGauge);
            m_bIsUltimateAttackReady = false;
        }
    }

    TWeaponAnimArgs t;

    if (CDInputMgr::GetInstance()->Key_Down(DIK_F))
    {
        if (m_fSpecialAtkGauge > 0.f)
        {
            m_bSpecialAttackSwitchOn = !m_bSpecialAttackSwitchOn;
        }
    }
    t.bSpecialAtk = m_bSpecialAttackSwitchOn;

    if (CDInputMgr::GetInstance()->Key_Press(DIK_LSHIFT))
    {
        t.bSprint = true;
    }
    else
    {
        t.bSprint = false;
    }

    if (CDInputMgr::GetInstance()->Key_Press(DIK_W)
        || CDInputMgr::GetInstance()->Key_Press(DIK_A)
        || CDInputMgr::GetInstance()->Key_Press(DIK_S)
        || CDInputMgr::GetInstance()->Key_Press(DIK_D))
    {
        t.bMove = true;
    }
    else
    {
        t.bMove = false;
    }

    GetCurrentWeapon()->UpdateAnimationArgs(t);
}

void CWeaponSystem::SwitchWeaponTo(int iIndex)
{

    m_iCurrentIndex = iIndex;
}

void CWeaponSystem::GainEnergy()
{
    m_fSpecialAtkGauge += 0.1f;
    m_fSpecialAtkGauge = clamp(m_fSpecialAtkGauge, 0.f, 1.f);
    CGameStatusMgr::GetInstance()->SetSpecialAttackGauge(m_fSpecialAtkGauge);

    m_fUltimateAtkGauge += 0.1f;
    m_fUltimateAtkGauge = clamp(m_fUltimateAtkGauge, 0.f, 1.f);
    if (m_fUltimateAtkGauge == 1.f)
    {
        m_bIsUltimateAttackReady = true;
    }
    CGameStatusMgr::GetInstance()->SetUltimateGauge(m_fUltimateAtkGauge);
}

CWeaponSystem* CWeaponSystem::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CWeaponSystem* pSystem = new CWeaponSystem(pGraphicDev);

    if (FAILED(pSystem->Ready_GameObject()))
    {
        Safe_Release(pSystem);
        MSG_BOX("CWeaponSystem Create Failed");
        return nullptr;
    }

    return pSystem;
}

void CWeaponSystem::Free()
{
	CGameObject::Free();
}
