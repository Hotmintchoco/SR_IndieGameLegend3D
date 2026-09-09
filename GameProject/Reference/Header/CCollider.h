#pragma once
#include "CComponent.h"

BEGIN(Engine)

class ENGINE_DLL CCollider : public CComponent
{
protected:
	enum COLLIDER_TYPE { CT_BOX, CT_SPHERE, CT_NONE };

	explicit CCollider();
	explicit CCollider(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCollider(const CCollider& rhs);
	virtual ~CCollider();

public:
	void	Set_IsTrigger(const bool& bIsTrigger) { m_bIsTrigger = bIsTrigger; }
	_bool	Get_IsTrigger() { return m_bIsTrigger; }

	COLLIDER_TYPE	Get_ColliderType() { return m_eColliderType; }

	virtual _bool	Intersect(CCollider* pOther) PURE;

public:
	virtual CComponent* Clone() PURE;

protected:
	virtual void		Free();

protected:
	_bool			m_bIsTrigger;
	COLLIDER_TYPE	m_eColliderType = CT_NONE;
	
};

END