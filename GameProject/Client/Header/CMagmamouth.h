#pragma once
#include "CMonster.h"

#define MAPX 13.f
#define MAPZ 11.f

class CMagmamouth : public CMonster
{
protected:
	explicit CMagmamouth(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMagmamouth();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

	virtual			void		OnCollisionEnter(COLLINFO eCollInfo) override;

private:
	HRESULT			Add_Component();

public:
	static CMagmamouth* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	void Shuffle_Array(_uint N);

	void Opening_MagmaMouth(const _float& fTimeDelta);

	void Spawn_Spider(const _float& fTimeDelta);
	void Throw_Fireball(const _float& fTimeDelta);
	void Move_Magmamouth(const _float& fTimeDelta);

	void Update_Motion(const _float& fTimeDelta);

	void Set_MovePosition();
	void Set_Position();
	void Find_BackPoint();

	void Set_Motion();
	void Set_Motion_OpenMouth(const _float& fTimeDelta);
	void Set_Motion_CloseMouth(const _float& fTimeDelta);
	void Set_Motion_CloseOpenMouth(const _float& fTimeDelta);

	void MagmaMouth_Trail(const _float& fTimeDelta);

	void MagmaMouth_Dead(const _float& fTimeDelta);
	void MagmaMouth_Dead_Effect();

protected:
	virtual void		Free();

private:
	enum MAGMAMOUTHSTATE { SPAWN, FIREBALL, MOVE, IDLE, DEAD, OPENING };
	MAGMAMOUTHSTATE m_eMagmaMouthState;

	_float m_fSpawn_CoolDown;
	_float m_fSpawnTime;
	_bool m_bSpawnFinish[4];
	_uint m_iSpawnOrderArr[4];

	_float m_fStateUpdateTime;
	_float m_fStateUpdateDuration;
	_bool m_bFireballFinish[4];

	_vec3 m_vRoomCenterLocation;
	_vec3 m_vMovePosition;
	_bool m_bMoveFlag;
	_bool m_bMoveFlag2;

	_bool m_bTrailStart;
	_bool m_bTrailFinish;
	_float m_fTrailTime;
	_float m_fTrailTime2;
	_float m_fTrailDuration;
	_vec3 m_fTrailPoint[4];

	_uint m_iMonsterX;
	_uint m_iMonsterZ;
	_uint m_iPlayerX;
	_uint m_iPlayerZ;

	_bool m_bCloseMouth;

	_bool m_bDead_Effect1 = false;
	_bool m_bDead_Effect2 = false;
	_float m_fElapsedDeadTime = 0.f;
	_float m_fElapsedDeadTime2 = 0.f;
	_float m_fDeadTime = 5.f;
	_bool m_DeadFireball[3] = { false, false, false };
	_bool m_DeadExplosion = false;

	_bool m_bOpening = true;
	_bool m_bOpeningMoveFlag = false;
	_float m_bElapsedOpeningTime = 0.f;

	_vec3 m_vOpeningMoveDirection[5] =
	{
		{-3,0,-3}, {+6,0,0},
		{0,0,+6}, {-3,0,-3},
		{+4.5f,0,0}
	};
	_int m_iOpeningMoveIndex = 0;



	_uint m_iPhase = 0;
	_bool m_bMoveState = true;

	inline static _vec3 s_vRoomCenter = { 0.f,0.f,0.f };
};
