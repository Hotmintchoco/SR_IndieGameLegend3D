#include "pch.h"
#include "CTriggerBox.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CBoxCollider.h"
#include "CPlayer.h"
#include "CCollisionMgr.h"
#include "CRoomLayer.h"

CTriggerBox::CTriggerBox(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CTriggerBox::~CTriggerBox()
{
}

HRESULT CTriggerBox::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_pColliderCom->Set_Owner(this);
    m_pColliderCom->Set_Extents(0.5f, 0.5f, 0.5f);

    return S_OK;
}

_int CTriggerBox::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_NONALPHA, this);

    /* 콜라이더 update에 부모 위치 맞춰주는 기능이 있긴 한데, 안되어서 일단 수동으로 */
    _vec3 vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    m_pColliderCom->m_tBox.Center = XMFLOAT3{ vPos.x, vPos.y, vPos.z };

    CCollisionMgr::GetInstance()->Add_Collider(COLL_OBSTACLE, m_pColliderCom);

    return iExit;
}

void CTriggerBox::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CTriggerBox::Render_GameObject()
{
    m_pGraphicDev->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    m_pColliderCom->Render_DebugCube();
}

HRESULT CTriggerBox::Add_Component()
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

    return S_OK;
}

void CTriggerBox::OnCollisionEnter(CGameObject* pOther)
{
    CPlayer* pPlayer = dynamic_cast<CPlayer*>(pOther);
    if (pPlayer)
    {
        /* Box는 Ready 단계에서만 만들어지니까 Owner가 보장됨 */
        static_cast<CRoomLayer*>(m_pOwner)->OnRoomTriggerBlockCollided();
    }
}

CTriggerBox* CTriggerBox::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CTriggerBox* pTriggerBox = new CTriggerBox(pGraphicDev);

    if (FAILED(pTriggerBox->Ready_GameObject()))
    {
        Safe_Release(pTriggerBox);
        MSG_BOX("CTriggerBox Create Failed");
        return nullptr;
    }

    return pTriggerBox;
}

void CTriggerBox::Free()
{
    CGameObject::Free();
}
