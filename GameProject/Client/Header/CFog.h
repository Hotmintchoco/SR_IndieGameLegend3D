#pragma once

#include "CGameObject.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
}

class CFog : public CGameObject
{
protected:
	explicit CFog(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CFog();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();

	inline void SetOpacity(int iOpacity) { m_iTextureIdx = iOpacity; }

private:
	HRESULT			Add_Component();

private:
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;

	float m_iTextureIdx = -1;

public:
	static CFog* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();
};

