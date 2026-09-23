#include "pch.h"
#include "CExplosiveFrustumLight.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CManagement.h"

CExplosiveFrustumLight::CExplosiveFrustumLight(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CExplosiveFrustumLight::~CExplosiveFrustumLight()
{
}

HRESULT CExplosiveFrustumLight::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    // Note : 순서에 주의
    if (FAILED(CGameObject::Ready_GameObject()))
        return E_FAIL;

    m_pTransformCom->Set_Scale(0.2f, 0.1f, 0.2f);
    m_pTransformCom->Set_Pos(0.f, 0.6f, 0.f);
  

    return S_OK;
}

_int CExplosiveFrustumLight::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);

    m_fTimeLasted += fTimeDelta;
    if (m_fTimeLasted > m_fFrameInterval)
    {
        m_fTimeLasted -= m_fFrameInterval;
        m_iTextureIndex = (m_iTextureIndex + 1) % 2;
    }

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHATEST, this);

    return iExit;
}

void CExplosiveFrustumLight::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CExplosiveFrustumLight::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    m_pTextureCom->Set_Texture(m_iTextureIndex);

    m_pBufferCom->Render_Buffer();
}

void CExplosiveFrustumLight::PropagateTransform(CTransform* pParentTransform)
{
    /* 빌보드 */
    _vec3 vPlayerPos, vItemPos;
    CTransform* pPlayerTransform = static_cast<CTransform*>(CManagement::GetInstance()->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"));
    pPlayerTransform->Get_Info(INFO_POS, &vPlayerPos);
    pParentTransform->Get_Info(INFO_POS, &vItemPos);
    _vec3 vDist = vItemPos - vPlayerPos;

    float fYaw = atan2f(vDist.x, vDist.z);

    m_pTransformCom->Set_Rotation_Raw(_vec3{ 0.f, D3DXToDegree(fYaw), 0.f });

    _matrix* pWorld = m_pTransformCom->Get_World();
    _matrix* pParentWorld;
    pParentWorld = pParentTransform->Get_World();

    _matrix matWorld;
    D3DXMatrixMultiply(&matWorld, pWorld, pParentWorld);
    m_pTransformCom->Set_World(&matWorld);

    _vec3   vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    Compute_ViewZ(&vPos);
}

HRESULT CExplosiveFrustumLight::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // RcTex
    pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BombLight_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });


    return S_OK;
}

CExplosiveFrustumLight* CExplosiveFrustumLight::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CExplosiveFrustumLight* pFrustum = new CExplosiveFrustumLight(pGraphicDev);

    if (FAILED(pFrustum->Ready_GameObject()))
    {
        Safe_Release(pFrustum);
        MSG_BOX("CExplosiveFrustumLight Create Failed");
        return nullptr;
    }

    return pFrustum;
}

void CExplosiveFrustumLight::Free()
{
    CGameObject::Free();
}
