#pragma once

#include "CGameObject.h"

namespace Engine
{
	class CRcTex;
	class CTexture;
	class CTransform;
	class CGameObject;
}

class CExplosiveFrustumLight : public CGameObject
{
protected:
	explicit CExplosiveFrustumLight(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CExplosiveFrustumLight();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

	inline void AttachTo(Engine::CGameObject* pObject) { m_pParentObject = pObject; }
	void PropagateTransform(_matrix* matParent);

private:
	HRESULT			Add_Component();

protected:
	Engine::CRcTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;

	Engine::CGameObject* m_pParentObject = nullptr;

	float m_fFrameInterval = 0.1f;
	float m_fTimeLasted = 0.f;
	int m_iTextureIndex = 0;

public:
	static CExplosiveFrustumLight* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();
};

