#pragma once
#include "CUI.h"

class CGaugeUI : public CUI
{
protected:
	explicit CGaugeUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CGaugeUI();
public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

protected:
	HRESULT			Add_Component();

	void			Render_HorizontalGauge();
	void			Render_VerticalGauge();

private:
	_bool			m_bHorizontal = true; // true면 수평 게이지, false면 수직 게이지

public:
	static CGaugeUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CGaugeUI* Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& wstrTextureTag, _bool bHorizontal = true);

protected:
	virtual void		Free();
};

