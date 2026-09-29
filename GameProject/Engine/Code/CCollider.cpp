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

void CCollider::OnCollisionEnter(COLLINFO eCollInfo)
{
	if (nullptr == m_pOwner || nullptr == eCollInfo.pOtherCollider)
		return;

	m_pOwner->OnCollisionEnter(eCollInfo);
}

void CCollider::OnCollisionStay(COLLINFO eCollInfo)
{
	if (nullptr == m_pOwner || nullptr == eCollInfo.pOtherCollider)
		return;

	m_pOwner->OnCollisionStay(eCollInfo);
}

void CCollider::OnCollisionExit(COLLINFO eCollInfo)
{
	if (nullptr == m_pOwner || nullptr == eCollInfo.pOtherCollider)
		return;

	m_pOwner->OnCollisionExit(eCollInfo);
}

_float CCollider::Get_ViewZ()
{
	return m_pOwner->Get_ViewZ();
}

void CCollider::Free()
{
	CComponent::Free();
}