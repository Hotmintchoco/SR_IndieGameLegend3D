#pragma once

#include "CFrustum.h"

namespace Engine
{
	class CPlyTex;
	class CTexture;
	class CGameObject;
}

class CExplosiveFrustumLight;
class CExplosiveFrustumGlass;
class CExplodeRange;

class CExplosiveFrustum : public CFrustum
{
protected:
	explicit CExplosiveFrustum(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CExplosiveFrustum();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(_float fTimeDelta);
	virtual			void		LateUpdate_GameObject(_float fTimeDelta);
	virtual			void		Render_GameObject();


	virtual void OnCollisionEnter(COLLINFO eCollInfo) override;

private:
	HRESULT			Add_Component();

	void SpawnChildren();

private:
	Engine::CPlyTex* m_pBufferCom = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;

	CExplosiveFrustumLight* m_pLight = nullptr;
	CExplosiveFrustumGlass* m_pGlass = nullptr;
	CExplodeRange* m_pExplodeRange = nullptr;

	virtual void Destroy() override;

	/* 폭파 관련 세부값 */
	float m_fPropagateSpeed = 0.15f;
	float m_fPropagateVariance = 0.7f;
	float m_fFlickerChance = 0.7f;
	float m_fFlickerDuration = 0.015f;

public:
	static CExplosiveFrustum* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();
};

