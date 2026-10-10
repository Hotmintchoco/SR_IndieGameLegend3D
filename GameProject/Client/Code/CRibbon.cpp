#include "pch.h"
#include "CRibbon.h"
#include "CProtoMgr.h"
#include "CRibbonBuffer.h"
#include "CRenderer.h"
#include "CCollisionMgr.h"
#include "Client_Enum.h"
#include "CManagement.h"
#include "CClientCameraMgr.h"
#include "CCamera.h"
#include "CRoomLayer.h"
#include "CCinematicCamera.h"
#include "CDInputMgr.h"
#include "CTimerMgr.h"
#include "CFrustumExplodeEffect.h"
#include "CExplodeRange.h"
#include "CExplodeSphere.h"
#include "CRandomMgr.h"
#include "CSoundMgr.h"

CRibbon::CRibbon(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vStart, const _vec3& vDir)
    : CProjectile(pGraphicDev)
{
    m_vStart = vStart;
    m_vDir = vDir;
}

CRibbon::~CRibbon()
{
}

HRESULT CRibbon::Ready_GameObject()
{
    if (FAILED(CProjectile::Ready_GameObject()))
        return E_FAIL;

    if (FAILED(Add_Component()))
        return E_FAIL;

    m_pData = &s_tData;

    InitializeWorldMatrix();

    m_pColliderCom->Set_Owner(this);
    m_pColliderCom->Set_Radius(0.3f);

    m_vPrevPos = m_vStart;

    return S_OK;
}

void CRibbon::InitializeWorldMatrix()
{
    _vec3 vLook = m_vDir;
    D3DXVec3Normalize(&vLook, &vLook);

    _vec3 vTempUp{ 0.f, 1.f, 0.f };
    if (fabsf(D3DXVec3Dot(&vLook, &vTempUp)) > 0.999f)
        vTempUp = _vec3{ 0.f, 0.f, 1.f };

    _vec3 vRight;
    D3DXVec3Cross(&vRight, &vTempUp, &vLook);
    D3DXVec3Normalize(&vRight, &vRight);

    _vec3 vUp;
    D3DXVec3Cross(&vUp, &vLook, &vRight);
    D3DXVec3Normalize(&vUp, &vUp);

    _matrix matWorld;
    D3DXMatrixIdentity(&matWorld);
    memcpy(&matWorld.m[0][0], &vRight, sizeof(_vec3));
    memcpy(&matWorld.m[1][0], &vUp, sizeof(_vec3));
    memcpy(&matWorld.m[2][0], &vLook, sizeof(_vec3));
    memcpy(&matWorld.m[3][0], &m_vStart, sizeof(_vec3));

    m_pTransformCom->Set_World(&matWorld);
    m_pTransformCom->WorldMatrixDecompose();
}

_int CRibbon::Update_GameObject(_float fTimeDelta)
{
    fTimeDelta = CTimerMgr::GetInstance()->GetGroupTimeDelta(CTG_WEAPON);

    _int iExit = CProjectile::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHATEST, this);
    CCollisionMgr::GetInstance()->Add_Collider(COLL_PROJECTILE, m_pColliderCom);

    if (!m_bOnExplosionPhase)
    {
        UpdateInput(fTimeDelta);
        UpdateCameraShot(fTimeDelta);
        UpdateBuffer();
    }
    else
    {
        UpdateExplosionPhase(fTimeDelta);
    }

    m_vPrevPos = m_pTransformCom->Get_Info_Value(INFO_POS);

    return iExit;
}

void CRibbon::LateUpdate_GameObject(_float fTimeDelta)
{
    fTimeDelta = CTimerMgr::GetInstance()->GetGroupTimeDelta(CTG_WEAPON);

    CProjectile::LateUpdate_GameObject(fTimeDelta);
}

