#pragma once

#include "CGameObject.h"
#include "CMiniGame_Header.h"

namespace Engine
{
	class CRcTex;
	class CTexture;
	class CTransform;
}

class CMG0_Sunglass : public CGameObject
{
protected:
	explicit CMG0_Sunglass(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMG0_Sunglass();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

	void			Update_Sunglass(_float fTimeDelta, const _matrix& matHead);

private:
	HRESULT			Add_Component();

private:
	Engine::CRcTex*		m_pBufferCom = nullptr;
	Engine::CTexture*	m_pTextureCom = nullptr;
	Engine::CTransform*	m_pTransformCom = nullptr;

	_float		m_fDropTime = 0.f;

public:
	static CMG0_Sunglass* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void		Free();
};
