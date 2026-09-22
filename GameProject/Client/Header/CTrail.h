#pragma once

#include "CEffect.h"

namespace Engine
{
	class CRcColCustom;
	class CTransform;
}

class CTrail : public CEffect
{
protected:
	explicit CTrail(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTrail(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3(&vTrailPoint)[4]);
	explicit CTrail(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3(&vTrailPoint)[4], const _float& fLifeTime);
	virtual ~CTrail();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

private:
	HRESULT			Add_Component();

private:
	Engine::CRcColCustom* m_pBufferCom;

public:
	static CTrail* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CTrail* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3(&vTrailPoint)[4]);
	static CTrail* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3(&vTrailPoint)[4], const _float& fLifeTime);

public:
	//void Set_TrailPoint(const _vec3(&vTrailPoint)[4]);
private:
	_vec3 m_vTrailPoint[4];
	_float m_fLifeTime;
	_float m_fElapsedLifeTime;
private:
	virtual void		Free();
};

