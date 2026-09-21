#pragma once

#include "CGameObject.h"

namespace Engine
{
	class CPlyTex;
	class CTexture;
	class CTransform;
}

class CExplosiveFrustumGlass : public CGameObject
{
protected:
	explicit CExplosiveFrustumGlass(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CExplosiveFrustumGlass();

public:
	virtual			HRESULT		Ready_GameObject();
	virtual			_int		Update_GameObject(const _float& fTimeDelta);
	virtual			void		LateUpdate_GameObject(const _float& fTimeDelta);
	virtual			void		Render_GameObject();

	inline void AttachTo(Engine::CGameObject* pObject) { m_pParentObject = pObject; }
	void PropagateTransform(CTransform* pTransform);

private:
	HRESULT			Add_Component();

protected:
	Engine::CPlyTex* m_pBufferCom;
	Engine::CTransform* m_pTransformCom;
	Engine::CTexture* m_pTextureCom;

	Engine::CGameObject* m_pParentObject = nullptr;

public:
	static CExplosiveFrustumGlass* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
	virtual void		Free();
};

