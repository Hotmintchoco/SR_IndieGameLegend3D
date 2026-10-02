#pragma once

#include "CMonster.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CCalculator;
}

class CWorm : public CMonster
{
public:
	enum WORMDIR
	{
		FRONT,
		SIDE,
		TOP,
		SIDE45,
		CONNECTOR
	};

protected:
	explicit CWorm(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CWorm();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

	virtual			void		OnCollisionEnter(COLLINFO eCollInfo) override;

private:
	HRESULT			Add_Component();

	void Shuffle_Array(_uint N);

	void Update_Motion(const _float& fTimeDelta);

	void Opening_Worm(const _float& fTimeDelta);
	void Move_Worm(const _float& fTimeDelta);
	void Spawn_Monster(const _float& fTimeDelta);
	void IDLE_Worm(const _float& fTimeDelta);

	void Look_AtPlayer();
	void Look_AtDestination();
	void Chase_Player_Worm(const _float& fTimeDelta);

	void Worm_Dead(const _float& fTimeDelta);
	void Worm_Dead_Effect();
	void Set_Front_Worm(CWorm* pFrontWorm) { m_pFrontWorm = pFrontWorm; }
	CGameObject* Get_Front_Worm() { return m_pFrontWorm; }
	void Set_Head_Worm(CWorm* pHeadWorm) { m_pHeadWorm = pHeadWorm; }
	CGameObject* Get_Head_Worm() { return m_pHeadWorm; }
	void Set_WormIndex (_uint iIndex) { m_iWormIndex = iIndex; }

	void Set_Angle();

public:
	static CWorm* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CWorm* Create(LPDIRECT3DDEVICE9 pGraphicDev, _uint iIndex);

protected:
	virtual void		Free();

private:
	enum WormSTATE { SPAWN, MOVE, IDLE, DEAD, OPENING };
	WormSTATE m_eWormState = OPENING;
	_bool m_bOpening = true;

	_vec3 m_fAngle_FromPlayer = {};
	_uint m_iWormIndex = 1;
	CMonster* m_pFrontWorm = nullptr;
	CMonster* m_pHeadWorm = nullptr;

	_int m_iPhase = 0;

	_float m_fSpawn_CoolDown = 1.0f;
	_float m_fSpawnTime = 0.f;
	_bool m_bSpawnFinish[4] = {};
	_bool m_bSpawnFinish2[4] = {};
	_float m_fSpawnStartTime[4] = {};
	_uint m_iSpawnOrderArr[4] = {};
	CGameObject* m_pSpawnMonster[4] = {};

	_float m_fStateUpdateTime = 0.f;
	_float m_fStateUpdateDuration = 2.f;

	_vec3 m_vRoomCenterLocation = {};
	_vec3 m_vMovePosition = {};
	_bool m_bMoveFlag = false;
	_bool m_bMoveFlag2 = false;
	_bool m_bMoveState = true;

	_bool m_bLandingState = true;
	_vec3 m_vLandingDirection = {};
	_float m_fLandingTime = 0.f;
	_float m_fVelocityY = 0.f;
	_uint m_iLandingCount = 0;

	_bool m_bDead_Effect1 = false;
	_bool m_bDead_Effect2 = false;
	_float m_fElapsedDeadTime = 0.f;
	_float m_fElapsedDeadTime2 = 0.f;
	_float m_fDeadTime = 5.f;
	_bool m_bDeadStart = false;
	_bool m_DeadExplosion = false;

	_bool m_bOpeningMoveFlag = false;
	_bool m_bOpeningMoveFlag2 = false;
	_float m_bElapsedOpeningTime = 0.f;

	_vec3 m_vOpeningMoveDirection[6] =
	{
		{2.5f,0,0}, {-5,0,0},
		{5,0,0}, {-5,0,0},
		{2.5f,0,0},
		{0,0,+2.0f}
	};
	_int m_iOpeningMoveIndex = 0;
};
