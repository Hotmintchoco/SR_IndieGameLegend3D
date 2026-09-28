#include "pch.h"
#include "CEffect.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CManagement.h"
#include "CParticle_Sphere.h"
#include "CParticle_Rectangle.h"
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
    case MAGMA_DEAD_EFFECT:
        break;
    case MAGMA_EXPLOSION1:
        break;
    case MAGMA_EXPLOSION2:
        break;
    }
}

void CEffect::Update_Effect(const _float fTimeDelta)
{
    _vec3 vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);

    CLayer* pLayer = CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer");
    CGameObject* pGameObject = nullptr;

    switch (m_eEffect_Type)
    {
    case MAGMA_FIREBALL:
        //CLayer* pLayer = CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer");
        //CGameObject* pGameObject = nullptr;

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
    case MAGMA_DEAD_EFFECT:
        m_fElapsedTime2 += fTimeDelta;

        if (m_fElapsedTime2 > 0.25f)
        {
            m_fElapsedTime2 = 0.f;
            _vec3 vVelocity;

            _int iRand1 = 0;
            _int iRand2 = 0;
            _int iRand3 = 0;

            D3DXCOLOR eColor = { 1.f,1.f,0.f,1.f };

            for (int i = 0; i < 5; ++i)
            {
                iRand1 = rand() % 128 - 64;
                iRand2 = rand() % 128 - 64;
                iRand3 = rand() % 128 - 64;

                vVelocity = { _float(iRand1) / 64.f,_float(iRand2) / 64.f,_float(iRand3) / 64.f };

                pGameObject = CParticle_Rectangle::Create(m_pGraphicDev, vPos, vVelocity, eColor);
                if (nullptr == pGameObject) return;
                if (FAILED(pLayer->Add_GameObject(L"Effect_Rectangle", pGameObject))) return;
            }
        }
        if (m_fElapsedTime > MAGMA_DEAD_TIME)
            Set_Dead(true);

        break;
    case MAGMA_EXPLOSION1:
        m_fElapsedTime3 += fTimeDelta;
        if (m_fElapsedTime3 > 0.125f && m_fElapsedTime < MAGMA_DEAD_TIME - 0.5f)
        {
            m_fElapsedTime3 = 0.f;

            //_vec3 vPos, vVelocity;
            //m_pTransformCom->Get_Info(INFO_POS, &vPos);
            //CGameObject* pGameObject = nullptr;
            //CLayer* pLayer = CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer");

            _vec3 vVelocity;

            _int iRand1 = rand() % 128 - 64;
            _int iRand2 = rand() % 128 - 64;
            _int iRand3 = rand() % 128 - 64;

            vVelocity = { _float(iRand1) / 64.f,_float(iRand2) / 64.f,_float(iRand3) / 64.f };

            vPos += vVelocity / 3.f * 2.f;

            CParticle_Sphere::EFFECT_SPHERE_COLOR eEffect_Color;
            int iRand = rand() % 3;
            if (iRand % 3 == 0) eEffect_Color = CParticle_Sphere::RED;
            else if (iRand % 3 == 1) eEffect_Color = CParticle_Sphere::ORANGE;
            else eEffect_Color = CParticle_Sphere::YELLOW;

            pGameObject = CParticle_Sphere::Create(m_pGraphicDev, vPos, eEffect_Color, 25, 1.5f, 0.5f, { 0.f,0.f,0.f }, CParticle_Sphere::UP);
            if (nullptr == pGameObject) return;
            if (FAILED(pLayer->Add_GameObject(L"Effect_Sphere", pGameObject))) return;

            pGameObject = CParticle_Sphere::Create(m_pGraphicDev, vPos, eEffect_Color, 15, 1.0f, 0.5f, { 0.f,0.f,0.f }, CParticle_Sphere::UP);
            if (nullptr == pGameObject) return;
            if (FAILED(pLayer->Add_GameObject(L"Effect_Sphere", pGameObject))) return;
        }

        if (m_fElapsedTime > MAGMA_DEAD_TIME)
            Set_Dead(true);
        break;
    case MAGMA_EXPLOSION2:

		//_vec3 vPos;
		//m_pTransformCom->Get_Info(INFO_POS, &vPos);
		//CLayer* pLayer = CManagement::GetInstance()->Get_Layer(L"GameLogic_Layer");
		//CGameObject* pGameObject = nullptr;

		pGameObject = CParticle_Sphere::Create(m_pGraphicDev, vPos, CParticle_Sphere::RED, 50, 3.f, 1.f, { 1.f,1.f,1.f }, CParticle_Sphere::DOWN);
		if (nullptr == pGameObject) return;
		if (FAILED(pLayer->Add_GameObject(L"Effect_Sphere", pGameObject))) return;
        
        Set_Dead(true);

        break;
    }
}
