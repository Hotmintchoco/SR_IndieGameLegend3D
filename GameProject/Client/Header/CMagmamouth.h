#pragma once

#include "CMonster.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CCalculator;
}

#define MAPX 13.f
#define MAPZ 11.f

class CMagmamouth : public CMonster
{
protected:
	explicit CMagmamouth(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMagmamouth();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

	virtual			void		OnCollisionEnter(CGameObject* pOther) override;

private:
	HRESULT			Add_Component();

public:
	static CMagmamouth* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	void Shuffle_Array(_uint N);

	void Spawn_Speyeder(const _float& fTimeDelta);
	void Throw_Fireball(const _float& fTimeDelta);
	void Move_Magmamouth(const _float& fTimeDelta);


	void Set_MovePosition();
	void Set_Position();
	void Find_BackPoint();

	void Set_Motion();
	void Set_Motion_OpenMouth(const _float& fTimeDelta);
	void Set_Motion_CloseMouth(const _float& fTimeDelta);
	void Set_Motion_CloseOpenMouth(const _float& fTimeDelta);

	void MagmaMouth_Trail(const _float& fTimeDelta);

protected:
	virtual void		Free();

private:
	enum MAGMAMOUTHSTATE { SPAWN, FIREBALL, MOVE, IDLE };
	MAGMAMOUTHSTATE m_eMagmaMouthState;

	_float m_fSpawn_CoolDown;
	_float m_fSpawnTime;
	_bool m_bSpawnFinish[4];
	_uint m_iSpawnOrderArr[4];

	_float m_fStateUpdateTime;
	_float m_fStateUpdateDuration;
	_bool m_bFireballFinish[3];

	_vec3 m_vRoomCenterLocation;
	_vec3 m_MovePosition;
	_bool m_bMoveFlag;
	_bool m_bMoveFlag2;

	_bool m_bTrailStart;
	_bool m_bTrailFinish;
	_float m_fTrailTime;
	_float m_fTrailDuration;
	_vec3 m_fTrailPoint[4];

	_uint m_iMonsterX;
	_uint m_iMonsterZ;
	_uint m_iPlayerX;
	_uint m_iPlayerZ;

	_bool m_bCloseMouth;

};
