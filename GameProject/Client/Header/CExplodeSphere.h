#pragma once

#include "CGameObject.h"

namespace Engine
{
	class CPlyTex;
	class CTexture;
	class CTransform;
}

class CExplodeSphere : public CGameObject
{
protected:
	explicit CExplodeSphere(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CExplodeSphere();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

	inline void SetScale(const float fStartScale, const float fEndScale) { m_fStartScale = fStartScale; m_fEndScale = fEndScale; }
	inline void SetLifeTime(const float fLifeTime) { m_fLifeTime = fLifeTime; }
	void SetPos(const _vec3& vPos);

private:
	HRESULT			Add_Component();

private:
	Engine::CPlyTex* m_pBufferCom = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;
	Engine::CTransform* m_pTransformCom = nullptr;

	float m_fLifeTime = 0.f;
	float m_fAccTime = 0.f;
	float m_fStartScale = 0.f;
	float m_fEndScale = 0.f;
	float m_fCurrentScale = 0.f;

public:
	static CExplodeSphere* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CExplodeSphere* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos);
	static CExplodeSphere* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3& vPos, const float fStartScale, const float fEndScale, const float fLifeTime);

private:
	virtual void		Free();
};


