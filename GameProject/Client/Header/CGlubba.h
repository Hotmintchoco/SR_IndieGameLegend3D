#pragma once
#include "CMonster.h"

class CGlubba : public CMonster
{
protected:
	explicit CGlubba(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CGlubba();

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

public:
	void Set_Velocity(const _vec3& vDirection) { m_vLandingDirection = vDirection; }
public:
	static CGlubba* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	void Land(const _float& fTimeDelta);
private:

	_bool m_bLandingState = false;
	_float m_fVelocityY = 0.f;
	_vec3 m_vLandingDirection = { 0.f, 0.f, 0.f };

protected:
	virtual void		Free();
};
