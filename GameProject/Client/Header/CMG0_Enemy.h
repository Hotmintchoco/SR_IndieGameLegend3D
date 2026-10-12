#pragma once

#include "CMG0_Sprite.h"

class CMG0_Player;

class CMG0_Enemy : public CMG0_Sprite
{
protected:
	explicit CMG0_Enemy(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMG0_Enemy();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

	virtual void	Check_Hp();
	virtual void	Move(_float fTimeDelta) {}
	virtual void	Attack(_float fTimeDelta, CMG0_Player* pPlayer) {}

	void			Set_Damage(_int iDamage);
	_int			Get_Hp() { return m_iHp; }
	_int			Get_MaxHp() { return m_iMaxHp; }
	_float			Get_HitRadius() { return 1.1f * m_fScale; }

protected:
	void			Update_HitState(_float fTimeDelta);
	void			Collision_Player(CMG0_Player* pPlayer);

protected:
	_int		m_iMaxHp = 3;
	_int		m_iHp = m_iMaxHp;
	_float		m_fHitEffectTime = 0.08f;
	_float		m_fHitEffectElapsedTime = 0.f;
	_bool		m_bHitState = false;

	_bool		m_bDelete = false;
	_float		m_fElapsedTime = 0.f;
	_float		m_fBodyRadius = 1.4f;

protected:
	virtual void		Free();
};
