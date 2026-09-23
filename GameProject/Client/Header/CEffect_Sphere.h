#pragma once

#include "CEffect.h"

namespace Engine
{
	class CPlyTex;
	class CTexture;
}

class CEffect_Sphere : public CEffect
{
public:
	enum EFFECT_SPHERE_COLOR {BLACK, YELLOW, ORANGE, RED};
protected:
	explicit CEffect_Sphere(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CEffect_Sphere();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

	void Set_Scale(const _float fScale);
	void Set_Opacity(const _int iOpacity);
	void Set_Color(const EFFECT_SPHERE_COLOR eColor) { m_eColor = eColor; }

	void Scale_Up(const _float fTimeDelta);

private:
	HRESULT	Add_Component();

	Engine::CPlyTex* m_pBufferCom = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;

	EFFECT_SPHERE_COLOR m_eColor = BLACK;

	_int m_iOpacity = 100;
	_float m_fScale = 1.f;

public:
	static CEffect_Sphere* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, EFFECT_SPHERE_COLOR eColor, _int iOpacity);

private:
	virtual void		Free();
};
