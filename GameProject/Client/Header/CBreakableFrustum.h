#pragma once

#include "CFrustum.h"

namespace Engine
{
	class CPlyTex;
	class CTexture;
	class CGameObject;
}

class CBreakableFrustum : public CFrustum
{
protected:
	explicit CBreakableFrustum(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBreakableFrustum();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();


	virtual void OnCollisionEnter(CGameObject* pOther) override;

private:
	HRESULT			Add_Component();

protected:
	Engine::CPlyTex* m_pBufferCom;
	Engine::CTexture* m_pTextureCom;

	virtual void Destroy() override;

public:
	static CBreakableFrustum* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();
};

