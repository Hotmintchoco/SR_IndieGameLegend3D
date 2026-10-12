#pragma once
#include "CMG0_Enemy.h"

class CMG0_Enemy_Skull : public CMG0_Enemy
{
protected:
	explicit CMG0_Enemy_Skull(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMG0_Enemy_Skull();

public:
	virtual			HRESULT		Ready_GameObject(const _vec3& vPos, _int iHp);
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

	void Move(_float fTimeDelta) override;
	void Attack(_float fTimeDelta, CMG0_Player* pPlayer) override;

private:
	HRESULT			Add_Component();

private:
	_vec3		m_vVelocity{ 0.f, 0.f, 0.f };
	_float		m_fPhase = 0.f;
	_float		m_fFireCool = 0.f;

public:
	static CMG0_Enemy_Skull* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos, _int iHp);

protected:
	virtual void		Free();
};
