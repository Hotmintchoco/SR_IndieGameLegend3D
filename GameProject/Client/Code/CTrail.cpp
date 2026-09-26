#include "pch.h"
#include "CTrail.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CManagement.h"
#include <ctime>

CTrail::CTrail(LPDIRECT3DDEVICE9 pGraphicDev)
    : CParticle(pGraphicDev)
{
}

CTrail::~CTrail()
{
}

HRESULT CTrail::Ready_GameObject()
{
    CParticle::Ready_GameObject();
    if (FAILED(Add_Component()))
        return E_FAIL;

    static_cast<CRcColCustom*>(m_pBufferCom)->Set_Buffer(m_vTrailPoint, m_eColor);
    return S_OK;
}

_int CTrail::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CParticle::Update_GameObject(fTimeDelta);

    //m_fElapsedTime += fTimeDelta;

    if (m_fLifeTime <= m_fElapsedTime)
        Set_Dead(true);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    return iExit;
}

void CTrail::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CParticle::LateUpdate_GameObject(fTimeDelta);

    _vec3       vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    CGameObject::Compute_ViewZ(&vPos);
}

void CTrail::Render_GameObject()
{
    CParticle::Render_GameObject();

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    m_pGraphicDev->SetTexture(0, nullptr);

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

CTrail* CTrail::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3(&vTrailPoint)[4], const _float& fLifeTime)
{
    CTrail* pTrail = new CTrail(pGraphicDev);
    pTrail->Set_TrailPoint(vTrailPoint);

    if (FAILED(pTrail->Ready_GameObject()))
    {
        Safe_Release(pTrail);
        MSG_BOX("CTrail Create Failed");
        return nullptr;
    }

    pTrail->Set_LifeTime(fLifeTime);

    return pTrail;
}

CTrail* CTrail::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3(&vTrailPoint)[4], const D3DXCOLOR(&eColor)[4], const _float& fLifeTime)
{
    CTrail* pTrail = new CTrail(pGraphicDev);
    pTrail->Set_TrailPoint(vTrailPoint);
    pTrail->Set_Color(eColor);

    if (FAILED(pTrail->Ready_GameObject()))
    {
        Safe_Release(pTrail);
        MSG_BOX("CTrail Create Failed");
        return nullptr;
    }

    pTrail->Set_LifeTime(fLifeTime);
    
    return pTrail;
}

void CTrail::Free()
{
    CParticle::Free();
}
