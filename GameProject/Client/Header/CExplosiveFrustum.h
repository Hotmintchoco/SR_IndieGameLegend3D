#pragma once

#include "CFrustum.h"

namespace Engine
{
	class CPlyTex;
	class CTexture;
	class CGameObject;
}

class CExplosiveFrustumLight;

class CExplosiveFrustum : public CFrustum
{
protected:
	explicit CExplosiveFrustum(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CExplosiveFrustum();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();


	virtual void OnCollisionEnter(CGameObject* pOther) override;

private:
	HRESULT			Add_Component();

	void SpawnLight();

protected:
	Engine::CPlyTex* m_pBufferCom;
	Engine::CTexture* m_pTextureCom;

	CExplosiveFrustumLight* m_pLight = nullptr;

public:
	static CExplosiveFrustum* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();
};

