#include "pch.h"
#include "CItem.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CBoxCollider.h"
#include "CPlayer.h"
#include "CCollisionMgr.h"
#include "CRoomLayer.h"
#include "CRcTex.h"
#include "CCameraMgr.h"
#include "CManagement.h"

CItem::CItem(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev), m_fFrame(0.f)
{
}

CItem::~CItem()
{
}

HRESULT CItem::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    m_pColliderCom->Set_Owner(this);
    m_pColliderCom->Set_Extents(0.2f, 0.2f, 0.2f);

    return S_OK;
}

_int CItem::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    CCollisionMgr::GetInstance()->Add_Collider(COLL_ITEM, m_pColliderCom);

    CalculateAlphaZ();

    return iExit;
}

void CItem::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);

    BillBoard();
}

void CItem::Render_GameObject()
{
}

HRESULT CItem::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Buffer
    pComponent = m_pBufferCom = dynamic_cast<CRcTex*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_RcTex"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_STATIC].insert({ L"Com_Buffer", pComponent });


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

void CItem::OnCollisionEnter(CGameObject* pOther)
{
    CPlayer* pPlayer = dynamic_cast<CPlayer*>(pOther);
    if (pPlayer)
    {
        Consume();
        Set_Dead(true);
    }
}

void CItem::CalculateAlphaZ()
{
    _vec3   vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    Compute_ViewZ(&vPos);
}

void CItem::BillBoard()
{
    /* 빌보드 */
    _vec3 vPlayerPos, vItemPos;
    CTransform* pTransform = static_cast<CTransform*>(CManagement::GetInstance()->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"));
    pTransform->Get_Info(INFO_POS, &vPlayerPos);
    m_pTransformCom->Get_Info(INFO_POS, &vItemPos);
    _vec3 vDist = vItemPos - vPlayerPos;

    float fYaw = atan2f(vDist.x, vDist.z);

    m_pTransformCom->Set_Rotation_Raw(_vec3{0.f, D3DXToDegree(fYaw), 0.f});
}

void CItem::Consume()
{
}

void CItem::Free()
{
    CGameObject::Free();
}
