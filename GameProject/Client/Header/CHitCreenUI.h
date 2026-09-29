#pragma once
#include "CUI.h"
class CHitCreenUI : public CUI
{
protected:
	explicit CHitCreenUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CHitCreenUI();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

public:
	void			Hit() { m_fAlpha = 255.f; }

private:
	_float			m_fAlpha;

public:
	static CHitCreenUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CHitCreenUI* Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& wstrTextureTag);

protected:
	virtual void		Free();
};

