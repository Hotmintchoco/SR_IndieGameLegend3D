#include "CSphereCollider.h"
#include "CGameObject.h"
#include "CTransform.h"

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

_int CSphereCollider::Update_Component(const _float& fTimeDelta)
{
	CTransform* pOwnerTransformCom = dynamic_cast<CTransform*>(m_pOwner->Get_Component(ID_DYNAMIC, L"Com_Transform"));
	
	_vec3   vOwnerPos;
	pOwnerTransformCom->Get_Info(INFO_POS, &vOwnerPos);
	m_tSphere.Center = { vOwnerPos.x, vOwnerPos.y, vOwnerPos.z };

	return 0;
}

void CSphereCollider::LateUpdate_Component()
{

}

CCollider* CSphereCollider::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	return new CSphereCollider(pGraphicDev);
}

CComponent* CSphereCollider::Clone()
{
	return new CSphereCollider(*this);
}

void CSphereCollider::Set_Radius(const _float& fRadius)
{
	m_fRadius = fRadius;
	m_tSphere.Radius = fRadius;
}
