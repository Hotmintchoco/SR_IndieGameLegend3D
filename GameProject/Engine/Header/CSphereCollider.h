#pragma once
#include "CCollider.h"

BEGIN(Engine)

class CPlyTex;

class ENGINE_DLL CSphereCollider : public CCollider
{
protected:
	explicit CSphereCollider();
	explicit CSphereCollider(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CSphereCollider(const CSphereCollider& rhs);
	virtual ~CSphereCollider();

public:
	virtual _int	Update_Component(const _float& fTimeDelta);
	virtual void	LateUpdate_Component();

	_bool Intersect(CCollider* pOther) override;

	/* Interface IRenderable */
	virtual void Render(LPDIRECT3DDEVICE9& pGraphicDev) override;
	/* ---------------------- */

public:
	BoundingSphere m_tSphere;

public:
	static CCollider* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	CComponent* Clone() override;

	// CCollider을(를) 통해 상속됨
	void Set_Radius(const _float& fRadius) override;

private:
	CPlyTex* m_pDebugSphereTex = nullptr;

	virtual void Free() override;
};

END

