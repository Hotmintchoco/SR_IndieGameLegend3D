#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CTransform;
	class CTexture;
	class CRcTex;
}

class CNpc : public CGameObject
{
protected:
	explicit CNpc(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CNpc();

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

	void BillBoard();
	_bool m_bStart = false;

public:
	static CNpc* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();
};

