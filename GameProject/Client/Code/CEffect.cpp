#include "pch.h"
#include "CEffect.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CManagement.h"
#include "CParticle_Sphere.h"
#include "CParticle_Rectangle.h"
#include <ctime>
#include "CTrail.h"
#include "CBullet_Trail.h"
#include "CArrow_Effect.h"
#include "CSandburst.h"
#include "CAirbubble.h"
#include "CPlayerCamera.h"
#include "CClientCameraMgr.h"

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
    if (!m_pBillBoardCamera)
    {
        m_pBillBoardCamera = dynamic_cast<CPlayerCamera*>(CClientCameraMgr::GetInstance()->Find_Camera(CLIENT_CAMERA_TYPE::PLAYER));
        assert(m_pBillBoardCamera);
    }

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

CEffect* CEffect::Create(LPDIRECT3DDEVICE9 pGraphicDev, EFFECT_TYPE eEffect_Type, const _vec3& vPos, _float fLifeTime)
{
    CEffect* pEffect = new CEffect(pGraphicDev);
    pEffect->Set_Effect_Type(eEffect_Type);
    pEffect->Set_LifeTime(fLifeTime);

    if (FAILED(pEffect->Ready_GameObject()))
    {
        Safe_Release(pEffect);
        MSG_BOX("CEffect Create Failed");
        return nullptr;
    }
    pEffect->Set_Pos(vPos);

    return pEffect;
}

CEffect* CEffect::Create(LPDIRECT3DDEVICE9 pGraphicDev, EFFECT_TYPE eEffect_Type, CGameObject* pEffect_Owner)
{
    CEffect* pEffect = new CEffect(pGraphicDev);
    pEffect->Set_Effect_Type(eEffect_Type);
    pEffect->Set_Effect_Owner(pEffect_Owner);

    if (FAILED(pEffect->Ready_GameObject()))
    {
        Safe_Release(pEffect);
        MSG_BOX("CEffect Create Failed");
        return nullptr;
    }
    return pEffect;
}

