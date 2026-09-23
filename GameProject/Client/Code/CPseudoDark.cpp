#include "pch.h"
#include "CPseudoDark.h"
#include "CTransform.h"
#include "CRenderer.h"
#include "CProtoMgr.h"
#include "CManagement.h"

CPseudoDark::CPseudoDark(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CPseudoDark::~CPseudoDark()
{
}

HRESULT CPseudoDark::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;
    
    return S_OK;
}

_int CPseudoDark::Update_GameObject(const _float& fTimeDelta)
{
    if (!Get_IsActive()) return S_OK;

    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    return iExit;
}

void CPseudoDark::LateUpdate_GameObject(const _float& fTimeDelta)
{
    if (!Get_IsActive()) return;

    CGameObject::LateUpdate_GameObject(fTimeDelta);

    CTransform* pTransform = static_cast<CTransform*>(CManagement::GetInstance()->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"));
    _vec3 vPos, vLook, vAlphaZ;
    pTransform->Get_Info(INFO_POS, &vPos);
    pTransform->Get_Info(INFO_LOOK, &vLook);
    m_pTransformCom->Set_Pos(vPos);

    /* 가상의 위치로 알파 소팅 */
    vAlphaZ = vPos + vLook * m_fScale;
    Compute_ViewZ(&vAlphaZ);
}

void CPseudoDark::Render_GameObject()
{
    if (!Get_IsActive()) return;

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
    m_pTextureCom->Set_Texture(m_iOpacity);

    m_pBufferCom->Render_Buffer();
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    

}

void CPseudoDark::SetScale(const float fScale)
{
    if (!m_pTransformCom) return;
    m_fScale = fScale;
    m_pTransformCom->Set_Scale(fScale, fScale, fScale);
}

void CPseudoDark::SetOpacity(const int iOpacity)
{
    m_iOpacity = iOpacity;
}

HRESULT CPseudoDark::Add_Component()
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

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });


    return S_OK;
}

CPseudoDark* CPseudoDark::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CPseudoDark* pDoor = new CPseudoDark(pGraphicDev);

    if (FAILED(pDoor->Ready_GameObject()))
    {
        Safe_Release(pDoor);
        MSG_BOX("CPseudoDark Create Failed");
        return nullptr;
    }

    return pDoor;
}

void CPseudoDark::Free()
{
    CGameObject::Free();
}
