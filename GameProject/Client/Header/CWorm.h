#pragma once

#include "CMonster.h"

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
	enum WORMSTATE { SPAWN, MOVE, ATTACK, IDLE, DEAD, OPENING };

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
	void Attack_Worm(const _float& fTimeDelta);

	void Move_WormHead_BeforeSpawn(const _float& fTimeDelta);
	void Move_WormHead_AfterSpawn(const _float& fTimeDelta);

	void Move_WormHead_BeforeAttack(const _float& fTimeDelta);
	void Move_WormHead_AfterAttack(const _float& fTimeDelta);

	void Set_MoveDest();
	void Set_Pos_Worm(_vec3 vPos);
	void Set_Speed_Worm(_float fSpeed);
	void Set_HeadWorm_Null();
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
	void Set_WormIndex(_uint iIndex) { m_iWormIndex = iIndex; }


	void Set_Init_Worm();
	void Set_Motion_FromAngle();

	void Update_Connector();

public:
	virtual void Set_Damage(_int iDamage) { m_iHp -= iDamage; }
	virtual _int Get_Hp() { return m_iHp; }
	WORMSTATE Get_WormState() { return m_eWormState; }

public:
	void Check_Sandburst(_float fTimeDelta);
	void Effect_Sandburst(_float fLifeTime);
	void Effect_Sandburst2();
protected:
	Engine::CTransform* m_pTransformCom2 = nullptr;
	Engine::CTexture* m_pTextureCom2 = nullptr;

public:
	static CWorm* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CWorm* Create(LPDIRECT3DDEVICE9 pGraphicDev, _uint iIndex, CWorm* Front);

protected:
	virtual void		Free();

private:
	WORMSTATE m_eWormState = OPENING;
	WORMDIR m_eDir = FRONT;
	_bool m_bOpening = true;

	_uint m_iWormIndex = 1;
	CMonster* m_pPrevWorm = nullptr;
	CMonster* m_pNextWorm = nullptr;
	CMonster* m_pHeadWorm = this;

	_int m_iPhase = 0;

	_float m_fSpawnTime = 0.f;
	_float m_fSpawnTime2 = 0.f;
	_bool m_bSpawnStart = false;

	_float m_fStateUpdateTime = 0.f;
	_float m_fStateUpdateDuration = 2.f;

	_vec3 m_vRoomCenterLocation = { 0.f,0.f,0.f };

	_bool m_bMoveFlag = false;
	_bool m_bMoveFlag2 = false;
	_bool m_bMoveState = true;

	_float m_fAttackTime = 0.f;
	_float m_fAttackTime2 = 0.f;
	_bool m_bAttackStart = false;

	_vec3 m_vSpawnDirection = { 0.f,0.f,0.f };

	_bool m_bDead_Effect1 = false;
	_bool m_bDead_Effect2 = false;
	_float m_fElapsedDeadTime = 0.f;
	_float m_fElapsedDeadTime2 = 0.f;
	_float m_fElapsedDeadTime3 = 0.f;
	_float m_fDeadTime = 0.5f;
	_bool m_bDeadStart = false;
	_bool m_DeadExplosion = false;

	_bool m_bOpeningStart = false;

	_float m_bElapsedOpeningTime = 0.f;

	vector<_vec3> m_vMoveDest;

	_bool m_bSet_InitPos = false;
	_float m_fSpeed = 6.f;
	_float m_fMoveHeight = 1.25f;

	_float m_fElapsedTime2 = 0.f;
	_float m_fElapsedTime3 = 0.f;

	_matrix m_matConnector;

	_bool m_bMotionEnd = false;

	inline static _vec3 s_vRoomCenter = { 0.f,0.f,0.f };
};
