#pragma once

#include "CFrustum.h"

namespace Engine
{
	class CPlyTex;
	class CTexture;
	class CGameObject;
}

class CUnbreakableFrustum : public CFrustum
{
protected:
	explicit CUnbreakableFrustum(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUnbreakableFrustum();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();


	virtual void	OnCollisionEnter(CGameObject* pOther) override;

private:
	HRESULT			Add_Component();

protected:
	Engine::CPlyTex* m_pBufferCom;
	Engine::CTexture* m_pTextureCom;

public:
	static CUnbreakableFrustum* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();
};

