#pragma once

#include "CParticle.h"

namespace Engine
{
	class CPlyTex;
	class CTexture;
}

class CParticle_Sphere : public CParticle
{
public:
	enum EFFECT_SPHERE_COLOR { BLACK, YELLOW, ORANGE, RED };
	enum EFFECT_SPHERE_OPTION {UP, DOWN, UP_DOWN};
protected:
	explicit CParticle_Sphere(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CParticle_Sphere();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

	void Set_Opacity(const _int iOpacity);
	void Set_Color(const EFFECT_SPHERE_COLOR eColor) { m_eColor = eColor; }
	void Set_ScaleUp_Speed(const _float& fSpeed) { m_fScaleUp_Speed = fSpeed; }
	void Set_ScaleDown_Speed(const _float& fSpeed) { m_fScaleDown_Speed = fSpeed; }
	void Set_Sphere_Option(const EFFECT_SPHERE_OPTION& eOption) { m_eOption = eOption; }

	void Scale_Up(const _float fTimeDelta);
	void Scale_Down(const _float fTimeDelta);

private:
	HRESULT	Add_Component();

	Engine::CPlyTex* m_pBufferCom = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;

	EFFECT_SPHERE_COLOR m_eColor = BLACK;

	_int m_iOpacity = 100;
	_float m_fScaleUp_Speed = 1.5f;
	_float m_fScaleDown_Speed = 1.5f;

	EFFECT_SPHERE_OPTION m_eOption = EFFECT_SPHERE_OPTION::UP;
public:
	static CParticle_Sphere* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, EFFECT_SPHERE_COLOR eColor, _int iOpacity);
	static CParticle_Sphere* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, EFFECT_SPHERE_COLOR eColor, _int iOpacity, _float fSpeed, _float fLifeTime);
	static CParticle_Sphere* Create(LPDIRECT3DDEVICE9 pGraphicDev, _vec3 vPos, EFFECT_SPHERE_COLOR eColor, _int iOpacity, _float fSpeed, _float fLifeTime, _vec3 vScale, EFFECT_SPHERE_OPTION eOption);

private:
	virtual void		Free();
};
