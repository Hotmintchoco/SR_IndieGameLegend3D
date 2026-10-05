#pragma once

#include "CGameObject.h"
#include "IRayTestable.h"

namespace Engine
{
	class CTexture;
	class CTransform;
	class CPlyTex;
	class CBoxCollider;
}

class CLiminalObject : public CGameObject, public IRayTestable
{
protected:
	explicit CLiminalObject(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLiminalObject();

public:
	virtual	HRESULT	Ready_GameObject() override;
	virtual	_int Update_GameObject(_float fTimeDelta) override;
	virtual	void LateUpdate_GameObject(_float fTimeDelta) override;
	virtual	void Render_GameObject() override;

	virtual void OnCollisionEnter(COLLINFO eCollInfo) override;

	/* IRayTestable */
	virtual vector<pair<Engine::CVIBuffer*, Engine::CTransform*>> GetRayTestTargetInfo() override;

	inline void SetGrabbed(bool bFlag) { m_bGrabbed = bFlag; }
	inline CTransform* GetTransform() { return m_pTransformCom; }

protected:
	HRESULT Add_Component();

	Engine::CTransform* m_pTransformCom = nullptr;
	Engine::CBoxCollider* m_pColliderCom = nullptr;
	Engine::CPlyTex* m_pBufferCom = nullptr;
	Engine::CTexture* m_pTextureCom = nullptr;

	/* 아웃라인 용 */
	Engine::CTexture* m_pWhiteTextureCom = nullptr;
	Engine::CTransform* m_pOutlineTransformCom = nullptr;

	bool m_bGrabbed = false;

public:
	static CLiminalObject* Create(LPDIRECT3DDEVICE9 pGraphicDev);

protected:
	virtual void Free();
};

