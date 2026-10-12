#pragma once
#include "CMG0_Sprite.h"

class CMG0_Bullet : public CMG0_Sprite
{
protected:
	explicit CMG0_Bullet(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMG0_Bullet();

public:
	virtual			HRESULT		Ready_GameObject(MINIGAME0::BULLETTYPE eType, const _vec3& vPos, const _vec3& vVelocity, _int iDamage);
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

	MINIGAME0::BULLETTYPE	Get_Type() { return m_eType; }

private:
	HRESULT			Add_Component();
	_bool			Is_OutOfRange();
	_bool			Collision_Enemy();
	_bool			Collision_Player();

private:
	MINIGAME0::BULLETTYPE	m_eType = MINIGAME0::BULLET_END;

	_vec3		m_vVelocity{ 0.f, 0.f, 0.f };
	_float		m_fLifeTime = 0.f;
	_float		m_fFrame = 0.f;
	_float		m_fFrameSpeed = 0.f;
	_int		m_iDamage = 1;

public:
	static CMG0_Bullet* Create(LPDIRECT3DDEVICE9 pGraphicDev, MINIGAME0::BULLETTYPE eType, const _vec3& vPos, const _vec3& vVelocity, _int iDamage = 1);

protected:
	virtual void		Free();
};
