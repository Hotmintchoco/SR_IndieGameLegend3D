#pragma once

#include "CParticle.h"

namespace Engine
{
	class CRcColCustom;
}

class CParticle_Rectangle : public CParticle
{
protected:
	explicit CParticle_Rectangle(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CParticle_Rectangle();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

private:
	HRESULT			Add_Component();

protected:
	Engine::CRcColCustom* m_pBufferCom = nullptr;

public:
	static CParticle_Rectangle* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CParticle_Rectangle* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vVelocity, D3DXCOLOR eColor);

	void Set_Velocity(const _vec3& vVelocity) { m_vVelocity = vVelocity; }
	void Set_Color(const D3DXCOLOR& eColor) { m_eColor = eColor; }
private:

	_vec3 m_vVelocity = { 0.f,0.f,0.f };
	D3DXCOLOR m_eColor = { 1.f,1.f,1.f,1.f };
private:
	virtual void		Free();
};

