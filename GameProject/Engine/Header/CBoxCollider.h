#pragma once
#include "CCollider.h"

BEGIN(Engine)

class CSphereCollider;

class ENGINE_DLL CBoxCollider : public CCollider
{
protected:
	explicit CBoxCollider();
	explicit CBoxCollider(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CBoxCollider(const CBoxCollider& rhs);
	virtual ~CBoxCollider();

public:
	virtual _int	Update_Component(const _float& fTimeDelta);
	virtual void	LateUpdate_Component();

	_bool			Intersect(CCollider* pOther) override;

public:
	void			Set_Extents(const _vec3& vExtents);
	void			Set_Extents(const _float& fX, const _float& fY, const _float& fZ);
	void			Set_Radius(const _float& fRadius) override;

public:
	BoundingBox		m_tBox;

public:
	static CCollider* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	CComponent* Clone() override;
};

END

