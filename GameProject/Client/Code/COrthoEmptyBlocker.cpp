#include "pch.h"
#include "COrthoEmptyBlocker.h"
#include "CProtoMgr.h"
#include "CCollisionMgr.h"
#include "CBoxCollider.h"
#include "Client_Enum.h"

COrthoEmptyBlocker::COrthoEmptyBlocker(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

COrthoEmptyBlocker::~COrthoEmptyBlocker()
{
}

HRESULT COrthoEmptyBlocker::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    if (FAILED(CGameObject::Ready_GameObject()))
        return E_FAIL;

    /* 큐브넷 블럭과 같은 크기 */
    m_pCollider->Set_Extents(_vec3{ 0.25f, 0.25f, 0.25f });
    m_pCollider->Set_DiffPos(_vec3{ 0.0f, 0.25f, 0.0f });

    return S_OK;
}

_int COrthoEmptyBlocker::Update_GameObject(_float fTimeDelta)
{
    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    /* 렌더 그룹에는 넣지 않음 : 보이지 않는 벽 */
    CCollisionMgr::GetInstance()->Add_Collider(COLL_OBSTACLE, m_pCollider);

    return iExit;
}

void COrthoEmptyBlocker::LateUpdate_GameObject(_float fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void COrthoEmptyBlocker::Render_GameObject()
{
}

void COrthoEmptyBlocker::OnCollisionStay(COLLINFO eCollInfo)
{
    switch (eCollInfo.iOtherID)
    {
    case COLLISIONID::COLL_PLAYER:
        Obstacle_Collision(eCollInfo.pOtherCollider, m_pCollider);
        break;
    default:
        break;
    }
}

HRESULT COrthoEmptyBlocker::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Transform
    pComponent = m_pTransform = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // Collider
    pComponent = m_pCollider = dynamic_cast<CBoxCollider*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_BoxCollider"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });

    return S_OK;
}

COrthoEmptyBlocker* COrthoEmptyBlocker::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    COrthoEmptyBlocker* pObject = new COrthoEmptyBlocker(pGraphicDev);

    if (FAILED(pObject->Ready_GameObject()))
    {
        Safe_Release(pObject);
        MSG_BOX("COrthoEmptyBlocker Create Failed");
        return nullptr;
    }

    return pObject;
}

void COrthoEmptyBlocker::Free()
{
    CGameObject::Free();
}
