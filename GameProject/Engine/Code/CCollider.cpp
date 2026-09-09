#include "CCollider.h"

CCollider::CCollider()
	: m_bIsTrigger(false)
{
}

CCollider::CCollider(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev), m_bIsTrigger(false)
{
}

CCollider::CCollider(const CCollider& rhs)
	: CComponent(rhs), m_bIsTrigger(rhs.m_bIsTrigger), m_eColliderType(rhs.m_eColliderType)
{
}

CCollider::~CCollider()
{
}

void CCollider::Free()
{
	CComponent::Free();
}