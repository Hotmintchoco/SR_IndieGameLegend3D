#pragma once

#include "CGameObject.h"

namespace Engine
{
	class CTransform;
}

class CEffect : public CGameObject
{
public:
	enum EFFECT_TYPE {MAGMA_FIREBALL, MAGMA_TRAIL, MAGMA_DEAD_EFFECT, MAGMA_EXPLOSION1, MAGMA_EXPLOSION2, IDLE};
protected:
	explicit CEffect(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CEffect();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

protected:
	HRESULT			Add_Component();

public:
	static CEffect* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CEffect* Create(LPDIRECT3DDEVICE9 pGraphicDev, EFFECT_TYPE eEffect_Type, const _vec3& vPos);
	//static CEffect* Create(LPDIRECT3DDEVICE9 pGraphicDev, EFFECT_TYPE eEffect_Type, CGameObject* pEffect_Owner);
	void Set_Pos(const _vec3& vPos);
	void Set_Scale(const _vec3& vScale);
	void Set_LifeTime(const _float& fLifeTime) { m_fLifeTime = fLifeTime; }
	void Set_Effect_Type(EFFECT_TYPE eEffect_Type) { m_eEffect_Type = eEffect_Type; }
	//void Set_Effect_Owner(CGameObject* pEffect_Owner) { m_pEffect_Owner = pEffect_Owner; }

	void Ready_Effect();
	void Update_Effect(const _float fTimeDelta);

protected:
	Engine::CTransform* m_pTransformCom = nullptr;

	_float m_fFrame = 0.f;

	_float m_fLifeTime = 0.f;
	_float m_fElapsedTime = 0.f;
	_float m_fElapsedTime2 = 0.f;
	_float m_fElapsedTime3 = 0.f;


	EFFECT_TYPE m_eEffect_Type = IDLE;


	//CGameObject* m_pEffect_Owner = nullptr;
	
public:

protected:
	virtual void		Free();
};

#define MAGMA_DEAD_TIME 5.f