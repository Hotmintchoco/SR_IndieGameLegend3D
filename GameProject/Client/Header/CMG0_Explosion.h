#pragma once
#include "CMG0_Sprite.h"

class CMG0_Explosion : public CMG0_Sprite
{
protected:
	explicit CMG0_Explosion(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMG0_Explosion();

public:
	virtual			HRESULT		Ready_GameObject(const _vec3& vPos, _float fScale);
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

private:
	HRESULT			Add_Component();

private:
	_float		m_fLifeTime = 0.4f;
	_float		m_fElapsedTime = 0.f;

public:
	static CMG0_Explosion* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos, _float fScale);

protected:
	virtual void		Free();
};
