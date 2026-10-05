#pragma once

#include "CMonster.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CCalculator;
}

class CCryder : public CMonster
{
protected:
	explicit CCryder(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCryder();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

	virtual			void		OnCollisionEnter(COLLINFO eCollInfo) override;

private:
	HRESULT			Add_Component();

public:
	static CCryder* Create(LPDIRECT3DDEVICE9 pGraphicDev);

public:
	void Set_Velocity(const _vec3& vDirection) { m_vLandingDirection = vDirection; }
	void Land(const _float& fTimeDelta);
private:
	_bool m_bLandingState;
	_vec3 m_vLandingDirection;
	_float m_fLandingTime;
	_float m_fVelocityY;

protected:
	virtual void		Free();
};
