#pragma once

#include "CMonster.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CCalculator;
}

class CSpeyeder : public CMonster
{
protected:
	explicit CSpeyeder(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CSpeyeder();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

	virtual			void		OnCollisionEnter(COLLINFO eCollInfo) override;

private:
	HRESULT			Add_Component();

public:
	static CSpeyeder* Create(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	void Set_Velocity(const _vec3& vDirection) { m_vLandingDirection = vDirection; }
	void Land(const _float& fTimeDelta);
private:
	_bool m_bLandingState = false;
	_vec3 m_vLandingDirection = {};
	_float m_fLandingTime = 0.f;
	_float m_fVelocityY = 0.f;

protected:
	virtual void		Free();
};
