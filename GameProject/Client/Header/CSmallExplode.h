#pragma once

#include "CParticle.h"

namespace Engine
{
	class CRcTex;
	class CTexture;
}

class CSmallExplode : public CParticle
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

protected:
	Engine::CRcTex* m_pBufferCom;
	Engine::CTexture* m_pTextureCom;
public:
	static CSmallExplode* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CSmallExplode* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vScale);


private:
	virtual void		Free();
};

