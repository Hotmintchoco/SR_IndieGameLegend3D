#include "pch.h"
#include "CCubeNetPlank.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CCollisionMgr.h"
#include "CLiminalFrame.h"
#include "CPlayer.h"
#include "CManagement.h"
#include "CRayCaster.h"

CCubeNetPlank::CCubeNetPlank(LPDIRECT3DDEVICE9 pGraphicDev)
    : CCubeNet(pGraphicDev)
{
}

CCubeNetPlank::~CCubeNetPlank()
{
}

HRESULT CCubeNetPlank::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    if (FAILED(CCubeNet::Ready_GameObject()))
        return E_FAIL;

    m_pOutlineTransform->SetUseLocal(true);
    m_pOutlineTransform->Set_Scale(1.2f, 1.2f, 1.2f);

    m_pColliderCom->Set_Extents(_vec3{ 1.0f, 1.0f, 1.0f });
    m_pColliderCom->Set_DiffPos(_vec3{ 0.0f, 0.5f, 0.0f });

    return S_OK;
}

_int CCubeNetPlank::Update_GameObject(_float fTimeDelta)
{
    _int iExit = CCubeNet::Update_GameObject(fTimeDelta);

    CCollisionMgr::GetInstance()->Add_Collider(COLL_ROOMLOGIC, m_pColliderCom);

    _matrix matWorld = *m_pTransformCom->Get_World();
    m_pOutlineTransform->WorldMatrixPropagation(matWorld);

    return iExit;
}

void CCubeNetPlank::LateUpdate_GameObject(_float fTimeDelta)
{
    CCubeNet::LateUpdate_GameObject(fTimeDelta);
}

void CCubeNetPlank::Render_GameObject()
{
    if (m_bIsPlayerOnGrabRange && !m_bIsGrabbed)
    {
        m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pOutlineTransform->Get_World());
        m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

        m_pOutlineTexture->Set_Texture(100);
        m_pBufferCom->Render_Buffer();

        m_pGraphicDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    }

    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pTextureCom->Set_Texture(0);
    m_pBufferCom->Render_Buffer();
}

void CCubeNetPlank::OnCollisionEnter(COLLINFO eCollInfo)
{
    auto& [pMyCol, pOtherCol, iMyID, iOtherID] = eCollInfo;

    switch (iOtherID)
    {
    case COLLISIONID::COLL_PLAYER:
    {
        CPlayer* pPlayer = dynamic_cast<CPlayer*>(pOtherCol->Get_Owner());
        if (pPlayer->IsGrabbingOrthoObject()) break;
        if (pPlayer) pPlayer->UpdateOrthoObjectCandidate(this);
        m_bIsPlayerOnGrabRange = true;
        break;
    }
    default:
        break;
    }
}

void CCubeNetPlank::OnCollisionStay(COLLINFO eCollInfo)
{
    // auto& [pMyCol, pOtherCol, iMyID, iOtherID] = eCollInfo;
    // 
    // switch (iOtherID)
    // {
    // case COLLISIONID::COLL_PLAYER:
    // {
    //     Obstacle_Collision(eCollInfo.pOtherCollider, m_pColliderCom);
    //     break;
    // }
    // default:
    //     break;
    // }
}

void CCubeNetPlank::OnCollisionExit(COLLINFO eCollInfo)
{
    auto& [pMyCol, pOtherCol, iMyID, iOtherID] = eCollInfo;

    switch (iOtherID)
    {
    case COLLISIONID::COLL_PLAYER:
    {
        m_bIsPlayerOnGrabRange = false;
        break;
    }
    default:
        break;
    }
}

float CCubeNetPlank::SampleTerrainHeight(const _vec3& vRayStart)
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


HRESULT CCubeNetPlank::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Transform
    pComponent = m_pOutlineTransform = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_OutlineTransform", pComponent });

    // Texture
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_CubeNet_Plank_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Texture", pComponent });

    // Outline Texture
    pComponent = m_pOutlineTexture = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Red_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_OutlineTexture", pComponent });

    return S_OK;
}

CCubeNetPlank* CCubeNetPlank::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CCubeNetPlank* pObject = new CCubeNetPlank(pGraphicDev);

    if (FAILED(pObject->Ready_GameObject()))
    {
        Safe_Release(pObject);
        MSG_BOX("CCubeNetPlank Create Failed");
        return nullptr;
    }

    return pObject;
}

void CCubeNetPlank::Free()
{
    CCubeNet::Free();
}