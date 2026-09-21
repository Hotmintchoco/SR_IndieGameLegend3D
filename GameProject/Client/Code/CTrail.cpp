#include "pch.h"
#include "CTrail.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CManagement.h"
#include <ctime>

CTrail::CTrail(LPDIRECT3DDEVICE9 pGraphicDev)
    : CEffect(pGraphicDev)
{
}

CTrail::CTrail(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3(&vTrailPoint)[4])
    : CEffect(pGraphicDev), m_fElapsedLifeTime(0.f), m_fLifeTime(1.f)
{
    memcpy(m_vTrailPoint, vTrailPoint, sizeof(m_vTrailPoint));
}


CTrail::~CTrail()
{
}

HRESULT CTrail::Ready_GameObject()
{
    CEffect::Ready_GameObject();
    if (FAILED(Add_Component()))
        return E_FAIL;

    D3DXCOLOR color[4];
    for (int i = 0; i < 4; ++i)
    {
        color[i] = { 1.f, 1.f, 1.f, 1.f };
        //color[i] = D3DCOLOR_ARGB(128, 255, 255, 255);
        //color[i] = { 0.f, 1.f, 0.f, 1.f };
    }
    static_cast<CRcColCustom*>(m_pBufferCom)->Set_Buffer(m_vTrailPoint, color);
    return S_OK;
}

_int CTrail::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CEffect::Update_GameObject(fTimeDelta);

    m_fElapsedLifeTime += fTimeDelta;

    if (m_fLifeTime <= m_fElapsedLifeTime)
        Set_Dead(true);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);
    //CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

    return iExit;
}

void CTrail::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CEffect::LateUpdate_GameObject(fTimeDelta);

    _vec3       vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    CGameObject::Compute_ViewZ(&vPos);


}

void CTrail::Render_GameObject()
{
    CEffect::Render_GameObject();

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    //m_pGraphicDev->SetTexture(0, nullptr);



    m_pBufferCom->Render_Buffer();




    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}

HRESULT CTrail::Add_Component()
{
    CComponent* pComponent = nullptr;

    // RcColCustom
    pComponent = m_pBufferCom = dynamic_cast<CRcColCustom*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcColCustom"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    return S_OK;
}


CTrail* CTrail::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CTrail* pTrail = new CTrail(pGraphicDev);

    if (FAILED(pTrail->Ready_GameObject()))
    {
        Safe_Release(pTrail);
        MSG_BOX("CTrail Create Failed");
        return nullptr;
    }

    return pTrail;
}

CTrail* CTrail::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3(&vTrailPoint)[4])
{
    CTrail* pTrail = new CTrail(pGraphicDev, vTrailPoint);

    if (FAILED(pTrail->Ready_GameObject()))
    {
        Safe_Release(pTrail);
        MSG_BOX("CTrail Create Failed");
        return nullptr;
    }

    return pTrail;
}

//void CTrail::Set_TrailPoint(const _vec3(&vTrailPoint)[4])
//{
//    memcpy(m_vTrailPoint, vTrailPoint, sizeof(m_vTrailPoint));
//}


void CTrail::Free()
{
    CEffect::Free();
}
