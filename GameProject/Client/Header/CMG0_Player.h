#pragma once

#include "CGameObject.h"
#include "CMiniGame_Header.h"

class CMG0_Player : public CGameObject
{
protected:
	explicit CMG0_Player(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMG0_Player();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

	void			OnHit();
	void			Reset();

	const _vec3&	Get_Pos() const { return m_vPos; }
	_int			Get_Life() const { return m_iLife; }
	_float			Get_Invincible() const { return m_fInvincible; }
	void			Set_Invincible(_float fTime) { m_fInvincible = fTime; }
	_bool			Is_GameOver() const { return m_bGameOver; }
	_int			Get_Attack() const { return m_iAttack; }
	void			Set_Attack(_int iAttack) { m_iAttack = iAttack; }

private:
	void			Ready_Mesh();
	void			Move_Free(_float fTimeDelta);
	void			Move_Train(_float fTimeDelta);
	void			Fire(_float fTimeDelta);
	void			Move_Lane(_int iDir);
	void			Effect_Explode(const _vec3& vPos, _float fScale);

private:
	vector<MINIGAME0::MGVTX>	m_vecMesh;

	_vec3		m_vPos{ 0.f, 2.f, 0.f };
	_float		m_fSpeed = 11.f;
	_float		m_fRoll = 0.f;
	_float		m_fPitch = 0.f;

	_float		m_fFireCool = 0.f;
	_float		m_fFireInterval = 0.12f;
	_int		m_iAttack = MG_BASE_ATTACK;
	_int		m_iBulletCount = 1;
	_int		m_iLane = MG_LANE_COUNT / 2;
	_int		m_iLaneDir = 0;
	_float		m_fLaneTime = 0.f;

	_float		m_fInvincible = 0.f;
	_int		m_iLife = MG_MAX_LIFE;
	_bool		m_bGameOver = false;
	_bool		m_bActive = false;
	_bool		m_bVisible = false;

public:
	static CMG0_Player* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void		Free();
};
