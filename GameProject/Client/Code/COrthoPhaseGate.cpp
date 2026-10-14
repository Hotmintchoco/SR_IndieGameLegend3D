#include "pch.h"
#include "COrthoPhaseGate.h"
#include "CProtoMgr.h"
#include "CBoxCollider.h"
#include "CCollisionMgr.h"
#include "CRenderer.h"
#include "COrthoRoomLayer.h"

COrthoPhaseGate::COrthoPhaseGate(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

COrthoPhaseGate::~COrthoPhaseGate()
{
}

HRESULT COrthoPhaseGate::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    if (FAILED(CGameObject::Ready_GameObject()))
        return E_FAIL;

    /* 바닥의 플랫 플레이어와 바닥 위의 플레이어가 둘 다 닿을 수 있도록 위아래로 넉넉하게 */
    m_pColliderCom->Set_Extents(_vec3{ 0.5f, 1.0f, 0.5f });
    m_pColliderCom->Set_DiffPos(_vec3{ 0.0f, 0.5f, 0.0f });

    return S_OK;
}

_int COrthoPhaseGate::Update_GameObject(_float fTimeDelta)
{
    _int iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);
    CCollisionMgr::GetInstance()->Add_Collider(COLL_ROOMLOGIC, m_pColliderCom);

    return iExit;
}

void COrthoPhaseGate::LateUpdate_GameObject(_float fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void COrthoPhaseGate::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    m_pTextureCom->Set_Texture(100);
    m_pBufferCom->Render_Buffer();
}

void COrthoPhaseGate::OnCollisionEnter(COLLINFO eCollInfo)
{
    SetOwnerSwitch(eCollInfo.iOtherID, true);
}

void COrthoPhaseGate::OnCollisionExit(COLLINFO eCollInfo)
{
    SetOwnerSwitch(eCollInfo.iOtherID, false);
}

void COrthoPhaseGate::SetOwnerSwitch(_int iOtherID, bool bFlag)
{
    COrthoRoomLayer* pRoom = dynamic_cast<COrthoRoomLayer*>(m_pOwner);
    if (nullptr == pRoom) return;

    switch (iOtherID)
    {
    case COLLISIONID::COLL_PLAYER:
        pRoom->SetPlayerOnGate(bFlag);
        break;
    case COLLISIONID::COLL_FLATPLAYER:
        pRoom->SetFlatPlayerOnGate(bFlag);
        break;
    default:
        break;
    }
}

HRESULT COrthoPhaseGate::Add_Component()
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

    // Buffer (임시)
    pComponent = m_pBufferCom = dynamic_cast<CPlyTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_CubeNet_Buffer"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Buffer", pComponent });

    // Texture (임시)
    pComponent = m_pTextureCom = dynamic_cast<CTexture*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Blue_Texture"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Texture", pComponent });

    return S_OK;
}

COrthoPhaseGate* COrthoPhaseGate::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    COrthoPhaseGate* pObject = new COrthoPhaseGate(pGraphicDev);

    if (FAILED(pObject->Ready_GameObject()))
    {
        Safe_Release(pObject);
        MSG_BOX("COrthoPhaseGate Create Failed");
        return nullptr;
    }

    return pObject;
}

void COrthoPhaseGate::Free()
{
    CGameObject::Free();
}
