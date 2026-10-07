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
    _int iExit = CWeapon::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

    return iExit;
}

void CLaserGun::LateUpdate_GameObject(_float fTimeDelta)
{
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
    return { false, EWeaponAnimEvent::NONE };
}

TWeaponOutput CLaserGun::UpdateUltimateAttack(EInputState ePri, EInputState eSec)
{
    return { false, EWeaponAnimEvent::NONE };
}

TWeaponOutput CLaserGun::EndUltimateAttack(EInputState ePri, EInputState eSec)
{
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