void CRibbon::Render_GameObject()
{
    _matrix matIdentity;
    D3DXMatrixIdentity(&matIdentity);

    m_pGraphicDev->SetTransform(D3DTS_WORLD, &matIdentity);
    
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pTextureCom->Set_Texture(0);

    m_pBufferCom->Render_Buffer();

    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

void CRibbon::OnCollisionEnter(COLLINFO eCollInfo)
{
    CProjectile::OnCollisionEnter(eCollInfo);

    // auto& [pMyCol, pOtherCol, iMyID, iOtherID] = eCollInfo;
    // auto pObject = eCollInfo.pOtherCollider->Get_Owner();
    // 
    // switch (iOtherID)
    // {
    // default:
    //     break;
    // }
}

void CRibbon::StartExplosionPhase()
{
    m_bOnExplosionPhase = true;
    m_fTraveled = 0.f;

    CCinematicCamera* pCamera = dynamic_cast<CCinematicCamera*>(
        CClientCameraMgr::GetInstance()->Find_Camera(CLIENT_CAMERA_TYPE::CINEMATIC));

    if (pCamera)
        pCamera->Stop();

    if (CLayer* pUILayer = CManagement::GetInstance()->Get_Layer(L"UI_Layer"))
        pUILayer->Set_IsActive(true);

}

HRESULT CRibbon::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Mesh
    pComponent = m_pBufferCom = CRibbonBuffer::Create(m_pGraphicDev);

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Laser_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    // Collider
    pComponent = m_pColliderCom = dynamic_cast<CSphereCollider*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_SphereCollider"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

    return S_OK;
}

void CRibbon::UpdateInput(float fTimeDelta)
{
    _vec3 vLook = m_pTransformCom->Get_Info_Value(INFO_LOOK);
    D3DXVec3Normalize(&vLook, &vLook);

    const _long mouseX = CDInputMgr::GetInstance()->Get_DIMouseMove(DIMS_X);
    const _long mouseY = CDInputMgr::GetInstance()->Get_DIMouseMove(DIMS_Y);
    _vec3 vAngle = m_pTransformCom->Get_Angle();
    vAngle.x = clamp(vAngle.x + mouseY * m_fCursorSensitivity, -89.f, 89.f);
    vAngle.y = remainderf(vAngle.y + mouseX * m_fCursorSensitivity, 360.f);
    m_pTransformCom->Set_Rotation_Raw(_vec3{ vAngle.x, vAngle.y, 0.f });

    vLook = m_pTransformCom->Get_Info_Value(INFO_LOOK);
    m_pTransformCom->Move_Pos(&vLook, s_tData.fSpeed, fTimeDelta);
}

void CRibbon::UpdateCameraShot(float fTimeDelta)
{
    _vec3 vPos = m_pTransformCom->Get_Info_Value(INFO_POS);
    _vec3 vLook = m_pTransformCom->Get_Info_Value(INFO_LOOK);
    D3DXVec3Normalize(&vLook, &vLook);
    _vec3 vUp = m_pTransformCom->Get_Info_Value(INFO_UP);
    D3DXVec3Normalize(&vUp, &vUp);
    const _vec3 vWorldUp{ 0.f, 1.f, 0.f };
    _vec3 vCamEyeWorld = vPos + m_vOffset.y * vWorldUp + m_vOffset.z * vLook;

    CCinematicCamera* pCamera = dynamic_cast<CCinematicCamera*>(CClientCameraMgr::GetInstance()->Find_Camera(CLIENT_CAMERA_TYPE::CINEMATIC));
    if (pCamera)
    {
        CINEMATIC_DESC desc;

        desc.vEyeFrom = vCamEyeWorld;
        desc.vLookAt = vPos;

        /* smoothstep 역산 */
        const _float fTau = 0.1f;
        _float fAlpha = 1.f - expf(-fTimeDelta / fTau);
        _float t = 0.5f - sinf(asinf(1.f - 2.f * fAlpha) / 3.f);
        desc.vEyeTo = vCamEyeWorld + vLook * s_tData.fSpeed * fTau;
        desc.fDuration = fTimeDelta / max(t, 0.0001f);

        desc.fFovFrom = D3DXToRadian(60.f);
        desc.fFovTo = D3DXToRadian(60.f);

        if (!m_bCamFollowing)
        {
            pCamera->Replace_Shot(desc);
            pCamera->Add_Shot(desc);
            pCamera->Play();

            m_bCamFollowing = true;
        }
        else
        {
            pCamera->Replace_Shot(desc);
        }
    }
}

