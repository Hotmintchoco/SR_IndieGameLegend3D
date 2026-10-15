#pragma once

#include "CGameObject.h"
#include "CMiniGame_Header.h"

namespace Engine
{
	class CRcTex;
}

class CMG0_Intro : public CGameObject
{
protected:
	explicit CMG0_Intro(LPDIRECT3DDEVICE9 pGraphicDev, _bool bOutro);
	virtual ~CMG0_Intro();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

	inline _bool	Is_Finished() { return m_bOutro && m_fOutroTime >= MG_INTRO_TOTAL_TIME; }

private:
	HRESULT			Add_Component();
	void			Render_Rect(_float fHalfX, _float fHalfY, DWORD dwColor);

private:
	Engine::CRcTex*		m_pBufferCom = nullptr;

	_float		m_fTime = 0.f;
	_bool		m_bOutro = false;
	_float		m_fOutroTime = 0.f;

public:
	static CMG0_Intro* Create(LPDIRECT3DDEVICE9 pGraphicDev, _bool bOutro = false);

protected:
	virtual void		Free();
};
