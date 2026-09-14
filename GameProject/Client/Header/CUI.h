#pragma once
#include "CGameObject.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
}

class CUI : public CGameObject
{
protected:
	explicit CUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CUI();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

public:
	void			Set_Pos(const _vec2& vPos);

protected:
	HRESULT			Add_Component();

protected:
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;

	_vec2				m_vPos;
	_float				m_fFrame;

public:
	static CUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void		Free();
};

