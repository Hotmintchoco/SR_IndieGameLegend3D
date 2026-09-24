#pragma once

#include "CParticle.h"

namespace Engine
{
	class CRcColCustom;
}

class CTrail : public CParticle
{
protected:
	explicit CTrail(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTrail();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();


	void Set_TrailPoint(const _vec3(&vTrailPoint)[4]) {
		for (int i = 0; i < 4; ++i)
		{
			m_vTrailPoint[i] = vTrailPoint[i];
		}
	}
	void Set_Color(const D3DXCOLOR(&eColor)[4]) {
		for (int i = 0; i < 4; ++i)
		{
			m_eColor[i] = eColor[i];
		}
	}
private:
	HRESULT			Add_Component();

private:
	Engine::CRcColCustom* m_pBufferCom = nullptr;

public:
	static CTrail* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CTrail* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3(&vTrailPoint)[4], const _float& fLifeTime);
	static CTrail* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3(&vTrailPoint)[4], const D3DXCOLOR(&eColor)[4], const _float& fLifeTime);

private:
	_vec3 m_vTrailPoint[4] = {};
	D3DXCOLOR m_eColor[4] = {};
	//_float m_fLifeTime = 0.f;
	//_float m_fElapsedLifeTime = 0.f;

private:
	virtual void		Free();
};

