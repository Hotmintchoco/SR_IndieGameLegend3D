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

	void			Set_WindowSize(const _vec2& vSize)  { m_vWindowSize = vSize; }

protected:
	HRESULT			Add_Component();

private:
	_vec2			m_vWindowSize;

public:
	static CMinimapUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void		Free();
};

