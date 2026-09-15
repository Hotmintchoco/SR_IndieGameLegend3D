#pragma once
#include "CUI.h"

class CPlayerHpUI : public CUI
{
protected:
	explicit CPlayerHpUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPlayerHpUI();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

protected:
	HRESULT			Add_Component();

protected:

public:
	static CPlayerHpUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void		Free();
};

