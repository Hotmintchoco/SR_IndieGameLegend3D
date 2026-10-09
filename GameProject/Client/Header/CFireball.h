#pragma once
#include "CMonster.h"

class CFireball : public CMonster
{
protected:
	explicit CFireball(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CFireball();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

	virtual			void		OnCollisionEnter(COLLINFO eCollInfo) override;

	void Check_Hp(_float& fTimedelta) override {}
	void Animation_Monster(const _float& fTimeDelta) override;

private:
	HRESULT			Add_Component();

public:
	static CFireball* Create(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	void Set_Velocity(const _vec3& vLocation) { m_vVelocity = vLocation; }
	void Throw(const _float& fTimeDelta);
private:
	/* 성철 */
	void CheckDeadCondition();
	/* --- */

	_vec3 m_vVelocity{ 0.f,0.f,0.f };
	_float m_fLandingTime = 0.f;
	_float m_fLandingVelocity = 0.f;
	_uint m_iLandingCount = 0;

protected:
	virtual void		Free();
};
