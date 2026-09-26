#include "CCollider.h"
#include "CGameObject.h"

_uint CCollider::s_iNextColliderId = 0;

CCollider::CCollider()
	: m_bIsTrigger(false), m_bIsCollided(false), m_fRadius(0.f)
	, m_iColliderId(++s_iNextColliderId)
{
}

CCollider::CCollider(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev), m_bIsTrigger(false), m_bIsCollided(false), m_fRadius(0.f)
	, m_iColliderId(++s_iNextColliderId)
{
}

CCollider::CCollider(const CCollider& rhs)
	: CComponent(rhs), m_bIsTrigger(rhs.m_bIsTrigger), m_bIsCollided(false)
	, m_eColliderType(rhs.m_eColliderType), m_fRadius(rhs.m_fRadius)
	, m_iColliderId(++s_iNextColliderId)
{
}

CCollider::~CCollider()
{
}

void CCollider::OnCollisionEnter(CCollider* pOther)
{
	if (nullptr == m_pOwner || nullptr == pOther)
		return;

	CGameObject* pOtherOwner = pOther->Get_Owner();
	if (nullptr == pOtherOwner)
		return;

	m_pOwner->OnCollisionEnter(pOtherOwner);
}

void CCollider::OnCollisionStay(CCollider* pOther)
{
	if (nullptr == m_pOwner || nullptr == pOther)
		return;

	CGameObject* pOtherOwner = pOther->Get_Owner();
	if (nullptr == pOtherOwner)
		return;

	m_pOwner->OnCollisionStay(pOtherOwner);
}

void CCollider::OnCollisionExit(CCollider* pOther)
{
	if (nullptr == m_pOwner || nullptr == pOther)
		return;

	CGameObject* pOtherOwner = pOther->Get_Owner();
	if (nullptr == pOtherOwner)
		return;

	m_pOwner->OnCollisionExit(pOtherOwner);
}

_float CCollider::Get_ViewZ()
{
	return m_pOwner->Get_ViewZ();
}

void CCollider::Free()
{
	CComponent::Free();
}