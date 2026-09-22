#pragma once

#include "CEffect.h"

namespace Engine
{
	class CRcColCustom;
}

class CEffect_YellowBox : public CEffect
{
protected:
	explicit CEffect_YellowBox(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CEffect_YellowBox(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3(&vTrailPoint)[4]);
	explicit CEffect_YellowBox(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3(&vTrailPoint)[4], const _float& fLifeTime);
	virtual ~CEffect_YellowBox();

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
	static CEffect_YellowBox* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CEffect_YellowBox* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos);
	static CEffect_YellowBox* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vVelocity);

	void Set_Velocity(const _vec3& vVelocity) { m_vVelocity = vVelocity; }
private:
	//_vec3 m_vYellowBox_Point[4];
	_float m_fLifeTime;
	_float m_fElapsedLifeTime;

	_vec3 m_vVelocity;

private:
	virtual void		Free();
};