CEffect* CEffect::Create(LPDIRECT3DDEVICE9 pGraphicDev, EFFECT_TYPE eEffect_Type, CGameObject* pEffect_Owner, _float fLifeTime)
{
    CEffect* pEffect = new CEffect(pGraphicDev);
    pEffect->Set_Effect_Type(eEffect_Type);
    pEffect->Set_Effect_Owner(pEffect_Owner);
    pEffect->Set_LifeTime(fLifeTime);

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

void CEffect::Set_Pos(const _vec3& vPos)
{
    m_pTransformCom->Set_Pos(vPos);
}

void CEffect::Ready_Effect()
{
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
    case BULLET_TRAIL:
    {
        m_fLifeTime = 100.f;

        CGameObject* pGameObject = CBullet_Trail::Create(m_pGraphicDev, static_cast<CProjectile*>(m_pEffect_Owner));
        if (nullptr == pGameObject) return;

        CScene* pScene = CManagement::GetInstance()->GetCurrentScene();
        if (FAILED(pScene->Add_GameObject(L"Bullet_Trail", pGameObject))) return;
        static_cast<CProjectile*>(m_pEffect_Owner)->Set_TrailPointer(pGameObject);
        break;
    }
    case ARROW_TRAIL:
        break;
    case SANDBURST:
    {
        m_fElapsedTime2 = 0.5f;
        break;
    }
    case SANDBURST2:
    {
        m_fLifeTime = 2.f;
        break;
    }
    case WORM_DEAD_EFFECT:
    {
        m_fLifeTime = 0.5f;
        break;
    }
    case WORM_EXPLOSION1:
    {
        m_fLifeTime = 0.5f;
        break;
    }
    case WORM_EXPLOSION2:
    {
        m_fLifeTime = 0.5f;
        break;
    }
    case AIRBUBBLE:
    {
        break;
    }
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
    {
        pGameObject = CParticle_Sphere::Create(m_pGraphicDev, vPos, CParticle_Sphere::ORANGE, 45, 7.f, 0.16f, { 0.f,0.f,0.f }, CParticle_Sphere::UP);
        if (nullptr == pGameObject) return;
        if (FAILED(pLayer->Add_GameObject(L"Effect_Sphere", pGameObject))) return;

        pGameObject = CParticle_Sphere::Create(m_pGraphicDev, vPos, CParticle_Sphere::ORANGE, 45, 5.f, 0.16f, { 0.f,0.f,0.f }, CParticle_Sphere::UP);
        if (nullptr == pGameObject) return;
        if (FAILED(pLayer->Add_GameObject(L"Effect_Sphere", pGameObject))) return;

        Set_Dead(true);
        break;
    }
    case MAGMA_TRAIL:
    {
        break;
    }
    case MAGMA_DEAD_EFFECT:
    {
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
    }
    case BOSS1_DEAD_EFFECT:
    {
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
    }
    case MAGMA_EXPLOSION1:
    {
        m_fElapsedTime3 += fTimeDelta;
        if (m_fElapsedTime3 > 0.125f && m_fElapsedTime < m_fLifeTime - 0.5f)
        {
            m_fElapsedTime3 = 0.f;

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
    }
    case MAGMA_EXPLOSION2:
    {
        pGameObject = CParticle_Sphere::Create(m_pGraphicDev, vPos, CParticle_Sphere::RED, 50, 3.f, 1.f, { 1.f,1.f,1.f }, CParticle_Sphere::DOWN);
        if (nullptr == pGameObject) return;
        if (FAILED(pLayer->Add_GameObject(L"Effect_Sphere", pGameObject))) return;

        Set_Dead(true);
        break;
    }
    case BOSS1_EXPLOSION2:
    {
        pGameObject = CParticle_Sphere::Create(m_pGraphicDev, vPos, CParticle_Sphere::PINK, 40, 6.f, 1.f, { 1.5f,1.5f,1.5f }, CParticle_Sphere::DOWN);
        if (nullptr == pGameObject) return;
        if (FAILED(pLayer->Add_GameObject(L"Effect_Sphere", pGameObject))) return;
        Set_Dead(true);
        break;
    }
    case BULLET_EFFECT:
    {
        _vec3 vVelocity;

        _float fRand1 = 0;
        _float fRand2 = 0;
        _float fRand3 = 0;

        _float fRandRed = 0;
        _float fRandGreen = 0;
        _float fRandBlue = 0;


        D3DXCOLOR eColor = {};
        for (int i = 0; i < 5; ++i)
        {
            _float fRandColor = (rand() % 101) / 100.f;

            fRandRed = 1.0f - 0.2f * fRandColor;
            fRandGreen = 1.0f - 0.1f * fRandColor;
            fRandBlue = 1.0f;

            eColor = { fRandRed , fRandGreen, fRandBlue, 1.f };

            const TBillBoardInfo& tInfo = m_pBillBoardCamera->GetBillBoardInfo();
            _vec3 vPlayerPos; vPlayerPos = tInfo.vPosition;
            _vec3 vPlayerLook; vPlayerLook = tInfo.vLook;

            D3DXVec3Normalize(&vPlayerLook, &vPlayerLook);

            fRand1 = (_float)(rand() % 128 - 64) / 64.f;
            fRand2 = (_float)(rand() % 128 - 64) / 64.f;
            fRand3 = (_float)(rand() % 128 - 64) / 64.f;

            vVelocity = { fRand1,fRand2,fRand3 };

            D3DXVec3Cross(&vVelocity, &vPlayerLook, &vVelocity);
            vVelocity = vVelocity * 1.5f;
            _float fRandScale = _float(rand() % 128 - 64) / (64.f * 64.f);

            _float fScale = 0.25f * 0.25f * 0.75f;
            _vec3 vScale = { fScale,fScale,fScale };
            _vec3 vRandScale = { fRandScale ,fRandScale ,fRandScale };
            vScale += vRandScale;

            pGameObject = CParticle_Rectangle::Create(m_pGraphicDev, vPos, vVelocity, vScale, eColor, 0.125f, CParticle_Rectangle::BULLET);
            if (nullptr == pGameObject) return;
            if (FAILED(pLayer->Add_GameObject(L"Effect_Rectangle", pGameObject))) return;
        }
        Set_Dead(true);
        break;
    }
    case BULLET_TRAIL:
    {
        Set_Dead(true);
        break;
    }
    case ARROW_TRAIL:
    {
        m_fElapsedTime2 += fTimeDelta;
        CTransform* pEffectOwnerTransformCom = static_cast<CProjectile*>(m_pEffect_Owner)->Get_Transform();
        _vec3 vPos, vLook;
        pEffectOwnerTransformCom->Get_Info(INFO_POS, &vPos);
        pEffectOwnerTransformCom->Get_Info(INFO_LOOK, &vLook);
        D3DXVec3Normalize(&vLook, &vLook);
        vPos = vPos - vLook * 0.66f;

        m_pTransformCom->Set_Pos(vPos);

        if (m_fElapsedTime2 > 0.125f && m_fElapsedTime < m_fLifeTime)
        {
            m_fElapsedTime2 = 0.f;

            pGameObject = CArrow_Effect::Create(m_pGraphicDev, vPos, vLook);

            if (nullptr == pGameObject) return;
            if (FAILED(pLayer->Add_GameObject(L"Arrow_Trail", pGameObject))) return;
        }
        if (m_fElapsedTime >= m_fLifeTime)
            Set_Dead(true);
        break;
    }
    case SANDBURST:
    {
        m_fElapsedTime2 += fTimeDelta;

        if (m_fElapsedTime2 > 0.5f)
        {
            m_fElapsedTime2 -= 0.5f;

            pGameObject = CSandburst::Create(m_pGraphicDev, vPos);

            if (nullptr == pGameObject) return;
            if (FAILED(pLayer->Add_GameObject(L"Sandburst", pGameObject))) return;
        }
        if (m_fElapsedTime >= m_fLifeTime)
            Set_Dead(true);
        break;
    }
    case SANDBURST2:
    {
		_int iRand1 = 0;
		_int iRand2 = 0;
		_int iRand3 = 0;

		D3DXCOLOR eColor = { 1.f,1.f,0.f,1.f };

		_vec3 vPos, vDir, vScale;
		for (int i = 0; i < 128; ++i)
		{
			m_pTransformCom->Get_Info(INFO_POS, &vPos);
			vPos.y = 0.5f;

            vDir.x = (_float)(rand() % 128 - 64) / 64.f;
            vDir.y = (_float)(rand() % 128 - 64) / 64.f;
			//vDir.y = (_float)(rand() % 64) / 64.f;
			vDir.z = (_float)(rand() % 128 - 64) / 64.f;
			//D3DXVec3Normalize(&vDir, &vDir);
			vDir *= 0.5f;
			vPos += vDir;
			vDir = { 0.f,-2.5f,0.f };

            _int iRandScale = rand() % 5;
            _float fScale = 0.05f + (_float)iRandScale * 0.01f;
            vScale = { fScale,fScale,fScale };

			_int iRand = rand() % 3;
            if (iRand == 0) eColor = { 217.f / 256.f, 199.f / 256.f, 141.f / 256.f,1.f };
            else if (iRand == 1) eColor = { 232.f / 256.f, 216.f / 256.f, 179.f / 256.f,1.f };
            else eColor = { 179.f / 256.f, 148.f / 256.f, 54.f / 256.f ,1.f };

			pGameObject = CParticle_Rectangle::Create(m_pGraphicDev, vPos, vDir, vScale, eColor, 2.f, CParticle_Rectangle::SAND);
			if (nullptr == pGameObject) return;
			if (FAILED(pLayer->Add_GameObject(L"Effect_Rectangle", pGameObject))) return;
		}
		Set_Dead(true);

        break;
    }
    case WORM_DEAD_EFFECT:
    {
        m_fElapsedTime2 += fTimeDelta;

        if (m_fElapsedTime2 > 0.125f)
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
            D3DXVec3Normalize(&vRand, &vRand);
            vPos += vRand * 0.25f;

            for (int i = 0; i < 12; ++i)
            {
                //iRand1 = rand() % 128 - 64;
                //iRand2 = rand() % 128;
                //iRand3 = rand() % 128 - 64;

                //vVelocity = { _float(iRand1) / 48.f,_float(iRand2) / 96.f,_float(iRand3) / 48.f };

                iRand1 = rand() % 128 - 64;
                iRand2 = rand() % 128 - 64;
                iRand3 = rand() % 128 - 64;

                vVelocity = { _float(iRand1) / 48.f,_float(iRand2) / 48.f,_float(iRand3) / 48.f };

                //vVelocity *= 0.75f;
                vVelocity *= 1.f;

                int iRand = rand() % 3;
                if (iRand == 0) eColor.g = 245.f / 256.f;
                else if (iRand == 1) eColor.g = 235.f / 256.f;

                float fScale = 0.5f * 0.5f * 0.5f * 0.75f * 0.66f;
                _vec3 vScale = { fScale,fScale ,fScale };
                pGameObject = CParticle_Rectangle::Create(m_pGraphicDev, vPos, vVelocity, vScale, eColor, 0.5f);
                if (nullptr == pGameObject) return;
                if (FAILED(pLayer->Add_GameObject(L"Effect_Rectangle", pGameObject))) return;
            }
        }
        if (m_fElapsedTime > m_fLifeTime)
            Set_Dead(true);

        break;
    }
    case WORM_EXPLOSION1:
    {
        m_fElapsedTime3 += fTimeDelta;
        if (m_fElapsedTime3 > 0.125f*0.5f)
        {
            m_fElapsedTime3 = 0.f;

            _vec3 vVelocity;

            _int iRand1 = rand() % 128 - 64;
            _int iRand2 = rand() % 128 - 64;
            _int iRand3 = rand() % 128 - 64;

            vVelocity = { _float(iRand1) / 64.f,_float(iRand2) / 64.f,_float(iRand3) / 64.f };

            vPos += vVelocity / 6.f * 2.f;

            CParticle_Sphere::EFFECT_SPHERE_COLOR eEffect_Color;
            int iRand = rand() % 3;
            if (iRand % 3 == 0) eEffect_Color = CParticle_Sphere::RED;
            else if (iRand % 3 == 1) eEffect_Color = CParticle_Sphere::ORANGE;
            else eEffect_Color = CParticle_Sphere::YELLOW;
            //eEffect_Color = CParticle_Sphere::RED;
            pGameObject = CParticle_Sphere::Create(m_pGraphicDev, vPos, eEffect_Color, 25, 3.f, 0.125f, { 0.f,0.f,0.f }, CParticle_Sphere::UP);
            if (nullptr == pGameObject) return;
            if (FAILED(pLayer->Add_GameObject(L"Effect_Sphere", pGameObject))) return;

            pGameObject = CParticle_Sphere::Create(m_pGraphicDev, vPos, eEffect_Color, 15, 2.0f, 0.125f, { 0.f,0.f,0.f }, CParticle_Sphere::UP);
            if (nullptr == pGameObject) return;
            if (FAILED(pLayer->Add_GameObject(L"Effect_Sphere", pGameObject))) return;
        }

        if (m_fElapsedTime > m_fLifeTime)
            Set_Dead(true);
        break;
        break;
    }
    case WORM_EXPLOSION2:
    {
        pGameObject = CParticle_Sphere::Create(m_pGraphicDev, vPos, CParticle_Sphere::YELLOWOCHER, 45, 5.f, 0.5f, { 0.75f,0.75f,0.75f }, CParticle_Sphere::DOWN);
        if (nullptr == pGameObject) return;
        if (FAILED(pLayer->Add_GameObject(L"Effect_Sphere", pGameObject))) return;
        Set_Dead(true);
        break;
    }
    case AIRBUBBLE:
    {
        _int iRand = rand() % 3;
        for (int i = 0; i < 6 + iRand; ++i)
        {
            _float fRand1 = (_float)(rand() % 128 - 64) / (64.f * 4.f);
            _float fRand2 = (_float)(rand() % 128 - 64) / (64.f * 4.f);
            _float fRand3 = (_float)(rand() % 128 - 64) / (64.f * 4.f);
            _vec3 vRand{ fRand1, fRand2, fRand3 };

            _vec3 vPos; m_pTransformCom->Get_Info(INFO_POS, &vPos);
            vPos += vRand;

            //_int iRand = rand() % 2;
            _int iType = 0;
            if (i < 4)iType = 1;

            pGameObject = CAirbubble::Create(m_pGraphicDev, vPos, iType);
            if (nullptr == pGameObject) return;
            if (FAILED(pLayer->Add_GameObject(L"Effect_Rectangle", pGameObject))) return;
        }
        Set_Dead(true);
        break;
    }
    }
}
