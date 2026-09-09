#include "CSphereCollider.h"

CSphereCollider::CSphereCollider() 
{
	m_eColliderType = CT_SPHERE;
}

CSphereCollider::CSphereCollider(LPDIRECT3DDEVICE9 pGraphicDev)
	: CCollider(pGraphicDev)
{
	m_eColliderType = CT_SPHERE;
}

CSphereCollider::CSphereCollider(const CSphereCollider& rhs)
	: CCollider(rhs)
{
}

CSphereCollider::~CSphereCollider()
{
}

_bool CSphereCollider::Intersect(CCollider* pOther)
{
	if (pOther->Get_ColliderType() == CT_SPHERE)
	{
		CSphereCollider* pTargetSphere = static_cast<CSphereCollider*>(pOther);

		// 내 구(Sphere)와 상대방 구(Sphere)의 충돌 검사
		return m_tSphere.Intersects(pTargetSphere->m_tSphere);
	}

	return false;
}

CCollider* CSphereCollider::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	return new CSphereCollider(pGraphicDev);
}

CComponent* CSphereCollider::Clone()
{
	return new CSphereCollider(*this);
}
