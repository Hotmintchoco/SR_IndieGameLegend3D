#include "CCollider.h"
#include "CGameObject.h"

CCollider::CCollider()
	: m_bIsTrigger(false), m_bIsPos(false), m_fRadius(0.f), m_bIsActive(true)
{
}

CCollider::CCollider(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev), m_bIsTrigger(false), m_bIsPos(false), m_fRadius(0.f), m_bIsActive(true)
{
}

CCollider::CCollider(const CCollider& rhs)
	: CComponent(rhs), m_bIsTrigger(rhs.m_bIsTrigger), m_bIsPos(rhs.m_bIsPos), m_eColliderType(rhs.m_eColliderType), m_fRadius(rhs.m_fRadius), m_bIsActive(rhs.m_bIsActive)
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

void CCollider::Free()
{
	CComponent::Free();
}