#pragma once

#include "CEffect.h"

namespace Engine
{
	class CRcColCustom;
}

class CTrail_MagmaMouth : public CEffect
{
protected:
	explicit CTrail_MagmaMouth(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTrail_MagmaMouth();

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
	void Set_LifeTime(const _float& fLifeTime) { m_fLifeTime = fLifeTime; }
private:
	HRESULT			Add_Component();

private:
	Engine::CRcColCustom* m_pBufferCom = nullptr;

public:
	static CTrail_MagmaMouth* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CTrail_MagmaMouth* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3(&vTrailPoint)[4], const _float& fLifeTime);

private:
	_vec3 m_vTrailPoint[4];
	_float m_fLifeTime;
	_float m_fElapsedLifeTime;

private:
	virtual void		Free();
};

