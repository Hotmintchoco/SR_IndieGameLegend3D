#pragma once

#include "CGameObject.h"
#include "Define.h"

namespace Engine
{
	class CPlaneTex;
	class CTransform;
	class CTexture;
}

class CTile : public CGameObject
{
protected:
	explicit CTile(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CTile(LPDIRECT3DDEVICE9 pGraphicDev, int iPositionIdx, int iTextureIdx);
	virtual ~CTile();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

private:
	HRESULT			Add_Component();

private:
	Engine::CPlaneTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;

	int m_iPositionIdx = -1;
	int m_iTextureIdx = -1;

public:
	static CTile* Create(LPDIRECT3DDEVICE9 pGraphicDev, int iPositionIdx, int iTextureIdx);

private:
	virtual void		Free();
};

