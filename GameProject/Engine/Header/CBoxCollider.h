#pragma once
#include "CCollider.h"

BEGIN(Engine)

class CSphereCollider;
class CCubeTex;

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

	void			Set_DiffPos(const _vec3& vDiffPos) { m_vDiffPos = vDiffPos; }
	_vec3			Get_DiffPos() { return m_vDiffPos; }

	void			Render_DebugCube();

public:
	BoundingBox		m_tBox;

private:
	CCubeTex*		m_pDebugCubeTex = nullptr;
	_vec3 			m_vDiffPos = _vec3{ 0.f, 0.f, 0.f };

public:
	static CCollider* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	CComponent* Clone() override;

protected:
	virtual void	Free() override;
};

END