void CRibbon::UpdateBuffer()
{
    _vec3 vPos = m_pTransformCom->Get_Info_Value(INFO_POS);
    _vec3 vDisplacement = vPos - m_vPrevPos;
    _vec3 vSegLeft, vSegRight;
    _vec3 vRight = m_pTransformCom->Get_Info_Value(INFO_RIGHT);
    D3DXVec3Normalize(&vRight, &vRight);
    vSegLeft = vPos - m_fWidth / 2.f * vRight;
    vSegRight = vPos + m_fWidth / 2.f * vRight;

    m_fTraveledSingleSegment += D3DXVec3Length(&vDisplacement);
    m_fTraveledAfterExplodeMark += D3DXVec3Length(&vDisplacement);

    if (m_fTraveledSingleSegment >= m_fSampleIntervalLength)
    {
        m_fTraveled += m_fTraveledSingleSegment;
        m_fTraveledSingleSegment = 0.f;

        m_pBufferCom->AddSegment(vSegLeft, vSegRight, m_fTraveled);
    }

    if (m_fTraveledAfterExplodeMark >= m_fSampleExplosionLength)
    {
        m_fTraveledAfterExplodeMark = 0.f;
        m_vecExplodePoint.push_back(vPos);
    }

    m_pBufferCom->ReplaceHead(vSegLeft, vSegRight, m_fTraveled + m_fTraveledSingleSegment);
}

void CRibbon::UpdateExplosionPhase(float fTimeDelta)
{
    m_fTraveled += m_fExplodePropagationSpeed * fTimeDelta;

    if (m_fTraveled < m_fSampleExplosionLength) return;

    m_fTraveled = 0.f;
    ++m_iCurrentIndex;

    if (m_iCurrentIndex < m_iIgnoreExplosionCount) return;

    if (m_iCurrentIndex >= (int)m_vecExplodePoint.size())
    {
        m_OnExplosionPhaseEnded.Broadcast();
        Set_Dead(true);
        return;
    }

    _vec3 vPos = m_vecExplodePoint.at(m_iCurrentIndex);

    /* 폭발 효과 */
    _vec3 vPosNoise = _vec3{
    CRandomMgr::GetInstance()->GetRandomValue<float>(-0.1f, 0.1f),
    CRandomMgr::GetInstance()->GetRandomValue<float>(-0.2f, 0.2f),
    CRandomMgr::GetInstance()->GetRandomValue<float>(-0.1f, 0.1f),
    };
    const float fScaleNoise = CRandomMgr::GetInstance()->ApplyRandomNoise<float>(0.7f, 0.2f);
    CFrustumExplodeEffect* pEffect = CFrustumExplodeEffect::Create(m_pGraphicDev, vPos + vPosNoise, _vec3{ fScaleNoise, fScaleNoise, fScaleNoise });
    if (pEffect)
    {
        CManagement::GetInstance()->GetCurrentScene()->Add_GameObject(L"pEffect", pEffect);
    }

    /* 폭발 효과 (구) */
    vPosNoise = _vec3{
        CRandomMgr::GetInstance()->GetRandomValue<float>(-0.2f, 0.2f),
        CRandomMgr::GetInstance()->GetRandomValue<float>(-0.2f, 0.2f),
        CRandomMgr::GetInstance()->GetRandomValue<float>(-0.2f, 0.2f),
    };
    const float fStartScale = CRandomMgr::GetInstance()->ApplyRandomNoise<float>(0.2f, 1.f);
    const float fEndScale = CRandomMgr::GetInstance()->ApplyRandomNoise<float>(1.f + fStartScale, 0.6f);
    const float fLifeTime = CRandomMgr::GetInstance()->GetRandomValue<float>(0.05f, 0.2f);
    CExplodeSphere* pSphere = CExplodeSphere::Create(m_pGraphicDev, vPos + vPosNoise, fStartScale, fEndScale, fLifeTime);
    if (pSphere)
    {
        CManagement::GetInstance()->GetCurrentScene()->Add_GameObject(L"pSphere", pSphere);
    }

    /* 폭발 콜라이더 */
    CExplodeRange* pArea = CExplodeRange::Create(m_pGraphicDev, vPos);
    if (pArea)
    {
        CManagement::GetInstance()->GetCurrentScene()->Add_GameObject(L"pArea", pArea);
        pArea->SetScale(2.f);
        pArea->SetDelayTime(0);
        pArea->OnSwitch();
    }

    CSoundMgr::GetInstance()->PlaySFX(L"sfxExplode.wav");
}

CRibbon* CRibbon::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vStart, const _vec3& vDir)
{
    CRibbon* pProjectile = new CRibbon(pGraphicDev, vStart, vDir);

    if (FAILED(pProjectile->Ready_GameObject()))
    {
        Safe_Release(pProjectile);
        MSG_BOX("CRibbon Create Failed");
        return nullptr;
    }

    return pProjectile;
}

void CRibbon::Free()
{
    CProjectile::Free();
}
