#include "pch.h"
#include "CCubeNetBreakable.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CCollisionMgr.h"
#include "CRayCaster.h"
#include "CManagement.h"
#include "Client_Enum.h"
#include "CRandomMgr.h"

namespace
{
    _vec3 Noise(float fTime, float fMagnitude, float fFrequency, int iSeedA, int iSeedB, int iSeedC)
    {
        _vec3 vNoise = {
            fMagnitude * (sinf(fTime * fFrequency * 1.0f + iSeedA) + sinf(fTime * fFrequency * 2.17f + iSeedA * 1.3f) * 0.5f
                + sinf(fTime * fFrequency * 4.31f + iSeedA * 2.1f) * 0.25f) / 1.75f / 3.f,
            fMagnitude * (sinf(fTime * fFrequency * 1.0f + iSeedB) + sinf(fTime * fFrequency * 2.17f + iSeedB * 1.3f) * 0.5f
                + sinf(fTime * fFrequency * 4.31f + iSeedB * 2.1f) * 0.25f) / 1.75f / 3.f,
            fMagnitude * (sinf(fTime * fFrequency * 1.0f + iSeedC) + sinf(fTime * fFrequency * 2.17f + iSeedC * 1.3f) * 0.5f
                + sinf(fTime * fFrequency * 4.31f + iSeedC * 2.1f) * 0.25f) / 1.75f / 3.f
        };
        return vNoise;
    }
}


CCubeNetBreakable::CCubeNetBreakable(LPDIRECT3DDEVICE9 pGraphicDev)
    : CCubeNet(pGraphicDev)
{
}

CCubeNetBreakable::~CCubeNetBreakable()
{
}

HRESULT CCubeNetBreakable::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    if (FAILED(CCubeNet::Ready_GameObject()))
        return E_FAIL;

    m_pColliderCom->Set_Extents(_vec3{ 0.25f, 0.25f, 0.25f });
    m_pColliderCom->Set_DiffPos(_vec3{ 0.0f, 0.25f, 0.0f });

    m_iSeedA = CRandomMgr::GetInstance()->GetRandomValue<int>(0, 10000);
    m_iSeedB = CRandomMgr::GetInstance()->GetRandomValue<int>(0, 10000);
    m_iSeedC = CRandomMgr::GetInstance()->GetRandomValue<int>(0, 10000);

    return S_OK;
}

_int CCubeNetBreakable::Update_GameObject(_float fTimeDelta)
{
    _int iExit = CCubeNet::Update_GameObject(fTimeDelta);

    CCollisionMgr::GetInstance()->Add_Collider(COLL_OBSTACLE, m_pColliderCom);
    CCollisionMgr::GetInstance()->Add_Collider(COLL_ROOMLOGIC, m_pColliderCom);

    if (m_bPendingDead)
    {
        m_fTimeAfterAnimation += fTimeDelta;

        if (m_fTimeAfterAnimation >= m_fAnimationTime)
        {
            m_OnDead.Broadcast(this);
            Set_Dead(true);
        }
    }

    return iExit;
}

void CCubeNetBreakable::LateUpdate_GameObject(_float fTimeDelta)
{
    CCubeNet::LateUpdate_GameObject(fTimeDelta);
}

void CCubeNetBreakable::Render_GameObject()
{
    _matrix matWorld = *m_pTransformCom->Get_World();
    if (m_bPendingDead)
    {
        _vec3 vNoise = Noise(m_fTimeAfterAnimation, m_fWiggleMagnitude, m_fWiggleFrequency, m_iSeedA, m_iSeedB, m_iSeedC);
        matWorld.m[3][0] += vNoise.x;
        matWorld.m[3][2] += vNoise.z;
    }
    m_pGraphicDev->SetTransform(D3DTS_WORLD, &matWorld);
    m_pTextureCom->Set_Texture(0);
    m_pBufferCom->Render_Buffer();
}

HRESULT CCubeNetBreakable::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_CubeNet_Breakable_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

void CCubeNetBreakable::PropagateDead()
{
    if (m_bPendingDead) return;

    if (m_pHead)
    {
        m_pHead->PropagateDead();
    }
    else
    {
        m_bPendingDead = true;
        m_fTimeAfterAnimation = 0.f;

        for (auto p : m_vecLinkedBreakable)
        {
            p->m_bPendingDead = true;
            p->m_fTimeAfterAnimation = 0.f;
        }
    }
}

void CCubeNetBreakable::OnCollisionEnter(COLLINFO eCollInfo)
{
    auto& [pMyCol, pOtherCol, iMyID, iOtherID] = eCollInfo;

    switch (iOtherID)
    {
    case COLLISIONID::COLL_FLATPLAYER:
    {
        PropagateDead();
        break;
    }
    default:
        break;
    }
}

void CCubeNetBreakable::OnCollisionStay(COLLINFO eCollInfo)
{
    auto& [pMyCol, pOtherCol, iMyID, iOtherID] = eCollInfo;

    switch (iOtherID)
    {
    case COLLISIONID::COLL_PLAYER:
    {
        Obstacle_Collision(eCollInfo.pOtherCollider, m_pColliderCom);
        break;
    }
    default:
        break;
    }
}

float CCubeNetBreakable::SampleTerrainHeight(const _vec3& vRayStart)
{
    CRayCaster* pRayCaster = static_cast<CRayCaster*>(CManagement::GetInstance()->Get_GameObject(L"GameLogic_Layer", L"RayCaster"));

    THitInfo tHit{};
    _vec3 vDir{ 0.f, 0.f, -1.f };

    pRayCaster->RayTest(tHit, vRayStart, vDir, m_pBufferCom, m_pTransformCom->Get_World());

    if (tHit.bHit)
    {
        return tHit.fHitPoint.z;
    }
    else
    {
        return -FLT_MAX;
    }
}

void CCubeNetBreakable::SetHead(CCubeNetBreakable* pHead)
{
    m_pHead = pHead;
    if (pHead) pHead->m_vecLinkedBreakable.push_back(this);
}

CCubeNetBreakable* CCubeNetBreakable::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CCubeNetBreakable* pObject = new CCubeNetBreakable(pGraphicDev);

    if (FAILED(pObject->Ready_GameObject()))
    {
        Safe_Release(pObject);
        MSG_BOX("CCubeNetBreakable Create Failed");
        return nullptr;
    }

    return pObject;
}

void CCubeNetBreakable::Free()
{
    CCubeNet::Free();
}