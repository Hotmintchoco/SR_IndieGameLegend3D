#include "pch.h"
#include "CCubeNetKeyBlock.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CCollisionMgr.h"
#include "CRayCaster.h"
#include "CManagement.h"
#include "Client_Enum.h"

CCubeNetKeyBlock::CCubeNetKeyBlock(LPDIRECT3DDEVICE9 pGraphicDev)
    : CCubeNet(pGraphicDev)
{
}

CCubeNetKeyBlock::~CCubeNetKeyBlock()
{
}

HRESULT CCubeNetKeyBlock::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    if (FAILED(CCubeNet::Ready_GameObject()))
        return E_FAIL;

    m_pColliderCom->Set_Extents(_vec3{ 0.25f, 0.25f, 0.25f });
    m_pColliderCom->Set_DiffPos(_vec3{ 0.0f, 0.25f, 0.0f });

    return S_OK;
}

_int CCubeNetKeyBlock::Update_GameObject(_float fTimeDelta)
{
    _int iExit = CCubeNet::Update_GameObject(fTimeDelta);

    CCollisionMgr::GetInstance()->Add_Collider(COLL_OBSTACLE, m_pColliderCom);

    return iExit;
}

void CCubeNetKeyBlock::LateUpdate_GameObject(_float fTimeDelta)
{
    CCubeNet::LateUpdate_GameObject(fTimeDelta);
}

void CCubeNetKeyBlock::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pTextureCom->Set_Texture(0);
    m_pBufferCom->Render_Buffer();
}

HRESULT CCubeNetKeyBlock::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Texture (임시 : 회색)
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Gray_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

void CCubeNetKeyBlock::OnCollisionStay(COLLINFO eCollInfo)
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

float CCubeNetKeyBlock::SampleTerrainHeight(const _vec3& vRayStart)
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

CCubeNetKeyBlock* CCubeNetKeyBlock::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CCubeNetKeyBlock* pObject = new CCubeNetKeyBlock(pGraphicDev);

    if (FAILED(pObject->Ready_GameObject()))
    {
        Safe_Release(pObject);
        MSG_BOX("CCubeNetKeyBlock Create Failed");
        return nullptr;
    }

    return pObject;
}

void CCubeNetKeyBlock::Free()
{
    CCubeNet::Free();
}
