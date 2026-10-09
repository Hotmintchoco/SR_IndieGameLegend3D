#pragma once

#include "CMonster.h"

class COcto : public CMonster
{
protected:
	explicit COcto(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~COcto();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

	virtual			void		OnCollisionEnter(COLLINFO eCollInfo) override;

	void Check_Hp(_float& fTimeDelta) override;
	void Animation_Monster(const _float& fTimeDelta) override;

private:
	HRESULT			Add_Component();
	void Move_Octo(_float fTimeDelta);
	void Attack_Octo(_float fTimeDelta);

private:
	_float m_fMoveElapsedTime = 0.f;
	_bool m_bMoveFlag = false;
	_bool m_bMoveOrigin = false;
	_bool m_bRightMove = false;
	_vec3 m_vOriginPos = {};
	_vec3 m_vMoveDest = {};
	_bool m_bUpdateStart = false;
	static _bool sOctoRight;

	_float m_fAttackElapsedTime = 0.f;
	_float m_fAttackTime = 1.f + _float(rand() % 128)/256.f;
public:
	static COcto* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void		Free();
};
