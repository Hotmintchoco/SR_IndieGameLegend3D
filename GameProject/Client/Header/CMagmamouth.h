#pragma once

#include "CMonster.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
	class CCalculator;
}

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
	void Spawn_Speyeder();
	void Shuffle_SpawnArray();
private:
	enum MAGMAMOUTHSTATE { IDLE, SPAWN, ATTACK, MOVE };
	MAGMAMOUTHSTATE m_eMagmaMouthState;

	_float m_fSpawn_CoolDown;
	_float m_fSpawnTime;
	_bool m_bSpawnFinish[4];
	_uint m_iSpawnOrderArr[4];

	_float m_fStateUpdateTime;

protected:
	virtual void		Free();
};
