#pragma once

#include "CEffect.h"

namespace Engine
{
	class CPlyTex;
	class CTexture;
}

class CEffect_Sphere : public CEffect
{
protected:
	explicit CEffect_Sphere(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CEffect_Sphere();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

	void SetScale(const float fScale);
	void SetOpacity(const int iOpacity);

private:
	HRESULT	Add_Component();

	Engine::CPlyTex* m_pBufferCom = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;

	int m_iOpacity = 100;
	float m_fScale = 1.f;

public:
	static CEffect_Sphere* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();
};
