#pragma once

#include "CParticle.h"

namespace Engine
{
	class CRcTex;
	class CTexture;
}

class CAirbubble : public CParticle
{
protected:
	explicit CAirbubble(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CAirbubble();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

protected:
	HRESULT			Add_Component();
protected:
	Engine::CRcTex* m_pBufferCom = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;
public:
	static CAirbubble* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CAirbubble* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, _int iType);

	void Set_AirbubbleType(_int iType) { m_iAirbubbleType = iType; }
private:
	_int m_iAirbubbleType = 0;

protected:
	virtual void		Free();
};

