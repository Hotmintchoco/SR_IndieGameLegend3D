#pragma once

#include "CFrustum.h"
#include "IReflectable.h"

namespace Engine
{
	class CPlyTex;
	class CTexture;
	class CGameObject;
}

class CUnbreakableFrustum : public CFrustum, public IReflectable
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

	/* IReflectable */
	virtual const _vec3 GetNormal() override;

private:
	HRESULT			Add_Component();
	virtual void Destroy() override;

protected:
	Engine::CPlyTex* m_pBufferCom;
	Engine::CTexture* m_pTextureCom;

public:
	static CUnbreakableFrustum* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();
};

