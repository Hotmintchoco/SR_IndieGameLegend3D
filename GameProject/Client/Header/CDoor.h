#pragma once

#include "CGameObject.h"

namespace Engine
{
	class CRcTex;
	class CTransform;
	class CTexture;
}

class CDoor : public CGameObject
{
protected:
	explicit CDoor(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CDoor();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

	void Open();
	void Close();

private:
	HRESULT			Add_Component();

private:
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;

	int m_iTextureIdx = 4;
	bool m_bOnAnimation = false;
	const int m_iFrameCnt = 5;
	float m_fFrameInterval = 0.07f;
	float m_fSingleFrameAccTime = 0.f;
	int m_iPlayDirection = 1;

public:
	static CDoor* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();
};

