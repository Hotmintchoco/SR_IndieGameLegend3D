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
	enum WORMSTATE { SPAWN, MOVE, IDLE, DEAD, ATTACK, OPENING };

protected:
	explicit CWorm(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CWorm();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

	virtual			void		OnCollisionEnter(COLLINFO eCollInfo) override;

private:
	HRESULT			Add_Component();

	void Update_Motion(const _float& fTimeDelta);

	void Opening_Worm(const _float& fTimeDelta);
	void Move_WormHead(const _float& fTimeDelta);
	void Update_WormBoby(const _float& fTimeDelta);
	void Spawn_Monster(const _float& fTimeDelta);
	void IDLE_Worm(const _float& fTimeDelta);

	void Move_WormHead_BeforeSpawn(const _float& fTimeDelta);

	void Set_MoveDest();
	void Set_Pos_Worm(_vec3 vPos);
	void Set_Speed_Worm(_float fSpeed);
	void Push_Back_MoveDest(const _vec3& vDest);
	void Clear_MoveDest();

	void Worm_Dead(const _float& fTimeDelta);
	void Worm_Dead_Effect();

	void Set_Prev_Worm(CMonster* pPrevWorm) { m_pPrevWorm = pPrevWorm; }
	CMonster* Get_Prev_Worm() { return m_pPrevWorm; }
	void Set_Next_Worm(CMonster* pNextWorm) { m_pNextWorm = pNextWorm; }
	CMonster* Get_Next_Worm() { return m_pNextWorm; }
	void Set_Head_Worm(CMonster* pHeadWorm) { m_pHeadWorm = pHeadWorm; }
	CMonster* Get_Head_Worm() { return m_pHeadWorm; }
	void Set_WormIndex (_uint iIndex) { m_iWormIndex = iIndex; }


	void Set_Init_Worm();
	void Set_Motion_FromAngle();

public:
	virtual void Set_Damage(_int iDamage) { m_iHp -= iDamage; }
	virtual _int Get_Hp() { return m_iHp; }
	WORMSTATE Get_WormState() { return m_eWormState; }
	
public:

public:
	static CWorm* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CWorm* Create(LPDIRECT3DDEVICE9 pGraphicDev, _uint iIndex, CWorm* Front);

protected:
	virtual void		Free();

private:
	WORMSTATE m_eWormState = OPENING;
	WORMDIR m_eDir = FRONT;
	_bool m_bOpening = true;

	_vec3 m_fAngle_FromPlayer = {};
	_uint m_iWormIndex = 1;
	CMonster* m_pPrevWorm = nullptr;
	CMonster* m_pNextWorm = nullptr;
	CMonster* m_pHeadWorm = this;

	_int m_iPhase = 0;

	//_float m_fSpawn_CoolDown = 1.0f;
	_float m_fSpawnTime = 0.f;
	//_bool m_bSpawnFinish[4] = {};
	//_bool m_bSpawnFinish2[4] = {};
	//_float m_fSpawnStartTime[4] = {};
	//_uint m_iSpawnOrderArr[4] = {};
	//CGameObject* m_pSpawnMonster[4] = {};
	_bool m_bSpawnStart = false;

	_float m_fStateUpdateTime = 0.f;
	_float m_fStateUpdateDuration = 2.f;

	_vec3 m_vRoomCenterLocation = {};

	_bool m_bMoveFlag = false;
	_bool m_bMoveFlag2 = false;
	_bool m_bMoveState = true;

	_vec3 m_vSpawnDirection = {};

	_bool m_bDead_Effect1 = false;
	_bool m_bDead_Effect2 = false;
	_float m_fElapsedDeadTime = 0.f;
	_float m_fElapsedDeadTime2 = 0.f;
	_float m_fDeadTime = 5.f;
	_bool m_bDeadStart = false;
	_bool m_DeadExplosion = false;

	_bool m_bOpeningStart = false;
	//_bool m_bOpeningMoveFlag = false;
	//_bool m_bOpeningMoveFlag2 = false;
	_float m_bElapsedOpeningTime = 0.f;

	vector<_vec3> m_vMoveDest;

	_bool m_bSet_InitPos = false;
	_float m_fSpeed = 6.f;
	_float m_fMoveHeight = 1.25f;
};
