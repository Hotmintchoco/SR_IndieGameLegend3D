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
	enum PARTICLE_RECT_TYPE{DEAD, BULLET};

public:
	static CParticle_Rectangle* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CParticle_Rectangle* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vVelocity, D3DXCOLOR eColor);
	static CParticle_Rectangle* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vVelocity, D3DXCOLOR eColor, _float fLifeTime);
	static CParticle_Rectangle* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vVelocity, _vec3 vScale, D3DXCOLOR eColor, _float fLifeTime);
	static CParticle_Rectangle* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vVelocity, _vec3 vScale, D3DXCOLOR eColor, _float fLifeTime, PARTICLE_RECT_TYPE eType);

	void Set_Velocity(const _vec3& vVelocity) { m_vVelocity = vVelocity; }
	void Set_Color(const D3DXCOLOR& eColor) { m_eColor = eColor; }
	void Set_Type(_uint iType) { m_eType = iType; }

private:

	_vec3 m_vVelocity = { 0.f,0.f,0.f };
	D3DXCOLOR m_eColor = { 1.f,1.f,1.f,1.f };
	_uint m_eType = DEAD;
private:
	virtual void		Free();
};

