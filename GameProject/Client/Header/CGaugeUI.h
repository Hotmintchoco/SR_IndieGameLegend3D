#pragma once
#include "CUI.h"

class CGaugeUI : public CUI
{
protected:
	explicit CGaugeUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CGaugeUI();
public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

protected:
	HRESULT			Add_Component();

public:
	static CGaugeUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CGaugeUI* Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& wstrTextureTag);

protected:
	virtual void		Free();
};

