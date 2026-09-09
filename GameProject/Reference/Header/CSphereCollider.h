#pragma once
#include "CCollider.h"
class CSphereCollider : public CCollider
{
protected:
	explicit CSphereCollider();
	explicit CSphereCollider(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CSphereCollider(const CSphereCollider& rhs);
	virtual ~CSphereCollider();

public:
	_bool Intersect(CCollider* pOther) override;

public:
	BoundingSphere m_tSphere;

public:
	static CCollider* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	CComponent* Clone() override;
};

