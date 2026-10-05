#pragma once

#include "CParticle.h"

namespace Engine
{
	class CRcTex;
	class CTexture;
}

class CShockwave : public CParticle
{
protected:
	explicit CShockwave(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CShockwave();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

private:
	HRESULT			Add_Component();

protected:
	Engine::CRcTex* m_pBufferCom = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;
public:
	static CShockwave* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CShockwave* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _vec3 vScale);


private:
	virtual void		Free();
};

