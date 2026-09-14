#pragma once
#include "CUI.h"

class CCrosshair : public CUI
{
protected:
	explicit CCrosshair(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CCrosshair();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

protected:
	HRESULT			Add_Component();

protected:

public:
	static CCrosshair* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void		Free();
};

