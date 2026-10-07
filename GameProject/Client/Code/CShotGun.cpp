#include "pch.h"
#include "CShotGun.h"
#include "CImGuiTool.h"
#include "CProtoMgr.h"
#include "CSGBullet.h"
#include "CSoundMgr.h"
#include "CManagement.h"
#include "CRoomLayer.h"
#include "CRandomMgr.h"
#include "CRenderer.h"
#include "CWeaponSystem.h"

CShotGun::CShotGun(LPDIRECT3DDEVICE9 pGraphicDev)
    : CWeapon(pGraphicDev)
{
    D3DXMatrixIdentity(&m_matWorldCached);
}

CShotGun::~CShotGun()
{
}

HRESULT CShotGun::Ready_GameObject()
{
    if (FAILED(CWeapon::Ready_GameObject()))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;
    
    m_fSpecialAtkInterval = 0.5f;

    /* 궁극기 연출용이라 3인칭 고정 */
    m_pUltRenderingTransform->SetUseLocal(true);
    m_pUltRenderingTransform->Set_Scale(m_tLocalTView.vScale);
    m_pUltRenderingTransform->Set_Rotation_Raw(m_tLocalTView.vRotation);
    m_pUltRenderingTransform->Set_Pos(m_tLocalTView.vPosition);

    return S_OK;
}

_int CShotGun::Update_GameObject(_float fTimeDelta)
{
    _int iExit = CWeapon::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

    UpdateUltimateAttackStatus(fTimeDelta);

    return iExit;
}

void CShotGun::UpdateUltimateAttackStatus(Engine::_float fTimeDelta)
{
    if (!m_bOnUltimateAttack) return;

    m_fLeftUltimateTime -= fTimeDelta;
    
    if (m_fLeftUltimateTime >= 0.f)
    {
        m_fTimeAfterSingleShot += fTimeDelta;

        if (m_fTimeAfterSingleShot >= m_fUltimateShotInterval)
        {
            ShotSGBullet();
            m_fTimeAfterSingleShot -= CRandomMgr::GetInstance()->ApplyRandomNoise<float>(m_fUltimateShotInterval, 0.3f);
            m_bRHandShotOrder = !m_bRHandShotOrder;
        }
    }
    else
    {
        m_fLeftUltimateTime = 0.f;
        m_bOnUltimateAttack = false;
    }
}

void CShotGun::LateUpdate_GameObject(_float fTimeDelta)
{
    CWeapon::LateUpdate_GameObject(fTimeDelta);

    m_matWorldCached = *m_pTransformCom->Get_World();
}

void CShotGun::Render_GameObject()
{
    if (m_bSpecialAttackSwitchOn)
    {
        m_pTextureCom->Set_Texture(1);
    }
    else
    {
        m_pTextureCom->Set_Texture(0);
    }

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pBufferCom->Render_Buffer();
    
    if (m_bOnUltimateAttack)
    {
        m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pUltRenderingTransform->Get_World());
        m_pBufferCom->Render_Buffer();
    }

}

TWeaponOutput CShotGun::SpecialAttack(EInputState ePri, EInputState eSec)
{
    switch (ePri)
    {
    case EInputState::Held:
    {
        ShotSGBullet();
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

TWeaponOutput CShotGun::StartUltimateAttack(EInputState ePri, EInputState eSec)
{
    m_bOnUltimateAttack = true;
    m_fLeftUltimateTime = m_fUltimateTime;

    return { true, EWeaponAnimEvent::ULT_SHOTGUN };
}

TWeaponOutput CShotGun::UpdateUltimateAttack(EInputState ePri, EInputState eSec)
{
    return { false, EWeaponAnimEvent::NONE };
}

TWeaponOutput CShotGun::EndUltimateAttack(EInputState ePri, EInputState eSec)
{
    m_bOnUltimateAttack = false;
    m_fLeftUltimateTime = 0.f;
    m_pSystem->SetUltimateAttackOnGoing(false);

    return { false, EWeaponAnimEvent::NONE };
}

void CShotGun::ShotSGBullet()
{
    for (int i = 0; i < m_iBulletPerSpecialAtk; ++i)
    {
        float NoiseX = CRandomMgr::GetInstance()->GetRandomValue<float>(-10.f, 10.f);
        float NoiseY = CRandomMgr::GetInstance()->GetRandomValue<float>(-10.f, 10.f);
        _vec3 vecNoiseLocal{ NoiseX, NoiseY, 0.f };
        _vec3 vecNoiseWorld;

        D3DXVec3TransformNormal(&vecNoiseWorld, &vecNoiseLocal, &m_matWorldCached);

        _vec3 vDir, vPos;
        if (m_bOnUltimateAttack)
        {
            _matrix* matWorld = (m_bRHandShotOrder) ? m_pTransformCom->Get_World() : m_pUltRenderingTransform->Get_World();
            _vec3 vLook;
            memcpy(&vLook, &matWorld->m[2][0], sizeof(_vec3));
            D3DXVec3Normalize(&vLook, &vLook);
            vDir = vecNoiseWorld + m_fTargetDistance * vLook;

            _vec3 vLHandStart;
            D3DXVec3TransformCoord(&vLHandStart, &m_vMuzzlePositionLocal, m_pUltRenderingTransform->Get_World());
            vPos = (m_bRHandShotOrder) ? m_vBulletFrom : vLHandStart;
        }
        else
        {
            vPos = m_vBulletFrom;
            vDir = vecNoiseWorld + m_vBulletTo - m_vBulletFrom;
        }
        D3DXVec3Normalize(&vDir, &vDir);

        float fNoiseScale = CRandomMgr::GetInstance()->GetRandomValue<float>(1.f, 3.f);

        CProjectile* pProjectile = CSGBullet::Create(m_pGraphicDev, vPos, vDir, fNoiseScale);
        CScene* pScene = CManagement::GetInstance()->GetCurrentScene();
        pScene->Add_GameObject(L"Projectile_" + to_wstring(pProjectile->GetProjectileID()), pProjectile);

    }

    CSoundMgr::GetInstance()->PlaySFX(L"sfxBullet.wav");
    CSoundMgr::GetInstance()->PlaySFX(L"sfxBullet.wav");
    CSoundMgr::GetInstance()->PlaySFX(L"sfxBullet.wav");

    StartShotAnimation();
}

HRESULT CShotGun::Add_Component()
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

    // Transform
    m_pUltRenderingTransform = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == m_pUltRenderingTransform)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_TransformUlt", m_pUltRenderingTransform });

    return S_OK;
}

CShotGun* CShotGun::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CShotGun* pGun = new CShotGun(pGraphicDev);

    if (FAILED(pGun->Ready_GameObject()))
    {
        Safe_Release(pGun);
        MSG_BOX("CShotGun Create Failed");
        return nullptr;
    }

    return pGun;
}

void CShotGun::Free()
{
    CWeapon::Free();
}
