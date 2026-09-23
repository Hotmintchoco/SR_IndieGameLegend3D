#include "pch.h"
#include "CFrustum.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CBoxCollider.h"
#include "CCollisionMgr.h"
#include "Client_Struct.h"
#include "Client_Enum.h"
#include "CGameStatusMgr.h"
#include "CRoomLayer.h"

CFrustum::CFrustum(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
    if (!m_pOwner) m_pOwner = CGameStatusMgr::GetInstance()->GetCurrentRoomLayer();
}

CFrustum::~CFrustum()
{
}

HRESULT CFrustum::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    /* Note : 순서에 주의 (PostInitalize로 빼는 것도 고려) */
    if (FAILED(CGameObject::Ready_GameObject()))
        return E_FAIL;

	m_pColliderCom->Set_CollisionID(COLL_OBSTACLE);

    return S_OK;
}

_int CFrustum::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);

    CCollisionMgr::GetInstance()->Add_Collider(COLL_OBSTACLE, m_pColliderCom);

    return iExit;
}

void CFrustum::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CFrustum::Render_GameObject()
{
}

void CFrustum::OnCollisionStay(CGameObject* pOther)
{
	Obstacle_Collision(pOther, m_pColliderCom);
}

HRESULT CFrustum::Add_Component()
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

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

    m_pColliderCom->Set_Extents(_vec3{0.5f, 0.4f, 0.5f});
    m_pColliderCom->Set_DiffPos(_vec3{0.f, 0.4f, 0.f});


    return S_OK;
}

_bool CFrustum::CheckDestroyCondition(CCollider* pOtherCollider)
{
    if (!pOtherCollider) return false;
    _int ColliderID = pOtherCollider->Get_CollisionID();

    switch (ColliderID)
    {
    case COLL_PBULLET:
    case COLL_MBULLET:
    case COLL_EXPLODERANGE:
        return true;
        break;
    default:
        return false;
        break;
    }
}

void CFrustum::OnRoomEvent(const TRoomEventCtx& t)
{
    switch (t.eType)
    {
    case ERoomEventType::RESET_ROOM:
        Destroy();
        break;
    default:
        break;
    }
}

void CFrustum::Free()
{
    CGameObject::Free();
}
