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

public:
	void Set_LandingDirection(const _vec3& vLocation) { m_vLandingDirection = vLocation; }
	void Land(const _float& fTimeDelta);
private:
	_bool m_bLandingState;
	_vec3 m_vLandingDirection;
	_float m_fLandingTime;

protected:
	virtual void		Free();
};
