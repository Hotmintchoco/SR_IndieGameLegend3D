#include "pch.h"
#include "CFloor.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CBoxCollider.h"
#include "CPlayer.h"
#include "CCollisionMgr.h"
#include "CRoomLayer.h"
#include "CLaser.h"

CFloor::CFloor(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CFloor::~CFloor()
{
}

HRESULT CFloor::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_pColliderCom->Set_Owner(this);

    return S_OK;
}

_int CFloor::Update_GameObject(_float fTimeDelta)
{
    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);

    CCollisionMgr::GetInstance()->Add_Collider(COLL_OBSTACLE, m_pColliderCom);

    return iExit;
}

void CFloor::LateUpdate_GameObject(_float fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CFloor::Render_GameObject()
{
}

HRESULT CFloor::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // Collider
    pComponent = m_pColliderCom = dynamic_cast<CBoxCollider*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BoxCollider"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_BoxCollider", pComponent });
    // Collider

    pComponent = m_pBufferCom = dynamic_cast<CPlaneTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_PlaneTex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });

    return S_OK;
}

void CFloor::OnCollisionEnter(COLLINFO eCollInfo)
{
}

vector<pair<Engine::CVIBuffer*, Engine::CTransform*>> CFloor::GetRayTestTargetInfo()
{
    return vector <pair<Engine::CVIBuffer*, Engine::CTransform*>>{ { m_pBufferCom, m_pTransformCom } };
}

float CFloor::SampleTerrainHeight(const _vec3& vRayStart)
{
    return 0.f;
}

CFloor* CFloor::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CFloor* pTriggerBox = new CFloor(pGraphicDev);

    if (FAILED(pTriggerBox->Ready_GameObject()))
    {
        Safe_Release(pTriggerBox);
        MSG_BOX("CFloor Create Failed");
        return nullptr;
    }

    return pTriggerBox;
}

void CFloor::Free()
{
    CGameObject::Free();
}
