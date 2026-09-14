#pragma once

#include "CEffect.h"

namespace Engine
{
	class CRcTex;
	class CTexture;
	class CTransform;
}

class CSmallExplode : public CEffect
{
protected:
	explicit CSmallExplode(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CSmallExplode();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

private:
	HRESULT			Add_Component();

private:
	//_float				m_fFrame;

public:
	static CSmallExplode* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CSmallExplode* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vScale);


private:
	virtual void		Free();
};

