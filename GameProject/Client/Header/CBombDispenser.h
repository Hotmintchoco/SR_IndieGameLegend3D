#pragma once

#include "CGameObject.h"

namespace Engine
{
	class CTransform;
	class CBoxCollider;
	class CPlyTex;
	class CTexture;
}

class CBombDispenser : public CGameObject
{
protected:
	explicit CBombDispenser(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBombDispenser();

public:
	virtual	HRESULT	Ready_GameObject() override;
	virtual	_int Update_GameObject(_float fTimeDelta) override;
	virtual	void LateUpdate_GameObject(_float fTimeDelta) override;
	virtual	void Render_GameObject() override;

	virtual void OnCollisionEnter(COLLINFO eCollInfo) override;

protected:
	HRESULT Add_Component();

	Engine::CTransform* m_pTransformCom = nullptr;
	Engine::CBoxCollider* m_pColliderCom = nullptr;
	Engine::CPlyTex* m_pBufferCom = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;

	float m_fLaserSpawnInterval = 3.f;
	float m_fTimeAfterSpawn = 0.f;

public:
	static CBombDispenser* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void Free();
};

