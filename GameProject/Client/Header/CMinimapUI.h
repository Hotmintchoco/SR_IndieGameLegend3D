#pragma once
#include "CUI.h"

class CMinimapUI : public CUI
{
protected:
	explicit CMinimapUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CMinimapUI();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

protected:
	HRESULT			Add_Component();

public:
	static CMinimapUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void		Free();
};

