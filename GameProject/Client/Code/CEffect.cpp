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

_int CEffect::Update_GameObject(_float fTimeDelta)
{
    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);

    m_fElapsedTime += fTimeDelta;
    
    Update_Effect(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHATEST, this);

    return iExit;
}

void CEffect::LateUpdate_GameObject(_float fTimeDelta)
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
        break;
    case MAGMA_TRAIL:
        break;
    case MAGMA_DEAD_EFFECT:
        m_fLifeTime = MAGMA_DEAD_TIME;
        break;
    case MAGMA_EXPLOSION1:
        m_fLifeTime = MAGMA_DEAD_TIME;
        break;
    case MAGMA_EXPLOSION2:
        m_fLifeTime = MAGMA_DEAD_TIME;
        break;
    case BOSS1_DEAD_EFFECT:
        m_fLifeTime = BOSS1_DEAD_TIME;
        break;
    case BOSS1_EXPLOSION1:
        m_fLifeTime = BOSS1_DEAD_TIME;
        break;
    case BOSS1_EXPLOSION2:
        m_fLifeTime = BOSS1_DEAD_TIME;
        break;
    case BOSS1_SPAWN:
        m_fLifeTime = 2.f;
        break;
    case BULLET_EFFECT:
        m_fLifeTime = 1.f;
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

                int iRand = rand() % 3;
                if (iRand == 0) eColor.g = 245.f / 256.f;
                else if (iRand == 1) eColor.g = 235.f / 256.f;

                pGameObject = CParticle_Rectangle::Create(m_pGraphicDev, vPos, vVelocity, eColor);
                if (nullptr == pGameObject) return;
                if (FAILED(pLayer->Add_GameObject(L"Effect_Rectangle", pGameObject))) return;
            }
        }
        if (m_fElapsedTime > m_fLifeTime)
            Set_Dead(true);

        break;
    case BOSS1_DEAD_EFFECT:
        m_fElapsedTime2 += fTimeDelta;

        if (m_fElapsedTime2 > 0.3f && m_fElapsedTime < m_fLifeTime - 0.5f)
        {
            m_fElapsedTime2 = 0.f;
            _vec3 vVelocity;

            _int iRand1 = 0;
            _int iRand2 = 0;
            _int iRand3 = 0;

            D3DXCOLOR eColor = { 1.f,1.f,0.f,1.f };

            _vec3 vRand;
            vRand.x = (_float)(rand() % 128 - 64) / 64.f;
            vRand.y = (_float)(rand() % 128 - 64) / 64.f;
            vRand.z = (_float)(rand() % 128 - 64) / 64.f;

            vPos += vRand;

            for (int i = 0; i < 16; ++i)
            {
                iRand1 = rand() % 128 - 64;
                iRand2 = rand() % 128;
                iRand3 = rand() % 128 - 64;

                vVelocity = { _float(iRand1) / 48.f,_float(iRand2) / 96.f,_float(iRand3) / 48.f };
                vVelocity *= 2;

                int iRand = rand() % 3;
				if (iRand == 0) eColor.g = 245.f / 256.f;
				else if (iRand == 1) eColor.g = 235.f / 256.f;

                pGameObject = CParticle_Rectangle::Create(m_pGraphicDev, vPos, vVelocity, eColor, 0.5f);
                if (nullptr == pGameObject) return;
                if (FAILED(pLayer->Add_GameObject(L"Effect_Rectangle", pGameObject))) return;
            }
        }
        if (m_fElapsedTime > m_fLifeTime)
            Set_Dead(true);

        break;
    case MAGMA_EXPLOSION1:
        m_fElapsedTime3 += fTimeDelta;
        if (m_fElapsedTime3 > 0.125f && m_fElapsedTime < m_fLifeTime - 0.5f)
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

        if (m_fElapsedTime > m_fLifeTime)
            Set_Dead(true);
        break;
    case MAGMA_EXPLOSION2:
		pGameObject = CParticle_Sphere::Create(m_pGraphicDev, vPos, CParticle_Sphere::RED, 50, 3.f, 1.f, { 1.f,1.f,1.f }, CParticle_Sphere::DOWN);
		if (nullptr == pGameObject) return;
		if (FAILED(pLayer->Add_GameObject(L"Effect_Sphere", pGameObject))) return;
        
        Set_Dead(true);

        break;


    case BOSS1_EXPLOSION2:
        pGameObject = CParticle_Sphere::Create(m_pGraphicDev, vPos, CParticle_Sphere::PINK, 40, 6.f, 1.f, { 1.5f,1.5f,1.5f }, CParticle_Sphere::DOWN);
        if (nullptr == pGameObject) return;
        if (FAILED(pLayer->Add_GameObject(L"Effect_Sphere", pGameObject))) return;

        Set_Dead(true);

        break;
    case BULLET_EFFECT:

		_vec3 vVelocity;

		_float fRand1 = 0;
		_float fRand2 = 0;
        _float fRand3 = 0;

        _float fRandRed = 0;
        _float fRandGreen = 0;
        _float fRandBlue = 0;


        D3DXCOLOR eColor = {};
        for (int i = 0; i < 3; ++i)
		{
            _float fRandColor = (rand() % 101) / 100.f;

            fRandRed = 1.0f - 0.2f * fRandColor;
            fRandGreen = 1.0f - 0.1f * fRandColor;
            fRandBlue = 1.0f;

            eColor = { fRandRed , fRandGreen, fRandBlue, 1.f };

            CTransform* pPlayerTransformCom = dynamic_cast<CTransform*>(Engine::CManagement::GetInstance()
                ->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"));

            if (nullptr == pPlayerTransformCom)
                return;

            _vec3   vPlayerPos;
            pPlayerTransformCom->Get_Info(INFO_POS, &vPlayerPos);

            _vec3   vPlayerLook;
            pPlayerTransformCom->Get_Info(INFO_LOOK, &vPlayerLook);
            D3DXVec3Normalize(&vPlayerLook, &vPlayerLook);

            fRand1 = (_float)(rand() % 128 - 64) / 64.f;
            fRand2 = (_float)(rand() % 128 - 64) / 64.f;
            fRand3 = (_float)(rand() % 128 - 64) / 64.f;

            vVelocity = { fRand1,fRand2,fRand3 };

            D3DXVec3Cross(&vVelocity, &vPlayerLook, &vVelocity);

            _float fRandScale = _float(rand() % 128 - 64) / (64.f * 64.f);

            _float fScale = 0.25f * 0.25f * 0.75f;
            _vec3 vScale = { fScale,fScale,fScale };
            _vec3 vRandScale = { fRandScale ,fRandScale ,fRandScale };
            vScale += vRandScale;

			pGameObject = CParticle_Rectangle::Create(m_pGraphicDev, vPos, vVelocity, vScale, eColor, 0.25f, CParticle_Rectangle::BULLET);
			if (nullptr == pGameObject) return;
			if (FAILED(pLayer->Add_GameObject(L"Effect_Rectangle", pGameObject))) return;
		}
        
        Set_Dead(true);
        break;
    }

}
