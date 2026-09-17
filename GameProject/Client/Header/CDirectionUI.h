#pragma once
#include "CUI.h"

class CDirectionUI : public CUI
{
protected:
	explicit CDirectionUI(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CDirectionUI();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

protected:
	HRESULT			Add_Component();

private:
	_float			m_fCompassUOffset;

public:
	static CDirectionUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void		Free();
};

