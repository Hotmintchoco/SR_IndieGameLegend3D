#include "pch.h"
#include "CExplosiveFrustumGlass.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CManagement.h"

CExplosiveFrustumGlass::CExplosiveFrustumGlass(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CExplosiveFrustumGlass::~CExplosiveFrustumGlass()
{
}

HRESULT CExplosiveFrustumGlass::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    // Note : 순서에 주의
    if (FAILED(CGameObject::Ready_GameObject()))
        return E_FAIL;

    return S_OK;
}

_int CExplosiveFrustumGlass::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    return iExit;
}

void CExplosiveFrustumGlass::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CExplosiveFrustumGlass::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    m_pTextureCom->Set_Texture(0);

    m_pBufferCom->Render_Buffer();
}

void CExplosiveFrustumGlass::PropagateTransform(CTransform* pParentTransform)
{
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

HRESULT CExplosiveFrustumGlass::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // RcTex
    pComponent = m_pBufferCom = dynamic_cast<CPlyTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_ExplosiveFrustum_Top_Vertex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_ExplosiveFrustum_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });


    return S_OK;
}

CExplosiveFrustumGlass* CExplosiveFrustumGlass::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CExplosiveFrustumGlass* pFrustum = new CExplosiveFrustumGlass(pGraphicDev);

    if (FAILED(pFrustum->Ready_GameObject()))
    {
        Safe_Release(pFrustum);
        MSG_BOX("CExplosiveFrustumGlass Create Failed");
        return nullptr;
    }

    return pFrustum;
}

void CExplosiveFrustumGlass::Free()
{
    CGameObject::Free();
}
