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
	void Spawn_Speyeder(const _float& fTimeDelta);
	void Shuffle_Array(_uint N);

	void Throw_Fireball(const _float& fTimeDelta);
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


protected:
	virtual void		Free();
};
