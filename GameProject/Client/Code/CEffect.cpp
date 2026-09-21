#include "pch.h"
#include "CEffect.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CManagement.h"
#include <ctime>

CEffect::CEffect(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev), m_fFrame(0.f)
{
}


CEffect::~CEffect()
{
}

HRESULT CEffect::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    return S_OK;
}

_int CEffect::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    return iExit;
}

void CEffect::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);

    _vec3       vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    CGameObject::Compute_ViewZ(&vPos);
}

void CEffect::Render_GameObject()
{
    //m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    //m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    //
    //m_pTextureCom->Set_Texture((_uint)m_fFrame);

    //m_pBufferCom->Render_Buffer();
    //
    //m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CEffect::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    return S_OK;
}


CEffect* CEffect::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CEffect* pEffect = new CEffect(pGraphicDev);

    if (FAILED(pEffect->Ready_GameObject()))
    {
        Safe_Release(pEffect);
        MSG_BOX("CEffect Create Failed");
        return nullptr;
    }

    return pEffect;
}

void CEffect::Free()
{
    CGameObject::Free();
}

void CEffect::Set_Pos(_vec3 vPos)
{
    m_pTransformCom->Set_Pos(vPos);
}
void CEffect::Set_Pos(_float fX, _float fY, _float fZ)
{
    m_pTransformCom->Set_Pos(fX, fY, fZ);
}

void CEffect::Set_Scale(_vec3 vPos)
{
    m_pTransformCom->Set_Scale(vPos);
}

void CEffect::Set_Scale(_float fX, _float fY, _float fZ)
{
    m_pTransformCom->Set_Scale(fX, fY, fZ);
}