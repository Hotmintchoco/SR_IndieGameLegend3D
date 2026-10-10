#include "pch.h"
#include "CLaserGun.h"
#include "CImGuiTool.h"
#include "CProtoMgr.h"
#include "CLaser.h"
#include "CSoundMgr.h"
#include "CManagement.h"
#include "CRoomLayer.h"
#include "CRandomMgr.h"
#include "CRenderer.h"
#include "CRibbon.h"
#include "CWeaponSystem.h"
#include "CClientCameraMgr.h"
#include "CStage.h"
#include "CPlayer.h"
#include "CTimerMgr.h"
#include "CPlayerCamera.h"

CLaserGun::CLaserGun(LPDIRECT3DDEVICE9 pGraphicDev)
    : CWeapon(pGraphicDev)
{
}

CLaserGun::~CLaserGun()
{
}

HRESULT CLaserGun::Ready_GameObject()
{
    if (FAILED(CWeapon::Ready_GameObject()))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    m_fSpecialAtkInterval = 0.5f;

    return S_OK;
}

_int CLaserGun::Update_GameObject(_float fTimeDelta)
{
    fTimeDelta = CTimerMgr::GetInstance()->GetGroupTimeDelta(CTG_WEAPON);

    _int iExit = CWeapon::Update_GameObject(fTimeDelta);

    if (m_bOverlay)
    {
        CRenderer::GetInstance()->Add_RenderGroup(RENDER_OVERLAY, this);
    }
    else
    {
        CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);
    }

    return iExit;
}

void CLaserGun::LateUpdate_GameObject(_float fTimeDelta)
{
    fTimeDelta = CTimerMgr::GetInstance()->GetGroupTimeDelta(CTG_WEAPON);

    CWeapon::LateUpdate_GameObject(fTimeDelta);
}

void CLaserGun::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    if (m_bSpecialAttackSwitchOn)
    {
        m_pTextureCom->Set_Texture(1);
    }
    else
    {
        m_pTextureCom->Set_Texture(0);
    }

    m_pBufferCom->Render_Buffer();

    // CLaser::RenderEditorPanel();
}

void CLaserGun::OnExplosionPhaseEnded()
{
    EndUltimateAttack(EInputState::NONE, EInputState::NONE);
}

TWeaponOutput CLaserGun::SpecialAttack(EInputState ePri, EInputState eSec)
{
    switch (ePri)
    {
    case EInputState::Held:
    {
        ShotLaser();
        m_bIsCoolTime = true;
        m_fCoolTimeLeft = m_fSpecialAtkInterval;
        return { true, EWeaponAnimEvent::GUN_SHOT };
        break;
    }
    default:
        break;
    }

    return { false, EWeaponAnimEvent::NONE };
}

TWeaponOutput CLaserGun::StartUltimateAttack(EInputState ePri, EInputState eSec)
{
    m_pRibbon = CRibbon::Create(m_pGraphicDev, m_vBulletFrom, m_vBulletTo);
    if (m_pRibbon)
    {
        CManagement::GetInstance()->GetCurrentScene()->Add_GameObject(L"Ribbon", m_pRibbon);
        m_pRibbon->m_OnExplosionPhaseEnded.AddBinding(GetToken(), [this]() { OnExplosionPhaseEnded(); });
    }

    CStage* pStage = dynamic_cast<CStage*>(CManagement::GetInstance()->GetCurrentScene());
    if (pStage)
    {
        pStage->GetPlayer()->LockInput(PIC_MOVE);
    }

    CTimerMgr::GetInstance()->SetGlobalTimeScale(0.f);
    CTimerMgr::GetInstance()->SetGroupTimeScale(CTG_WEAPON, 1.f);
    CTimerMgr::GetInstance()->SetGroupTimeScale(CTG_CINEMATIC, 1.f);
    CTimerMgr::GetInstance()->SetGroupTimeScale(CTG_PLAYER, 1.f);

    m_bOnUltimateAttack = true;
    m_pSystem->SetUltimateAttackOnGoing(true);
    CClientCameraMgr::GetInstance()->SetPlayerCameraMode(CAMERA_MODE::THIRD_PERSON);
    CClientCameraMgr::GetInstance()->Select_Camera(CLIENT_CAMERA_TYPE::CINEMATIC);

    return { true, EWeaponAnimEvent::NONE };
}

TWeaponOutput CLaserGun::UpdateUltimateAttack(EInputState ePri, EInputState eSec)
{
    switch (ePri)
    {
    case EInputState::Released:
    {
        CStage* pStage = dynamic_cast<CStage*>(CManagement::GetInstance()->GetCurrentScene());
        if (pStage)
        {
            pStage->GetPlayer()->UnlockInput(PIC_MOVE);
        }
        CClientCameraMgr::GetInstance()->SetPlayerCameraMode(CAMERA_MODE::FIRST_PERSON);
        CClientCameraMgr::GetInstance()->Select_Camera(CLIENT_CAMERA_TYPE::PLAYER);
        m_pRibbon->StartExplosionPhase();
        return { true, EWeaponAnimEvent::NONE };
        break;
    }
    default:
        break;
    }

    return { false, EWeaponAnimEvent::NONE };
}

TWeaponOutput CLaserGun::EndUltimateAttack(EInputState ePri, EInputState eSec)
{
    CTimerMgr::GetInstance()->SetGlobalTimeScale(1.f);
    CTimerMgr::GetInstance()->ClearAllGroupTimeScale();

    m_bOnUltimateAttack = false;
    m_pSystem->SetUltimateAttackOnGoing(false);

    return { false, EWeaponAnimEvent::NONE };
}

void CLaserGun::ShotLaser()
{
    _vec3 vDir = m_vBulletTo - m_vBulletFrom;
    D3DXVec3Normalize(&vDir, &vDir);

    CProjectile* pProjectile = CLaser::Create(m_pGraphicDev, m_vBulletFrom, vDir);
    CScene* pScene = CManagement::GetInstance()->GetCurrentScene();

    pScene->Add_GameObject(L"Projectile_" + to_wstring(pProjectile->GetProjectileID()), pProjectile);

    CSoundMgr::GetInstance()->PlaySFX(L"sfxLaser.wav");

    StartShotAnimation();
}

HRESULT CLaserGun::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Mesh
    pComponent = m_pBufferCom = dynamic_cast<CPlyTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Gun_Vertex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Gun_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

CLaserGun* CLaserGun::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CLaserGun* pGun = new CLaserGun(pGraphicDev);

    if (FAILED(pGun->Ready_GameObject()))
    {
        Safe_Release(pGun);
        MSG_BOX("CLaserGun Create Failed");
        return nullptr;
    }

    return pGun;
}

void CLaserGun::Free()
{
    CWeapon::Free();
}
