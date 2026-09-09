#pragma once

#include "CGameObject.h"

namespace Engine
{
	class CPlyTex;
	class CTransform;
	class CTexture;
}

class CGun : public CGameObject
{
protected:
	explicit CGun(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CGun();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

private:
	HRESULT			Add_Component();

private:
	Engine::CPlyTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;

public:
	static CGun* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();
};

