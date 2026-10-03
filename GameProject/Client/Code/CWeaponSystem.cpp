#include "pch.h"
#include "CWeaponSystem.h"
#include "CWeapon.h"
#include "CDInputMgr.h"
#include "CGameStatus.h"
#include "CStage.h"
#include "Client_Struct.h"
#include "CLayer.h"
#include "CAbstractFactory.h"
#include "CManagement.h"
#include "CUI.h"

CWeaponSystem::CWeaponSystem(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CWeaponSystem::~CWeaponSystem()
{
}

HRESULT CWeaponSystem::Ready_GameObject()
{
    if (FAILED(AddWeapon(EObjectType::WEAPON_DEFAULT, L"RapidGun")))
        return E_FAIL;

    if (FAILED(AddWeapon(EObjectType::WEAPON_SHOTGUN, L"ShotGun")))
        return E_FAIL;
    
    if (FAILED(AddWeapon(EObjectType::WEAPON_LASERGUN, L"LaserGun")))
        return E_FAIL;
    
    if (FAILED(AddWeapon(EObjectType::WEAPON_BOW, L"Bow")))
        return E_FAIL;

    SwitchWeaponTo(0);

	return S_OK;
}

_int CWeaponSystem::Update_GameObject(_float fTimeDelta)
{
    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    return iExit;
}

void CWeaponSystem::LateUpdate_GameObject(_float fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CWeaponSystem::Render_GameObject()
{
}

TWeaponSystemOutput CWeaponSystem::UpdateInput(const TWeaponSystemInput& tInput)
{
    TWeaponSystemOutput t;

    if (tInput.bSpecialSwitchPressed)
    {
        if (m_fSpecialAtkGauge > 0.f)
        {
            m_bSpecialAttackSwitchOn = !m_bSpecialAttackSwitchOn;
        }
    }

    if (t.bAttacked = tInput.bAttack && !GetCurrentWeapon()->IsOnCoolTime())
    {
        if (m_bSpecialAttackSwitchOn)
        {
            GetCurrentWeapon()->SpecialAttack();
            // m_fSpecialAtkGauge -= GetCurrentWeapon()->GetSpecialAtkGaugeConsume();
            m_fSpecialAtkGauge = clamp(m_fSpecialAtkGauge, 0.f, 1.f);
            if (m_fSpecialAtkGauge <= 0.f)
            {
                m_bSpecialAttackSwitchOn = false;
            }

            CStage* pStage = dynamic_cast<CStage*>(CManagement::GetInstance()->GetCurrentScene());
            if (pStage)
            {
                pStage->GetStatus()->SetSpecialAttackGauge(m_fSpecialAtkGauge);
            }
        }
        else
        {
            GetCurrentWeapon()->DefaultAttack();
        }
    }

    if (tInput.bUltAttack && m_bIsUltimateAttackReady)
    {
        GetCurrentWeapon()->UltimateAttack();
        // m_fUltimateAtkGauge = 0.f;
        // m_bIsUltimateAttackReady = false;

        CStage* pStage = dynamic_cast<CStage*>(CManagement::GetInstance()->GetCurrentScene());
        if (pStage)
        {
            pStage->GetStatus()->SetUltimateGauge(m_fUltimateAtkGauge);
        }
    }

    if (CWeapon* pWeapon = GetCurrentWeapon())
    {
        if (tInput.bChargeBegin) pWeapon->ChargeStart();
        if (tInput.bChargeEnd) pWeapon->ChargeEnd();

        TWeaponAnimArgs t;
        t.bSpecialAtk = m_bSpecialAttackSwitchOn;
        t.bSprint = tInput.bSprint;
        t.bMove = tInput.bMove;
        pWeapon->UpdateAnimationArgs(t);
    }

    if (tInput.bSwitchWeapon)
    {
        SwitchWeaponTo((m_iCurrentIndex + 1) % (int)m_vecWeapon.size());
    }

    return t;
}

HRESULT CWeaponSystem::AddWeapon(EObjectType eType, const wstring& wstrName)
{
    CWeapon* pWeapon = CAbstractFactory::GetInstance()->CraeteWeapon(eType);

    if (pWeapon)
    {
        m_pOwner->Add_GameObject(wstrName, pWeapon);
        m_vecWeapon.push_back(pWeapon);
        SwitchWeaponTo((int)m_vecWeapon.size() - 1);
        return S_OK;
    }
    else
    {
        return E_FAIL;
    }
}

void CWeaponSystem::SwitchWeaponTo(int iIndex)
{
    if (iIndex < 0 || iIndex >= (int)m_vecWeapon.size()) return;

    m_vecWeapon.at(m_iCurrentIndex)->Set_IsActive(false);
    m_iCurrentIndex = iIndex;
    m_vecWeapon.at(m_iCurrentIndex)->Set_IsActive(true);

    // 정민 : 특수 공격 번호 UI 전달
    auto pUI = dynamic_cast<CUI*>(CManagement::GetInstance()->Get_GameObject(L"UI_Layer", L"SkillInfo"));
    if (pUI)
        pUI->Set_Texture(iIndex);
}

void CWeaponSystem::GainEnergy()
{
    m_fSpecialAtkGauge += 0.1f;
    m_fSpecialAtkGauge = clamp(m_fSpecialAtkGauge, 0.f, 1.f);

    m_fUltimateAtkGauge += 0.1f;
    m_fUltimateAtkGauge = clamp(m_fUltimateAtkGauge, 0.f, 1.f);
    if (m_fUltimateAtkGauge == 1.f)
    {
        m_bIsUltimateAttackReady = true;
    }

    CStage* pStage = dynamic_cast<CStage*>(CManagement::GetInstance()->GetCurrentScene());
    if (pStage)
    {
        pStage->GetStatus()->SetSpecialAttackGauge(m_fSpecialAtkGauge);
        pStage->GetStatus()->SetUltimateGauge(m_fUltimateAtkGauge);
    }
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
