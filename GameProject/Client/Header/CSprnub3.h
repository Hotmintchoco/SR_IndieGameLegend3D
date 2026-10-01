#pragma once

#include "CMonster.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CCalculator;
}

class CSprnub3 : public CMonster
{
protected:
	explicit CSprnub3(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CSprnub3();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

	virtual			void		OnCollisionEnter(COLLINFO eCollInfo) override;

private:
	HRESULT			Add_Component();

public:
	static CSprnub3* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CSprnub3* Create(LPDIRECT3DDEVICE9 pGraphicDev, _float fActiveTime);
	void Set_ActiveTime(_float fActiveTime) { m_fActiveTime = fActiveTime; }
private:
	void Jump(const _float& fTimeDelta);

private:
	_float m_fActiveTime = 0.f;
	_float m_fActiveElapsedTime = 0.f;

	_bool m_bLandingState = false;
	_vec3 m_vJumpDirection = {};
	_float m_fJumpTime = 0.f;
	_float m_fVelocityY = 0.f;
protected:
	virtual void		Free();
};
