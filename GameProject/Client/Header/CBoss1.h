#pragma once

#include "CMonster.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CCalculator;
}

class CBoss1 : public CMonster
{
protected:
	explicit CBoss1(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBoss1();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

	virtual			void		OnCollisionEnter(CGameObject* pOther) override;

private:
	HRESULT			Add_Component();

	void Shuffle_Array(_uint N);

	void Update_Motion(const _float& fTimeDelta);

	void Opening_Boss1(const _float& fTimeDelta);
	void Move_Boss1(const _float& fTimeDelta);
	void Spawn_Spn(const _float& fTimeDelta);
	void IDLE_Boss1(const _float& fTimeDelta);

	void Set_Stand(const _float& fTimeDelta);
	void Set_Walking(const _float& fTimeDelta);
	void Look_AtPlayer();
	void Look_AtDestination();
	void Chase_Player(const _float& fTimeDelta);

	void Boss1_Dead(const _float& fTimeDelta);
	void Boss1_Dead_Effect();

protected:
	Engine::CTexture* m_pTextureCom2 = nullptr;
	Engine::CTransform* m_pTransformCom2 = nullptr;
public:
	static CBoss1* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void		Free();

private:
	enum BOSS1STATE { SPAWN, MOVE, IDLE, DEAD, OPENING };
	BOSS1STATE m_eBoss1State = OPENING;
	_bool m_bOpening = true;
	_bool m_bStand = false;

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

	_bool m_bTrailStart = false;
	_bool m_bTrailFinish = false;
	_float m_fTrailTime = 0.f;
	_float m_fTrailTime2 = 0.f;
	_float m_fTrailDuration = 0.f;
	_vec3 m_fTrailPoint[4] = {};

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
