#pragma once

#include "CGameObject.h"
#include "CMiniGame_Header.h"

namespace Engine
{
	class CTransform;
}

class CMiniGame;
class CMG0_DancerPart;

class CMG0_Herobrine : public CGameObject
{
private:
	enum PART { PART_BODY, PART_HEAD, PART_LARM, PART_RARM, PART_LLEG, PART_RLEG, PART_END };
	enum STATE { STATE_WAIT, STATE_ENTER, STATE_IDLE, STATE_DEAD, STATE_EXPLODE, STATE_END };
	enum PATTERN { PATTERN_SKULL, PATTERN_LASER, PATTERN_FAN, PATTERN_SIDE_LASER, PATTERN_RING, PATTERN_END };

protected:
	explicit CMG0_Herobrine(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMG0_Herobrine();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

public:
	void			Set_Damage(_int iDamage);
	_bool			Is_Hit(const _vec3& vPos) const;
	_bool			Is_Dying() const { return m_eState >= STATE_DEAD; }
	_bool			Is_Dead() const { return STATE_END == m_eState; }
	_int			Get_Hp() const { return m_iHp; }
	_int			Get_MaxHp() const { return m_iMaxHp; }

private:
	HRESULT			Add_Component();
	HRESULT			Ready_Texture(LPDIRECT3DTEXTURE9* ppTexture, _bool bDead);
	HRESULT			Ready_Part();
	void			Update_State(CMiniGame* pScene, _float fTimeDelta);
	void			Update_Part(_float fTimeDelta);

	void			Update_Pattern(CMiniGame* pScene, _float fTimeDelta);
	void			Next_Pattern();
	_bool			Is_Step(_float fGap, _int iCount);
	_bool			Pattern_Skull(CMiniGame* pScene);
	_bool			Pattern_Laser(CMiniGame* pScene);
	_bool			Pattern_Fan(CMiniGame* pScene);
	_bool			Pattern_SideLaser(CMiniGame* pScene);
	_bool			Pattern_Ring(CMiniGame* pScene);

	void			Update_Dead(CMiniGame* pScene, _float fTimeDelta);
	void			Effect_Explode(CMiniGame* pScene, _float fScale);
	_vec3			Get_CorePos() const;
	_vec3			Get_AimDir(CMiniGame* pScene) const;

private:
	CMG0_DancerPart*	m_pPart[PART_END]{};
	Engine::CTransform*	m_pTransformCom = nullptr;
	LPDIRECT3DTEXTURE9	m_pTexture = nullptr;
	LPDIRECT3DTEXTURE9	m_pDeadTexture = nullptr;

	STATE		m_eState = STATE_WAIT;
	_float		m_fHeight = MG_BOSS_ENTER_HEIGHT;
	_float		m_fStateTime = 0.f;
	_float		m_fExplodeTime = 0.f;
	_vec3		m_vShake{ 0.f, 0.f, 0.f };
	_int		m_iMaxHp = MG_BOSS_HP;
	_int		m_iHp = MG_BOSS_HP;

	PATTERN		m_ePattern[PATTERN_END]{ PATTERN_SKULL, PATTERN_LASER, PATTERN_FAN, PATTERN_SIDE_LASER, PATTERN_RING };
	_int		m_iPatternIndex = 0;
	_int		m_iPatternStep = 0;
	_float		m_fPatternTime = 0.f;
	_float		m_fPatternRest = MG_BOSS_PATTERN_REST;

public:
	static CMG0_Herobrine* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void		Free();
};
