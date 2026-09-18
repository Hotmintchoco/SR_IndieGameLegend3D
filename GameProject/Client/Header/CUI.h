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
	void			Set_Size(const _vec2& vSize);
	void            Set_Texture(const _uint& iIndex);

protected:
	HRESULT			Add_Component();

protected:
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;

	_vec2				m_vPos;
	_vec2				m_vSize;
	_float				m_fFrame;

	wstring				m_wstrTextureTag;

public:
	static CUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CUI* Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& wstrTextureTag);

protected:
	virtual void		Free();
};

