#pragma once

#include "CEffect.h"

namespace Engine
{
	class CRcTex;
}

class CEffect_YellowBox : public CEffect
{
protected:
	explicit CEffect_YellowBox(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CEffect_YellowBox();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

private:
	HRESULT			Add_Component();

protected:
	Engine::CRcTex* m_pBufferCom;

public:
	static CEffect_YellowBox* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CEffect_YellowBox* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vScale);


private:
	virtual void		Free();
};

