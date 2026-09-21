#include "pch.h"
#include "CExplodeRange.h"
#include "CProtoMgr.h"
#include "CRenderer.h"
#include "CManagement.h"
#include "CCollisionMgr.h"
#include "Client_Enum.h"

CExplodeRange::CExplodeRange(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev)
{
}

CExplodeRange::~CExplodeRange()
{
}

HRESULT CExplodeRange::Ready_GameObject()
{
    if (FAILED(Add_Component()))
        return E_FAIL;

    // Note : 순서에 주의
    if (FAILED(CGameObject::Ready_GameObject()))
        return E_FAIL;

    m_pColliderCom->Set_CollisionID(COLL_EXPLODERANGE);

    return S_OK;
}

_int CExplodeRange::Update_GameObject(const _float& fTimeDelta)
{
    _int    iExit = CGameObject::Update_GameObject(fTimeDelta);

    if (m_bDelayedSwitch)
    {
        m_fLeftCollisionCheckTime -= fTimeDelta;
        if (m_fLeftCollisionCheckTime <= 0.f)
        {
            CCollisionMgr::GetInstance()->Add_Collider(COLL_EXPLODERANGE, m_pColliderCom);
            Set_Dead(true);
        }
    }

    return iExit;
}

void CExplodeRange::LateUpdate_GameObject(const _float& fTimeDelta)
{
    CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CExplodeRange::Render_GameObject()
{
}

void CExplodeRange::PropagateTransform(CTransform* pParentTransform)
{
    /* 폭발 전달용 구형 콜라이더 이므로 굳이 회전, 스케일은 고려하지 않음. 부모 삭제 시 위치 업데이트 문제도 있고 */    
    _vec3 vFrustumPos;
    pParentTransform->Get_Info(INFO_POS, &vFrustumPos);
    m_pTransformCom->Set_Pos(vFrustumPos);

    _vec3   vPos;
    m_pTransformCom->Get_Info(INFO_POS, &vPos);
    Compute_ViewZ(&vPos);
}

void CExplodeRange::SetScale(const float fScale)
{
    if (!m_pColliderCom) return;

    m_pColliderCom->Set_Radius(fScale);
}

HRESULT CExplodeRange::Add_Component()
{
    CComponent* pComponent = nullptr;

    // Transform
    pComponent = m_pTransformCom = dynamic_cast<CTransform*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_Transform"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Transform", pComponent });

    // Collider
    pComponent = m_pColliderCom = dynamic_cast<CSphereCollider*>(CProtoMgr::GetInstance()->Clone_Prototype(L"Proto_SphereCollider"));

    if (nullptr == pComponent)
        return E_FAIL;

    m_mapComponent[ID_DYNAMIC].insert({ L"Com_Collider", pComponent });


    return S_OK;
}

void CExplodeRange::OnCollisionEnter(CGameObject* pOther)
{
}

CExplodeRange* CExplodeRange::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CExplodeRange* pArea = new CExplodeRange(pGraphicDev);

    if (FAILED(pArea->Ready_GameObject()))
    {
        Safe_Release(pArea);
        MSG_BOX("CExplodeRange Create Failed");
        return nullptr;
    }

    return pArea;
}

void CExplodeRange::Free()
{
    CGameObject::Free();
}
