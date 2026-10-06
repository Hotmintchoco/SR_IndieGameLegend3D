#pragma once

#include "CMonster.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CCalculator;
}

class CGlubba : public CMonster
{
protected:
	explicit CGlubba(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CGlubba();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

	virtual			void		OnCollisionEnter(COLLINFO eCollInfo) override;

private:
	HRESULT			Add_Component();

public:
	void Set_Velocity(const _vec3& vDirection) { m_vLandingDirection = vDirection; }
public:
	static CGlubba* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CGlubba* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vDir);

private:
	void Land(const _float& fTimeDelta);
private:

	_bool m_bLandingState = false;
	_float m_fVelocityY = 0.f;
	_vec3 m_vLandingDirection = {};

protected:
	virtual void		Free();
};
