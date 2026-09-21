#pragma once

#include "CGameObject.h"

namespace Engine
{
	class CPlyTex;
	class CTransform;
	class CTexture;
}

class CPseudoDark : public CGameObject
{
protected:
	explicit CPseudoDark(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPseudoDark();

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
	Engine::CTransform* m_pTransformCom = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;

	int m_iOpacity = 100;
	float m_fScale = 1.f;

public:
	static CPseudoDark* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();
};
