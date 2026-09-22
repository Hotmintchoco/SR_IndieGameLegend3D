#include "pch.h"
#include "CEffect_Sphere.h"
#include "CTransform.h"
#include "CRenderer.h"
#include "CProtoMgr.h"
#include "CManagement.h"

CEffect_Sphere::CEffect_Sphere(LPDIRECT3DDEVICE9 pGraphicDev)
    : CEffect(pGraphicDev)
{
}

CEffect_Sphere::~CEffect_Sphere()
{
}

HRESULT CEffect_Sphere::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    return S_OK;
}

_int CEffect_Sphere::Update_GameObject(const _float& fTimeDelta)
{
    //if (!Get_IsActive()) return S_OK;

    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    return iExit;
}

void CEffect_Sphere::LateUpdate_GameObject(const _float& fTimeDelta)
{
    //if (!Get_IsActive()) return;
    //////////////////////////////////////
    CGameObject::LateUpdate_GameObject(fTimeDelta);

    CTransform* pTransform = static_cast<CTransform*>(CManagement::GetInstance()->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"));
    _vec3 vPos, vLook, vAlphaZ;
    pTransform->Get_Info(INFO_POS, &vPos);
    pTransform->Get_Info(INFO_LOOK, &vLook);
    m_pTransformCom->Set_Pos(vPos);
    m_pTransformCom->ForceUpdateWorldMatrix();

    /* 가상의 위치로 알파 소팅 */
    vAlphaZ = vPos + vLook * m_fScale;
    Compute_ViewZ(&vAlphaZ);
}

void CEffect_Sphere::Render_GameObject()
{
    //if (!Get_IsActive()) return;

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
    m_pTextureCom->Set_Texture(m_iOpacity);

    m_pBufferCom->Render_Buffer();
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);


}

void CEffect_Sphere::SetScale(const float fScale)
{
    if (!m_pTransformCom) return;
    m_fScale = fScale;
    m_pTransformCom->Set_Scale(fScale, fScale, fScale);
}

void CEffect_Sphere::SetOpacity(const int iOpacity)
{
    m_iOpacity = iOpacity;
}

HRESULT CEffect_Sphere::Add_Component()
{
    CComponent* pComponent = nullptr;

    // TerrainTex
    pComponent = m_pBufferCom = dynamic_cast<CPlyTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Sphere_Vertex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Black_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

CEffect_Sphere* CEffect_Sphere::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CEffect_Sphere* pEffect_Sphere = new CEffect_Sphere(pGraphicDev);

    if (FAILED(pEffect_Sphere->Ready_GameObject()))
    {
        Safe_Release(pEffect_Sphere);
        MSG_BOX("CEffect_Sphere Create Failed");
        return nullptr;
    }

    return pEffect_Sphere;
}

void CEffect_Sphere::Free()
{
    CGameObject::Free();
}
