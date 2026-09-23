#pragma once

#include "CMonster.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CCalculator;
}

class CFireball : public CMonster
{
protected:
	explicit CFireball(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CFireball();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

	virtual			void		OnCollisionEnter(CGameObject* pOther) override;

private:
	HRESULT			Add_Component();

public:
	static CFireball* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CFireball* Create(LPDIRECT3DDEVICE9 pGraphicDev, _bool bSammon);

public:
	void Set_Velocity(const _vec3& vLocation) { m_vVelocity = vLocation; }
	void Throw(const _float& fTimeDelta);
private:
	/* 성철 */
	void CheckDeadCondition();
	/* --- */

	_vec3 m_vVelocity;
	_float m_fLandingTime;
	_float m_fLandingVelocity;
	_uint m_iLandingCount;
protected:
	virtual void		Free();
};
