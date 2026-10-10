#pragma once
#include "CMonster.h"

class CWalkerboss : public CMonster
{
protected:
	explicit CWalkerboss(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CWalkerboss();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

	virtual			void		OnCollisionEnter(COLLINFO eCollInfo) override;

	void Check_Hp(_float& fTimeDelta) override;
	void Animation_Monster(const _float& fTimeDelta) override;
	void Check_Attack(const _float& fTimeDelta);
private:
	HRESULT			Add_Component();

public:
	static CWalkerboss* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	void Move(const _float& fTimeDelta);
	void Attack(const _float& fTiemDelta);

private:
	_float m_fAttackInterval = 2.f + _float(rand() % 32) / 16.f;
	_float m_fAttackElapsedTime = 0.f;
	_bool m_bAttackStart = false;

protected:
	virtual void		Free();
};
