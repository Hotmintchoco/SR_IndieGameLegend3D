#include "pch.h"
#include "CItem.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CBoxCollider.h"
#include "CPlayer.h"
#include "CCollisionMgr.h"
#include "CRoomLayer.h"
#include "CRcTex.h"
#include "CManagement.h"

CItem::CItem(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
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
    //m_pColliderCom->Set_Extents(0.2f, 0.2f, 0.2f);
    //m_pColliderCom->Set_Extents(1.f, 1.f, 1.f);
    m_pColliderCom->Set_Extents(1.5f, 1.5f, 1.5f);

    return S_OK;
}

_int CItem::Update_GameObject(_float fTimeDelta)
{
    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHATEST, this);

    CCollisionMgr::GetInstance()->Add_Collider(COLL_ITEM, m_pColliderCom);

    Drop(fTimeDelta);
    m_fLifeTime += fTimeDelta;
    if (m_fLifeTime > m_fLifeDuration - m_fBlinkDuration2)
        m_bBlinkStart = true;

    if (m_fLifeTime > m_fLifeDuration)
        Set_Dead(true);

    if (m_bBlinkStart == true)
    {
        m_fBlinkTime += fTimeDelta;
        if (m_fBlinkTime > m_fBlinkDuration)
        {
            m_bVisible = !m_bVisible;
            m_fBlinkDuration -= m_fBlinkDuration/16.f;
            m_fBlinkTime = 0.f;
        }
    }
    if (m_bAttractStart == true)
    {
        Attract_To_Player(fTimeDelta);
    }
    return iExit;
}

void CItem::LateUpdate_GameObject(_float fTimeDelta)
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

void CItem::OnCollisionEnter(COLLINFO eCollInfo)
{
    if (m_bAttractStart == true) return;

    if (eCollInfo.iOtherID == COLL_PLAYER)
    {
        //Consume();
        m_bAttractStart = true;
    }
}

void CItem::BillBoard()
{
    _vec3 vPlayerPos, vItemPos;
    CTransform* pTransform = static_cast<CTransform*>(CManagement::GetInstance()->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"));
    pTransform->Get_Info(INFO_POS, &vPlayerPos);
    m_pTransformCom->Get_Info(INFO_POS, &vItemPos);
    _vec3 vDist = vItemPos - vPlayerPos;

    float fYaw = atan2f(vDist.x, vDist.z);

    m_pTransformCom->Set_Rotation_Raw(_vec3{0.f, D3DXToDegree(fYaw), 0.f});
}

void CItem::Drop(_float fTimeDelta)
{
    if (m_bDropFinish == true)return;
    m_fDropTime += fTimeDelta;
    _vec3 vDir = {0, 1, 0};
    if (m_fDropTime < m_fDropDuration / 2.f)
    {
        _vec3 vDir = { 0.f, 1.f, 0.f };
        m_pTransformCom->Move_Pos(&vDir, 2.f, fTimeDelta);
    }
    else if (m_fDropTime < m_fDropDuration)
    {
        _vec3 vDir = { 0.f, -1.f, 0.f };
        m_pTransformCom->Move_Pos(&vDir, 2.f, fTimeDelta);
    }
    else
        m_bDropFinish = true;
}

void CItem::Attract_To_Player(const _float& fTimeDelta)
{
    m_fAttractTime += fTimeDelta;
    if (m_fAttractTime > m_fAttractDuration)
        Consume();

    _vec3 vPlayerPos, vItemPos;
    CTransform* pTransform = static_cast<CTransform*>(CManagement::GetInstance()->Get_Component(ID_DYNAMIC, L"GameLogic_Layer", L"Player", L"Com_Transform"));
    pTransform->Get_Info(INFO_POS, &vPlayerPos);
    m_pTransformCom->Get_Info(INFO_POS, &vItemPos);
    vPlayerPos.y = 0.4f;

    _vec3 vPos = vItemPos + (vPlayerPos - vItemPos) / 128.f;
    //vPos.y = vItemPos.y;
    m_pTransformCom->Set_Pos(vPos);
}

void CItem::Consume()
{
}

void CItem::Free()
{
    CGameObject::Free();
}
