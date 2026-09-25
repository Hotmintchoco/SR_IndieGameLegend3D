#include "pch.h"
#include "CEffect.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CManagement.h"
#include "CParticle_Sphere.h"
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
    Ready_Effect();


    return S_OK;
}

_int CEffect::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);

    m_fElapsedTime += fTimeDelta;
    
    Update_Effect(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHATEST, this);

    return iExit;
}

void CEffect::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);

}

void CEffect::Render_GameObject()
{
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

CEffect* CEffect::Create(LPDIRECT3DDEVICE9 pGraphicDev, EFFECT_TYPE eEffect_Type, const _vec3& vPos)
{
    CEffect* pEffect = new CEffect(pGraphicDev);
    pEffect->Set_Effect_Type(eEffect_Type);

    if (FAILED(pEffect->Ready_GameObject()))
    {
        Safe_Release(pEffect);
        MSG_BOX("CEffect Create Failed");
        return nullptr;
    }

    pEffect->Set_Pos(vPos);

    return pEffect;
}
 
//CEffect* CEffect::Create(LPDIRECT3DDEVICE9 pGraphicDev, EFFECT_TYPE eEffect_Type, CGameObject* pEffect_Owner)
//{
//    CEffect* pEffect = new CEffect(pGraphicDev);
//    pEffect->Set_Effect_Type(eEffect_Type);
//    pEffect->Set_Effect_Owner(pEffect_Owner);
//
//    if (FAILED(pEffect->Ready_GameObject()))
//    {
//        Safe_Release(pEffect);
//        MSG_BOX("CEffect Create Failed");
//        return nullptr;
//    }
//
//    return pEffect;
//}

void CEffect::Free()
{
    CGameObject::Free();
}

void CEffect::Set_Pos(const _vec3& vPos)
{
    m_pTransformCom->Set_Pos(vPos);
}

void CEffect::Set_Scale(const _vec3& vPos)
{
    m_pTransformCom->Set_Scale(vPos);
}

void CEffect::Ready_Effect()
{
    _vec3 vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    switch (m_eEffect_Type)
    {
    case MAGMA_FIREBALL:
        //CLayer* pLayer = CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer");
        //CGameObject* pGameObject = nullptr;

        //pGameObject = CParticle_Sphere::Create(m_pGraphicDev, vPos, CParticle_Sphere::ORANGE, 45, 5.f, 0.16f, { 0.f,0.f,0.f }, CParticle_Sphere::UP);
        //if (nullptr == pGameObject) return;
        //if (FAILED(pLayer->Add_GameObject(L"Effect_Sphere", pGameObject))) return;

        //pGameObject = CParticle_Sphere::Create(m_pGraphicDev, vPos, CParticle_Sphere::ORANGE, 45, 3.f, 0.16f, { 0.f,0.f,0.f }, CParticle_Sphere::UP);
        //if (nullptr == pGameObject) return;
        //if (FAILED(pLayer->Add_GameObject(L"Effect_Sphere", pGameObject))) return;
        break;
    case MAGMA_TRAIL:
        break;
    }
}

void CEffect::Update_Effect(const _float fTimeDelta)
{
    _vec3 vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    switch (m_eEffect_Type)
    {
    case MAGMA_FIREBALL:
        CLayer* pLayer = CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer");
        CGameObject* pGameObject = nullptr;

        pGameObject = CParticle_Sphere::Create(m_pGraphicDev, vPos, CParticle_Sphere::ORANGE, 45, 5.f, 0.16f, { 0.f,0.f,0.f }, CParticle_Sphere::UP);
        if (nullptr == pGameObject) return;
        if (FAILED(pLayer->Add_GameObject(L"Effect_Sphere", pGameObject))) return;

        pGameObject = CParticle_Sphere::Create(m_pGraphicDev, vPos, CParticle_Sphere::ORANGE, 45, 3.f, 0.16f, { 0.f,0.f,0.f }, CParticle_Sphere::UP);
        if (nullptr == pGameObject) return;
        if (FAILED(pLayer->Add_GameObject(L"Effect_Sphere", pGameObject))) return;

        Set_Dead(true);
        break;

    case MAGMA_TRAIL:
        break;
    case MAGMA_EXPLOSION1:
        break;
    case MAGMA_EXPLOSION2:
        break;
    }
}
