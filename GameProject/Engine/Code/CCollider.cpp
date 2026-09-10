#include "CCollider.h"
#include "CGameObject.h"

CCollider::CCollider()
	: m_bIsTrigger(false), m_fRadius(0.f)
{
}

CCollider::CCollider(LPDIRECT3DDEVICE9 pGraphicDev)
	: CComponent(pGraphicDev), m_bIsTrigger(false), m_fRadius(0.f)
{
}

CCollider::CCollider(const CCollider& rhs)
	: CComponent(rhs), m_bIsTrigger(rhs.m_bIsTrigger), m_eColliderType(rhs.m_eColliderType), m_fRadius(rhs.m_fRadius)
{
}

CCollider::~CCollider()
{
}

void CCollider::Free()
{
	CComponent::Free();
}