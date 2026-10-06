#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CTransform;
	class CTexture;
	class CRcTex;
}

class CSeaweed : public CGameObject
{
protected:
	explicit CSeaweed(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CSeaweed();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

private:
	HRESULT			Add_Component();

	virtual void OnCollisionEnter(COLLINFO eCollInfo) override;

private:
	Engine::CTransform* m_pTransformCom = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;
	Engine::CRcTex* m_pBufferCom = nullptr;

	_float m_fFrame = 0.f;
	_bool m_bStart = false;

public:
	static CSeaweed* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();
};

