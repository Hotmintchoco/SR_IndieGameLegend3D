#include "CBoxCollider.h"
#include "CGameObject.h"
#include "CTransform.h"
#include "CSphereCollider.h"

CBoxCollider::CBoxCollider()
{
    m_eColliderType = CT_BOX;
}

CBoxCollider::CBoxCollider(LPDIRECT3DDEVICE9 pGraphicDev)
    : CCollider(pGraphicDev)
{
    m_eColliderType = CT_BOX;
}

CBoxCollider::CBoxCollider(const CBoxCollider& rhs)
    : CCollider(rhs), m_tBox(rhs.m_tBox)
{
}

CBoxCollider::~CBoxCollider()
{
}

_bool CBoxCollider::Intersect(CCollider* pOther)
{
    if (nullptr == pOther)
        return false;

    if (pOther->Get_ColliderType() == CT_BOX)
    {
        CBoxCollider* pTargetBox = static_cast<CBoxCollider*>(pOther);
        return m_tBox.Intersects(pTargetBox->m_tBox);
    }

    if (pOther->Get_ColliderType() == CT_SPHERE)
    {
        CSphereCollider* pTargetSphere = static_cast<CSphereCollider*>(pOther);
        return m_tBox.Intersects(pTargetSphere->m_tSphere);
    }

    return false;
}

_int CBoxCollider::Update_Component(const _float& fTimeDelta)
{
    CTransform* pOwnerTransformCom = dynamic_cast<CTransform*>(m_pOwner->Get_Component(ID_DYNAMIC, L"Com_Transform"));

    _vec3 vOwnerPos;
    pOwnerTransformCom->Get_Info(INFO_POS, &vOwnerPos);
    m_tBox.Center = { vOwnerPos.x, vOwnerPos.y, vOwnerPos.z };

    if (!m_bIsPos)
        m_bIsPos = true;

    return 0;
}

void CBoxCollider::LateUpdate_Component()
{
}

void CBoxCollider::Set_Extents(const _vec3& vExtents)
{
    m_tBox.Extents = { vExtents.x, vExtents.y, vExtents.z };
}

void CBoxCollider::Set_Extents(const _float& fX, const _float& fY, const _float& fZ)
{
    m_tBox.Extents = { fX, fY, fZ };
}

void CBoxCollider::Set_Radius(const _float& fRadius)
{
    m_fRadius = fRadius;
    m_tBox.Extents = { fRadius, fRadius, fRadius };
}

CCollider* CBoxCollider::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    return new CBoxCollider(pGraphicDev);
}

CComponent* CBoxCollider::Clone()
{
    return new CBoxCollider(*this);
}