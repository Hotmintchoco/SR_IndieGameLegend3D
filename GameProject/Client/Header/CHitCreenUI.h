#pragma once
#include "CUI.h"
class CHitCreenUI : public CUI
{
protected:
	explicit CHitCreenUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CHitCreenUI();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

public:
	void			Hit() { m_fAlpha = 255.f; }
	void			EnterBossRoom();

private:
	_float			m_fAlpha;
	_float			m_fShowTime;

public:
	static CHitCreenUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CHitCreenUI* Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& wstrTextureTag);

protected:
	virtual void		Free();
};

