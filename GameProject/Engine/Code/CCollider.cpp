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

void CCollider::Free()
{
	CComponent::Free();
}