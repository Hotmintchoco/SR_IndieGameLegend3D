#pragma once

#include "CGameObject.h"
#include "CMiniGame_Header.h"

namespace Engine
{
	class CRcTex;
}

class CIntro_MG1 : public CGameObject
{
protected:
	explicit CIntro_MG1(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CIntro_MG1();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

private:
	HRESULT			Add_Component();
	void			Render_Rect(_float fHalfX, _float fHalfY, DWORD dwColor);

private:
	Engine::CRcTex* m_pBufferCom = nullptr;

	_float		m_fTime = 0.f;

public:
	static CIntro_MG1* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void		Free();
};
